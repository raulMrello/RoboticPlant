/*
    Copyright (c) 2017 @raulMrello
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
 
    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
 
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
    
    @file          TrunkController.cpp
    @purpose       Hyper-redundant segmented trunk robot controller
    @version       v1.0
    @date          Jun 2017
    @author        @raulMrello
*/

#include "TrunkController.h"

//- PRIVATE -----------------------------------------------------------------------


/** Callback de propósito general 'dummy' */
static void default_cb(){}


//- PUBLIC ------------------------------------------------------------------------

TrunkController::TrunkController(PinName gpio_oe, PinName gpio_srclr,
								 PinName gpio_rclk, PinName gpio_srclk,
								 PinName gpio_ser, Stepper::Stepper_mode_t mode,
                                 uint8_t freq, Logger *logger)
								: Shifter(gpio_oe, gpio_srclr, gpio_rclk, gpio_srclk, gpio_ser, logger){
    // Instalo serial logger
    _logger = logger;
	PRINT_LOG(_logger, "[TrunkCtrl] Creando Steppers...\r\n");
                                    
    // Inicializo motores paso a paso por segmentos
	for(int i=0;i<SECTION_COUNT;i++){
		for(int j=0;j<SEGMENTS_PER_SECTION;j++){
				_steppers[i][j] = new Stepper(j+(SEGMENTS_PER_SECTION*i), Stepper::FULL_STEP/*, logger*/);
                _steppers[i][j]->setRange(MAX_POSITIVE_MOTOR_ROTATION, MAX_NEGATIVE_MOTOR_ROTATION);
				PRINT_LOG(_logger, "[TrunkCtrl] Stepper[%d][%d] con id=%d listo\r\n",i,j, (j+(SEGMENTS_PER_SECTION*i)));
			}
	}
    
    // Inicializo callbacks de notificación de fin de paso
    _cb_step = callback(default_cb);
    _cb_ready = callback(default_cb);
    
    // Establezco periodo de refresco de los motores (max 100Hz)
    _wait_sec = 1.0f/freq;
	
    // Inicializo callback para la ejecución temporizada
	_tmr.detach();
	_cb_tmr = callback(this, &TrunkController::nextAction);
    
    // Inicializo y arranco thread de control
    _th = new Thread();    
    _th->start(callback(this, &TrunkController::task));
}


//------------------------------------------------------------------------------------------------                                    
bool TrunkController::ready(){
	for(int i=0;i<SECTION_COUNT;i++){
		for(int j=0;j<SEGMENTS_PER_SECTION;j++){
			if(!_steppers[i][j]->ready()){
				return false;
			}
		}
	}
	return true;
}


//------------------------------------------------------------------------------------------------                                    
void TrunkController::notifyReady(Callback<void()> cb_ready){
	_cb_ready = (cb_ready)? cb_ready : callback(default_cb);
}


//------------------------------------------------------------------------------------------------                                    
void TrunkController::notifyStep(Callback<void()> cb_step){
	_cb_step = (cb_step)? cb_step : callback(default_cb);
}


//------------------------------------------------------------------------------------------------                                    
bool TrunkController::actionRequested(int16_t* degrees){
    Action_t* a = (Action_t*)_mail.alloc();
    if(!a){
        return false;
    }
	for(int i=0; i<SECTION_COUNT; i++){
        for(int j=0;j<SEGMENTS_PER_SECTION;j++){
            a->degrees[i][j] = *degrees;
            degrees++;
        }
    }
    if(_mail.put(a) != osOK){
        _mail.free(a);
        return false;
    }
    return true;
}


