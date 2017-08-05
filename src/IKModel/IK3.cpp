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
    
    @file          IK3.cpp
    @purpose       Especialización IK3 con 3 grados de libertad 1sections x 3segments
    @version       v1.0
    @date          Jun 2017
    @author        @raulMrello
*/

#include "IK3.h"

//- PRIVATE -----------------------------------------------------------------------


//- IMPL. -------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------                                    
int16_t* IK3::goStand(){
    PRINT_LOG(_logger, "[IK3] Stand: ");
    // Realizo ajuste de la inclinación y orientación
    _orientation = 0;
    _inclination[_sect] = 0;
    oi2section(_orientation, _inclination[_sect], &_next_pos.degrees[(_sect * TrunkController::SEGMENTS_PER_SECTION)]);
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IK3::goLeft(IKLevel_enum level){
    PRINT_LOG(_logger, "[IK3] Left x %d: ", level);
    // Ajusto la orientación en función de la cantidad de movimiento deseada
    _orientation += level * ORIENTATION_STEP; 
    _orientation = (_orientation < 360)? _orientation : (_orientation - 360);
    
    // Realizo la conversión oi2section en cada una de las secciones del robot
    oi2section(_orientation, _inclination[_sect], &_next_pos.degrees[(_sect * TrunkController::SEGMENTS_PER_SECTION)]);
    
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IK3::goRight(IKLevel_enum level){
    PRINT_LOG(_logger, "[IK3] Right x %d: ", level);
    // Ajusto la orientación en función de la cantidad de movimiento deseada
    _orientation -= level * ORIENTATION_STEP; 
    _orientation = (_orientation > 0)? _orientation : (360 + _orientation);
    
    // Realizo la conversión oi2section en cada una de las secciones del robot
    oi2section(_orientation, _inclination[_sect], &_next_pos.degrees[(_sect * TrunkController::SEGMENTS_PER_SECTION)]);
    
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IK3::goUp(IKLevel_enum level){
    PRINT_LOG(_logger, "[IK3] Up x %d: ", level);
    
    // Ajusto la orientación en función de la cantidad de movimiento deseada
    _inclination[_sect] -= (level * INCLINATION_STEP); 
    _inclination[_sect] = (_inclination[_sect] > TrunkController::MAX_NEGATIVE_MOTOR_ROTATION)? _inclination[_sect] : TrunkController::MAX_NEGATIVE_MOTOR_ROTATION;
    
    oi2section(_orientation, _inclination[_sect], &_next_pos.degrees[(_sect * TrunkController::SEGMENTS_PER_SECTION)]);
    
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IK3::goDown(IKLevel_enum level){
    PRINT_LOG(_logger, "[IK3] Down x %d: ", level);
    
    // Ajusto la orientación en función de la cantidad de movimiento deseada
    _inclination[_sect] += (level * INCLINATION_STEP); 
    _inclination[_sect] = (_inclination[_sect] < TrunkController::MAX_POSITIVE_MOTOR_ROTATION)? _inclination[_sect] : TrunkController::MAX_POSITIVE_MOTOR_ROTATION;
    
    oi2section(_orientation, _inclination[_sect], &_next_pos.degrees[(_sect * TrunkController::SEGMENTS_PER_SECTION)]);
    
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IK3::headUp(IKLevel_enum level){
    PRINT_LOG(_logger, "[IK3] HeadUp x %d: ", level);
	return goUp(level);
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IK3::headDown(IKLevel_enum level){
    PRINT_LOG(_logger, "[IK3] HeadDown x %d: ",level);
	return goDown(level);
}




