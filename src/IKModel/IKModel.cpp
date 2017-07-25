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
    
    @file          IKModel.cpp
    @purpose       Serial to parallel shift register controller
    @version       v1.0
    @date          Jun 2017
    @author        @raulMrello
*/

#include "IKModel.h"

//- PRIVATE -----------------------------------------------------------------------

static void oi2section(int16_t odeg, int16_t ideg, int16_t* section){
    float a_comp = 0, b_comp = 0, c_comp = 0;
    
    // En primer lugar calculo en qué proporción es afectada cada componente
    // en función de la orientación odeg.
    
    // Calculo proporción de la componente A
    if(odeg >= 0 && odeg < 120){
        a_comp = ((float)(120-odeg))/120;
    }
    else if(odeg >= 240){
        a_comp = ((float)(odeg-240))/120;
    }
    
    // Calculo proporción de la componente B
    if(odeg >= 120 && odeg < 240){
        b_comp = ((float)(240-odeg))/120;
    }
    else if(odeg <= 120){
        b_comp = ((float)odeg)/120;
    }
    
    // Calculo proporción de la componente C
    if(odeg >= 240){
        c_comp = ((float)(360-odeg))/120;
    }
    else if(odeg >= 120 && odeg <= 240){
        c_comp = ((float)(odeg-120))/120;
    }
    
    // En segundo lugar, multiplico la inclinación a las proporciones anteriores
    a_comp *= ideg;
    b_comp *= ideg;
    c_comp *= ideg;
    
    // En tercer lugar, obtengo la posición deseada de cada segmento (componete) de
    // la sección
    section[0] = (int16_t)a_comp;
    section[1] = (int16_t)b_comp;
    section[2] = (int16_t)c_comp;
}

//- IMPL. -------------------------------------------------------------------------