//------------------------------------------------------------------------------------------------                                    
void TrunkController::task(){
    PRINT_LOG(_logger, "[TrunkCtrl] Thread %d running...\r\n", _th->gettid());
    
    // inicialmente esperará hasta que llegue alguna acción
    uint32_t timeout = osWaitForever;
    
    // inicia el contador de acciones ejecutadas
    _action_count = 0;
    
    // inicia el flag de cancelación de acciones
    bool cancelled = false;
    
    for(;;){
        osEvent oe = _mail.get(timeout);
        if(oe.status == osEventMail && oe.value.p != 0){
            Action_t* a = (Action_t*)oe.value.p;
			_th->signal_clr(0xffff);
            // si la acción se permite, se ejecuta. En caso contrario, se descarta
            if(!cancelled){
                // inicia la acción que se completa a través de las callbacks de ISR
                exec((int16_t*)a->degrees);
                // espera el final de la acción
                osEvent oe_sig = _th->signal_wait(TYPE_SIGNAL_ACTION_COMPLETED, osWaitForever);
                // una vez completada...
                if((oe_sig.value.signals & TYPE_SIGNAL_ACTION_COMPLETED) != 0){
                    // incrementa el contador
                    _action_count++;
                    // y vuelve al bucle principal para ver si hay más acciones pendientes 
                    // para ello, elimina el timeout de espera.                    
                    timeout = 0;
                }  
                // si se ha solicitado la cancelación del resto de acciones...
                oe_sig = _th->signal_wait(TYPE_SIGNAL_ACTION_CANCELLED, 0);
                if((oe_sig.value.signals & TYPE_SIGNAL_ACTION_CANCELLED) != 0){
                    // marca el flag
                    cancelled = true;
                    timeout = 0;
                    PRINT_LOG(_logger, "[TrunkCtrl] Resto de Acciones CANCELADAS!!\r\n");
                }
            }     
            // si la acción se ha cancelado...
            else{
                // y vuelve al bucle principal para ver si hay más acciones pendientes 
                // para ello, elimina el timeout de espera.                    
                timeout = 0;
                PRINT_LOG(_logger, "[TrunkCtrl] Acción descartada.\r\n");
            }   
            // libera la memoria del mensaje procesado
            _mail.free(a);
        }
        // si no quedan más acciones pendientes, notifica y vuelve a restaurar el timeout y el flag de cancelación
        else{
            timeout = osWaitForever;
            cancelled = false;
            _cb_ready.call();            
        }
    }
}


//- PRIVATE --------------------------------------------------------------------------------------


void TrunkController::exec(int16_t* degrees){
	uint8_t c = 0;
	uint16_t curr_step;
	_max_steps = 0;
	for(int i=0;i<SECTION_COUNT;i++){
		for(int j=0;j<SEGMENTS_PER_SECTION;j++){
			//PRINT_LOG("[TrunkCtrl] Solicitando acción a nodo [%d][%d], array_pos[%d]...\r\n",i,j,c/2);
            uint16_t u16deg = (*degrees >= 0)? (uint16_t)(*degrees) : (uint16_t)(-(*degrees));
            bool clockwise = (*degrees >= 0)? true : false;
            degrees++;
			_actions[i][j] = _steppers[i][j]->request(u16deg, clockwise);
			curr_step = _steppers[i][j]->getSteps();
			_max_steps = (curr_step > _max_steps)? curr_step : _max_steps;
			if((c & 1) == 0){
				_next_action[c/2] &= 0x0f;
				_next_action[c/2] |= (_actions[i][j] << 4);
			}
			else{
				_next_action[c/2] &= 0xf0;
				_next_action[c/2] |= _actions[i][j];
			}
			c++;
		}
	}
	PRINT_LOG(_logger, "[TrunkCtrl] Construyendo acción con %d pasos...\r\n", _max_steps);
	if(!buildAction()){
        PRINT_LOG(_logger, "[TrunkCtrl] ERROR Stepper fuera de rango\r\n");
        return;
    }
	PRINT_LOG(_logger, "[TrunkCtrl] Escribiendo acción en Shifter\r\n");
	write(_next_action, SHIFTER_OUTPUTS);
	_tmr.attach(_cb_tmr, _wait_sec);
    _cb_step.call();
}


//------------------------------------------------------------------------------------------------                                    
bool TrunkController::buildAction(bool do_next){
	uint8_t c = 0;
    bool result = true;
	for(int i=0;i<SECTION_COUNT;i++){
		for(int j=0;j<SEGMENTS_PER_SECTION;j++){
			if(do_next){
				_actions[i][j] = _steppers[i][j]->next();
                if(_steppers[i][j]->isOOL(true, true)){
                    result = false;
                }
			}
			if((c & 1) == 0){
				_next_action[c/2] &= 0x0f;
				_next_action[c/2] |= (_actions[i][j] << 4);
			}
			else{
				_next_action[c/2] &= 0xf0;
				_next_action[c/2] |= _actions[i][j];
			}
			c++;
		}
	}
    return result;
}


//------------------------------------------------------------------------------------------------                                    
void TrunkController::nextAction(){
	_max_steps = (_max_steps > 0)? (_max_steps-1) : 0;
	if(!buildAction(true)){
        _max_steps = 0;
        _th->signal_set(TYPE_SIGNAL_ACTION_COMPLETED);
		_tmr.detach();
        return;
    }
	write(_next_action, SHIFTER_OUTPUTS);
     _cb_step.call();
	if(ready()){
        _th->signal_set(TYPE_SIGNAL_ACTION_COMPLETED);
		_tmr.detach();
	}
}

