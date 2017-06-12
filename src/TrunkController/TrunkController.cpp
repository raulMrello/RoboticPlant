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

#define PRINT_LOG(format, ...)   if(_serial){_serial->printf(format, ##__VA_ARGS__);}

static void default_cb(){}


//- IMPL. -------------------------------------------------------------------------

TrunkController::TrunkController(PinName gpio_oe, PinName gpio_srclr,
								 PinName gpio_rclk, PinName gpio_srclk,
								 PinName gpio_ser, Stepper::Stepper_mode_t mode,
                                 uint8_t freq, RawSerial *serial)
								: Shifter(gpio_oe, gpio_srclr, gpio_rclk, gpio_srclk, gpio_ser, serial){

    _serial = serial;
	PRINT_LOG("[TrunkCtrl] Creando Steppers...\r\n");
	for(int i=0;i<SECTION_COUNT;i++){
		for(int j=0;j<SEGMENTS_PER_SECTION;j++){
				_steppers[i][j] = new Stepper(j+(SEGMENTS_PER_SECTION*i), Stepper::FULL_STEP, serial);
				PRINT_LOG("[TrunkCtrl] Stepper[%d][%d] con id=%d listo\r\n",i,j, (j+(SEGMENTS_PER_SECTION*i)));
			}
	}
    _cb_ready = callback(default_cb);
    _cb_step = callback(default_cb);
    
    _wait_sec = 1.0f/freq;
	
//  #warning QUITAR ESTA MODIFICACION
//  _wait_sec = 2.0f;
    
	_tmr.detach();
	_cb_tmr = callback(this, &TrunkController::nextAction);
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
void TrunkController::exec(uint16_t* degrees, bool* clockwise){
	uint8_t c = 0;
	uint16_t curr_step;
	_max_steps = 0;
	for(int i=0;i<SECTION_COUNT;i++){
		for(int j=0;j<SEGMENTS_PER_SECTION;j++){
			PRINT_LOG("[TrunkCtrl] Solicitando acción a nodo [%d][%d], array_pos[%d]...\r\n",i,j,c/2);
			_actions[i][j] = _steppers[i][j]->request(degrees[c], clockwise[c]);
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
	PRINT_LOG("[TrunkCtrl] Construyendo acción con %d pasos...\r\n", _max_steps);
	buildAction();
	PRINT_LOG("[TrunkCtrl] Escribiendo acción en Shifter\r\n");
	write(_next_action, SHIFTER_OUTPUTS);
	_tmr.attach(_cb_tmr, _wait_sec);
    _cb_step.call();
}


//------------------------------------------------------------------------------------------------                                    
void TrunkController::buildAction(bool do_next){
	uint8_t c = 0;
	for(int i=0;i<SECTION_COUNT;i++){
		for(int j=0;j<SEGMENTS_PER_SECTION;j++){
			if(do_next){
				_actions[i][j] = _steppers[i][j]->next();
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
}


//------------------------------------------------------------------------------------------------                                    
void TrunkController::nextAction(){
	_max_steps = (_max_steps > 0)? (_max_steps-1) : 0;
	buildAction(true);
	write(_next_action, SHIFTER_OUTPUTS);
     _cb_step.call();
	if(ready()){
		_tmr.detach();
		_cb_ready.call();
	}
}


