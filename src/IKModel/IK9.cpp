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
    
    @file          IK9.cpp
    @purpose       Especialización IK9 con 9 grados de libertad 3sections x 3segments
    @version       v1.0
    @date          Jun 2017
    @author        @raulMrello
*/

#include "IK9.h"

//- PRIVATE -----------------------------------------------------------------------


//- IMPL. -------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------                                    
int16_t* IK9::goStand(){
    PRINT_LOG(_logger, "[IK9] Stand: ");
    // Realizo ajuste de la inclinación y orientación
    _next_orientation = 0;
	_next_inclination = 0;
    for(uint8_t i=0;i<TrunkController::SECTION_COUNT;i++){
        _inclination[i] = 0;
    } 
    
    // Realizo la conversión oi2section en cada una de las secciones del robot
    for(int i=0; i<TrunkController::SECTION_COUNT; i++){
        oi2section(_next_orientation, _inclination[i], &_next_pos.degrees[(i * TrunkController::SEGMENTS_PER_SECTION)]);
    }    
    
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IK9::goLeft(IKLevel_enum level){
    PRINT_LOG(_logger, "[IK9] Left x %d: ", level);
    // Ajusto la orientación en función de la cantidad de movimiento deseada
    _next_orientation = _curr_orientation + (level * ORIENTATION_STEP); 
    _next_orientation = (_next_orientation < 360)? _next_orientation : (_next_orientation - 360);
    
    // Realizo la conversión oi2section en cada una de las secciones del robot
    for(int i=0; i<TrunkController::SECTION_COUNT; i++){
        oi2section(_next_orientation, _inclination[i], &_next_pos.degrees[(i * TrunkController::SEGMENTS_PER_SECTION)]);
    }    
    
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IK9::goRight(IKLevel_enum level){
    PRINT_LOG(_logger, "[IK9] Right x %d: ", level);
    // Ajusto la orientación en función de la cantidad de movimiento deseada
    _next_orientation = _curr_orientation - (level * ORIENTATION_STEP); 
    _next_orientation = (_next_orientation > 0)? _next_orientation : (360 + _next_orientation);
    
    // Realizo la conversión oi2section en cada una de las secciones del robot
    for(int i=0; i<TrunkController::SECTION_COUNT; i++){
        oi2section(_next_orientation, _inclination[i], &_next_pos.degrees[(i * TrunkController::SEGMENTS_PER_SECTION)]);
    }    
    
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IK9::goUp(IKLevel_enum level){
    int16_t inc1;
    PRINT_LOG(_logger, "[IK9] Up x %d: ", level);
    
    // Ajusto la orientación en función de la cantidad de movimiento deseada
    inc1 = _inclination[1] - (level * INCLINATION_STEP); 
    inc1 = (inc1 > TrunkController::MAX_NEGATIVE_MOTOR_ROTATION)? inc1 : TrunkController::MAX_NEGATIVE_MOTOR_ROTATION;
    
    // Dependiendo de la inclinación, habrá que mover la segunda sección y cuando ésta llegue a su límite, la primera
    if(inc1 > TrunkController::MAX_NEGATIVE_MOTOR_ROTATION){
        _inclination[1] = inc1;
        oi2section(_curr_orientation, _inclination[1], &_next_pos.degrees[(1 * TrunkController::SEGMENTS_PER_SECTION)]);
    }
    else {
        if(_inclination[1] > TrunkController::MAX_NEGATIVE_MOTOR_ROTATION){
            _inclination[1] = TrunkController::MAX_NEGATIVE_MOTOR_ROTATION;
            oi2section(_curr_orientation, _inclination[1], &_next_pos.degrees[(1 * TrunkController::SEGMENTS_PER_SECTION)]);
        }
        // Ajusto la orientación en función de la cantidad de movimiento deseada
        _inclination[0] -= (level * INCLINATION_STEP); 
        _inclination[0] = (_inclination[0] > TrunkController::MAX_NEGATIVE_MOTOR_ROTATION)? _inclination[0] : TrunkController::MAX_NEGATIVE_MOTOR_ROTATION;
        oi2section(_curr_orientation, _inclination[0], &_next_pos.degrees[0]);
    }
    
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IK9::goDown(IKLevel_enum level){
    int16_t inc0;
    PRINT_LOG(_logger, "[IK9] Down x %d: ", level);
    
    // Ajusto la orientación en función de la cantidad de movimiento deseada
    inc0 = _inclination[0] + (level * INCLINATION_STEP); 
    inc0 = (inc0 < TrunkController::MAX_POSITIVE_MOTOR_ROTATION)? inc0 : TrunkController::MAX_POSITIVE_MOTOR_ROTATION;
    
    // Dependiendo de la inclinación, habrá que mover la primera y cuando llegue a su límite, la segunda sección
    if(inc0 < TrunkController::MAX_POSITIVE_MOTOR_ROTATION){
        _inclination[0] = inc0;
        oi2section(_curr_orientation, _inclination[0], &_next_pos.degrees[0]);
    }
    else {
        // Ajusto la orientación en función de la cantidad de movimiento deseada
        if(_inclination[0] < TrunkController::MAX_POSITIVE_MOTOR_ROTATION){
            _inclination[0] = TrunkController::MAX_POSITIVE_MOTOR_ROTATION;
            oi2section(_curr_orientation, _inclination[0], &_next_pos.degrees[0]);
        }
        _inclination[1] += (level * INCLINATION_STEP); 
        _inclination[1] = (_inclination[1] < TrunkController::MAX_POSITIVE_MOTOR_ROTATION)? _inclination[1] : TrunkController::MAX_POSITIVE_MOTOR_ROTATION;
        oi2section(_curr_orientation, _inclination[1], &_next_pos.degrees[(1 * TrunkController::SEGMENTS_PER_SECTION)]);
    }
    
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IK9::headUp(IKLevel_enum level){
    PRINT_LOG(_logger, "[IK9] HeadUp x %d: ", level);
    // Ajusto la orientación en función de la cantidad de movimiento deseada
    _inclination[2] -= (level * INCLINATION_STEP); 
    _inclination[2] = (_inclination[2] > TrunkController::MAX_NEGATIVE_MOTOR_ROTATION)? _inclination[2] : TrunkController::MAX_NEGATIVE_MOTOR_ROTATION;
    
    oi2section(_curr_orientation, _inclination[2], &_next_pos.degrees[(2 * TrunkController::SEGMENTS_PER_SECTION)]);
    
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IK9::headDown(IKLevel_enum level){
    PRINT_LOG(_logger, "[IK9] HeadDown x %d: ",level);
    // Ajusto la orientación en función de la cantidad de movimiento deseada
    _inclination[2] += (level * INCLINATION_STEP); 
    _inclination[2] = (_inclination[2] < TrunkController::MAX_POSITIVE_MOTOR_ROTATION)? _inclination[2] : TrunkController::MAX_POSITIVE_MOTOR_ROTATION;
    
    oi2section(_curr_orientation, _inclination[2], &_next_pos.degrees[(2 * TrunkController::SEGMENTS_PER_SECTION)]);
        
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
bool IK9::setPosition(int16_t orientation, int16_t inclination, int16_t **action){
	bool result = false;
	int16_t dif_orient = 0;
	
	_next_orientation = orientation;
	// En primer lugar chequea si el movimiento se completa en la misma sección
	if( (_curr_orientation <= 120 && orientation <= 120) || 
		(_curr_orientation >= 120 && _curr_orientation <= 240 && orientation >= 120 && orientation <= 240) ||
		(_curr_orientation >= 240 && orientation >= 240)){
		result = true;
		dif_orient = orientation - _curr_orientation;		
	}
	else if(_curr_orientation == 0 && orientation >= 240){
		dif_orient = orientation - 360;
		result = true;
	}
	else if(_curr_orientation >= 240 && orientation == 0){
		dif_orient = 360 - _curr_orientation;
		result = true;
	}
	
	// En caso de realizar un movimiento por varias secciones, ver el sentido de giro
	else{
		dif_orient = orientation - _curr_orientation;
		// si es a izqda
		if((dif_orient >= 0 && dif_orient < 180) || dif_orient < -180){
			// Seleccionar el límite de la sección
			if(_curr_orientation < 120){
				_next_orientation = 120;
			}
			else if(_curr_orientation < 240){
				_next_orientation = 240;
			}
			else{
				_next_orientation = 360;
			}					
		}
		// a dcha
		else{
			// Seleccionar el límite de la sección
			if(_curr_orientation >= 240 || _curr_orientation == 0){
				_next_orientation = 240;
			}
			else if(_curr_orientation >= 120){
				_next_orientation = 120;
			}
			else{
				_next_orientation = 0;
			}
		}
		dif_orient = _next_orientation - _curr_orientation;	
	}
	
	// actualizo posición tras el movimiento
	_next_orientation = (_next_orientation >= 360)? (_next_orientation-360) : _next_orientation;
	_next_orientation = (_next_orientation < 0)? (360 + _next_orientation) : _next_orientation;
	
	// Actualizo inclinación
	if(inclination < TrunkController::MAX_POSITIVE_MOTOR_ROTATION){
		_inclination[2] = (inclination > TrunkController::MAX_NEGATIVE_MOTOR_ROTATION)? inclination : TrunkController::MAX_NEGATIVE_MOTOR_ROTATION;
		_inclination[1] = 0;
		_inclination[0] = 0;
	}
	else if(inclination >= TrunkController::MAX_POSITIVE_MOTOR_ROTATION && inclination < (2 * TrunkController::MAX_POSITIVE_MOTOR_ROTATION)){
		_inclination[2] = TrunkController::MAX_POSITIVE_MOTOR_ROTATION;
		_inclination[1] = inclination - TrunkController::MAX_POSITIVE_MOTOR_ROTATION;
		_inclination[0] = 0;
	}
	else {
		_inclination[2] = TrunkController::MAX_POSITIVE_MOTOR_ROTATION;
		_inclination[1] = TrunkController::MAX_POSITIVE_MOTOR_ROTATION;
		_inclination[0] = (inclination < (3 * TrunkController::MAX_POSITIVE_MOTOR_ROTATION))? (inclination - (2 * TrunkController::MAX_POSITIVE_MOTOR_ROTATION)) : TrunkController::MAX_POSITIVE_MOTOR_ROTATION;
	}
	_next_inclination = _inclination[0] + _inclination[1] + _inclination[2];
	oi2section(_next_orientation, _inclination[2], &_next_pos.degrees[(2 * TrunkController::SEGMENTS_PER_SECTION)]);
	oi2section(_next_orientation, _inclination[1], &_next_pos.degrees[(1 * TrunkController::SEGMENTS_PER_SECTION)]);
	oi2section(_next_orientation, _inclination[0], &_next_pos.degrees[0]);
    
    // Computo la acción y devuelvo resultado
    *action = computeAction();
	return result;
}