IKModel::IKModel(Logger* logger){
    _logger = logger;
	PRINT_LOG(_logger, "[IKModel] Configurando...\r\n");

    // situo en la posición inicial (totalmente erguido)
    _curr_pos = (IKAction_t){{0,0,0,0,0,0,0,0,0}};
    _next_pos = (IKAction_t){{0,0,0,0,0,0,0,0,0}};
    _action   = (IKAction_t){{0,0,0,0,0,0,0,0,0}};
    _orientation = 0;
    for(uint8_t i=0;i<TrunkController::SECTION_COUNT;i++){
        _inclination[i] = 0;
    }    
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IKModel::goStand(){
    PRINT_LOG(_logger, "[IKModel] Stand: ");
    // Realizo ajuste de la inclinación y orientación
    _orientation = 0;
    for(uint8_t i=0;i<TrunkController::SECTION_COUNT;i++){
        _inclination[i] = 0;
    } 
    
    // Realizo la conversión oi2section en cada una de las secciones del robot
    for(int i=0; i<TrunkController::SECTION_COUNT; i++){
        oi2section(_orientation, _inclination[i], &_next_pos.degrees[(i * TrunkController::SEGMENTS_PER_SECTION)]);
    }    
    
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IKModel::goLeft(IKLevel_enum level){
    PRINT_LOG(_logger, "[IKModel] Left x %d: ", level);
    // Ajusto la orientación en función de la cantidad de movimiento deseada
    _orientation += level * ORIENTATION_STEP; 
    _orientation = (_orientation < 360)? _orientation : (_orientation - 360);
    
    // Realizo la conversión oi2section en cada una de las secciones del robot
    for(int i=0; i<TrunkController::SECTION_COUNT; i++){
        oi2section(_orientation, _inclination[i], &_next_pos.degrees[(i * TrunkController::SEGMENTS_PER_SECTION)]);
    }    
    
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IKModel::goRight(IKLevel_enum level){
    PRINT_LOG(_logger, "[IKModel] Right x %d: ", level);
    // Ajusto la orientación en función de la cantidad de movimiento deseada
    _orientation -= level * ORIENTATION_STEP; 
    _orientation = (_orientation > 0)? _orientation : (360 + _orientation);
    
    // Realizo la conversión oi2section en cada una de las secciones del robot
    for(int i=0; i<TrunkController::SECTION_COUNT; i++){
        oi2section(_orientation, _inclination[i], &_next_pos.degrees[(i * TrunkController::SEGMENTS_PER_SECTION)]);
    }    
    
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IKModel::goUp(IKLevel_enum level){
    int16_t inc1;
    PRINT_LOG(_logger, "[IKModel] Up x %d: ", level);
    
    // Ajusto la orientación en función de la cantidad de movimiento deseada
    inc1 = _inclination[1] - (level * INCLINATION_STEP); 
    inc1 = (inc1 > TrunkController::MAX_NEGATIVE_MOTOR_ROTATION)? inc1 : TrunkController::MAX_NEGATIVE_MOTOR_ROTATION;
    
    // Dependiendo de la inclinación, habrá que mover la segunda sección y cuando ésta llegue a su límite, la primera
    if(inc1 > TrunkController::MAX_NEGATIVE_MOTOR_ROTATION){
        _inclination[1] = inc1;
        oi2section(_orientation, _inclination[1], &_next_pos.degrees[(1 * TrunkController::SEGMENTS_PER_SECTION)]);
    }
    else {
        if(_inclination[1] > TrunkController::MAX_NEGATIVE_MOTOR_ROTATION){
            _inclination[1] = TrunkController::MAX_NEGATIVE_MOTOR_ROTATION;
            oi2section(_orientation, _inclination[1], &_next_pos.degrees[(1 * TrunkController::SEGMENTS_PER_SECTION)]);
        }
        // Ajusto la orientación en función de la cantidad de movimiento deseada
        _inclination[0] -= (level * INCLINATION_STEP); 
        _inclination[0] = (_inclination[0] > TrunkController::MAX_NEGATIVE_MOTOR_ROTATION)? _inclination[0] : TrunkController::MAX_NEGATIVE_MOTOR_ROTATION;
        oi2section(_orientation, _inclination[0], &_next_pos.degrees[0]);
    }
    
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IKModel::goDown(IKLevel_enum level){
    int16_t inc0;
    PRINT_LOG(_logger, "[IKModel] Down x %d: ", level);
    
    // Ajusto la orientación en función de la cantidad de movimiento deseada
    inc0 = _inclination[0] + (level * INCLINATION_STEP); 
    inc0 = (inc0 < TrunkController::MAX_POSITIVE_MOTOR_ROTATION)? inc0 : TrunkController::MAX_POSITIVE_MOTOR_ROTATION;
    
    // Dependiendo de la inclinación, habrá que mover la primera y cuando llegue a su límite, la segunda sección
    if(inc0 < TrunkController::MAX_POSITIVE_MOTOR_ROTATION){
        _inclination[0] = inc0;
        oi2section(_orientation, _inclination[0], &_next_pos.degrees[0]);
    }
    else {
        // Ajusto la orientación en función de la cantidad de movimiento deseada
        if(_inclination[0] < TrunkController::MAX_POSITIVE_MOTOR_ROTATION){
            _inclination[0] = TrunkController::MAX_POSITIVE_MOTOR_ROTATION;
            oi2section(_orientation, _inclination[0], &_next_pos.degrees[0]);
        }
        _inclination[1] += (level * INCLINATION_STEP); 
        _inclination[1] = (_inclination[1] < TrunkController::MAX_POSITIVE_MOTOR_ROTATION)? _inclination[1] : TrunkController::MAX_POSITIVE_MOTOR_ROTATION;
        oi2section(_orientation, _inclination[1], &_next_pos.degrees[(1 * TrunkController::SEGMENTS_PER_SECTION)]);
    }
    
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IKModel::headUp(IKLevel_enum level){
    PRINT_LOG(_logger, "[IKModel] HeadUp x %d: ", level);
    // Ajusto la orientación en función de la cantidad de movimiento deseada
    _inclination[2] -= (level * INCLINATION_STEP); 
    _inclination[2] = (_inclination[2] > TrunkController::MAX_NEGATIVE_MOTOR_ROTATION)? _inclination[2] : TrunkController::MAX_NEGATIVE_MOTOR_ROTATION;
    
    oi2section(_orientation, _inclination[2], &_next_pos.degrees[(2 * TrunkController::SEGMENTS_PER_SECTION)]);
    
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IKModel::headDown(IKLevel_enum level){
    PRINT_LOG(_logger, "[IKModel] HeadDown x %d: ",level);
    // Ajusto la orientación en función de la cantidad de movimiento deseada
    _inclination[2] += (level * INCLINATION_STEP); 
    _inclination[2] = (_inclination[2] < TrunkController::MAX_POSITIVE_MOTOR_ROTATION)? _inclination[2] : TrunkController::MAX_POSITIVE_MOTOR_ROTATION;
    
    oi2section(_orientation, _inclination[2], &_next_pos.degrees[(2 * TrunkController::SEGMENTS_PER_SECTION)]);
        
    // Computo la acción y devuelvo resultado
    return computeAction();
}

//------------------------------------------------------------------------------------------------                                    
void IKModel::update(){
    for(uint8_t i=0;i<TrunkController::MOTOR_COUNT;i++){
        _curr_pos.degrees[i] = _next_pos.degrees[i];
    }    
    PRINT_LOG(_logger, "[IKModel] Position: [%d,%d,%d] [%d,%d,%d] [%d,%d,%d]\r\n",
                _curr_pos.degrees[0],_curr_pos.degrees[1],_curr_pos.degrees[2],
                _curr_pos.degrees[3],_curr_pos.degrees[4],_curr_pos.degrees[5],
                _curr_pos.degrees[6],_curr_pos.degrees[7],_curr_pos.degrees[8]);
    
}

//------------------------------------------------------------------------------------------------                                    
int16_t* IKModel::computeAction(){    
    // Calculo disparidad de la posición deseada con la actual
    bool pending = false;
    for(uint8_t i=0;i<TrunkController::MOTOR_COUNT;i++){
        _action.degrees[i] = 0;
        if(_curr_pos.degrees[i] != _next_pos.degrees[i]){
            _action.degrees[i] = _next_pos.degrees[i] - _curr_pos.degrees[i];
            pending = true;
        }
    }
    PRINT_LOG(_logger, "[%d,%d,%d] [%d,%d,%d] [%d,%d,%d]\r\n",
                _action.degrees[0],_action.degrees[1],_action.degrees[2],
                _action.degrees[3],_action.degrees[4],_action.degrees[5],
                _action.degrees[6],_action.degrees[7],_action.degrees[8]);
    
    // Chequeo si el movimiento se ha completado, en ese caso devuelve NULL
    if(!pending){
        return (int16_t*)0;
    }
    
    // En caso contrario, devuelve la acción a realizar
    return _action.degrees;
}





