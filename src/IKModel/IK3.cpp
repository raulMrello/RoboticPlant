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
    _next_orientation = 0;
	_next_inclination = 0;
    _inclination[_sect] = 0;
    oi2section(_next_orientation, _inclination[_sect], &_next_pos.degrees[(_sect * TrunkController::SEGMENTS_PER_SECTION)]);
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IK3::goLeft(IKLevel_enum level){
    PRINT_LOG(_logger, "[IK3] Left x %d: ", level);
    // Ajusto la orientación en función de la cantidad de movimiento deseada
    _next_orientation = _curr_orientation + (level * ORIENTATION_STEP); 
    _next_orientation = (_next_orientation < 360)? _next_orientation : (_next_orientation - 360);
    
    // Realizo la conversión oi2section en cada una de las secciones del robot
    oi2section(_next_orientation, _inclination[_sect], &_next_pos.degrees[(_sect * TrunkController::SEGMENTS_PER_SECTION)]);
    
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IK3::goRight(IKLevel_enum level){
    PRINT_LOG(_logger, "[IK3] Right x %d: ", level);
    // Ajusto la orientación en función de la cantidad de movimiento deseada
    _next_orientation = _curr_orientation - (level * ORIENTATION_STEP); 
    _next_orientation = (_next_orientation > 0)? _next_orientation : (360 + _next_orientation);
    
    // Realizo la conversión oi2section en cada una de las secciones del robot
    oi2section(_next_orientation, _inclination[_sect], &_next_pos.degrees[(_sect * TrunkController::SEGMENTS_PER_SECTION)]);
    
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IK3::goUp(IKLevel_enum level){
    PRINT_LOG(_logger, "[IK3] Up x %d: ", level);
    
    // Ajusto la orientación en función de la cantidad de movimiento deseada
    _inclination[_sect] -= (level * INCLINATION_STEP); 
    _inclination[_sect] = (_inclination[_sect] > TrunkController::MAX_NEGATIVE_MOTOR_ROTATION)? _inclination[_sect] : TrunkController::MAX_NEGATIVE_MOTOR_ROTATION;
    
    oi2section(_curr_orientation, _inclination[_sect], &_next_pos.degrees[(_sect * TrunkController::SEGMENTS_PER_SECTION)]);
    
    // Computo la acción y devuelvo resultado
    return computeAction();
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IK3::goDown(IKLevel_enum level){
    PRINT_LOG(_logger, "[IK3] Down x %d: ", level);
    
    // Ajusto la orientación en función de la cantidad de movimiento deseada
    _inclination[_sect] += (level * INCLINATION_STEP); 
    _inclination[_sect] = (_inclination[_sect] < TrunkController::MAX_POSITIVE_MOTOR_ROTATION)? _inclination[_sect] : TrunkController::MAX_POSITIVE_MOTOR_ROTATION;
    
    oi2section(_curr_orientation, _inclination[_sect], &_next_pos.degrees[(_sect * TrunkController::SEGMENTS_PER_SECTION)]);
    
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


//------------------------------------------------------------------------------------------------                                    
bool IK3::setPosition(int16_t orientation, int16_t inclination, int16_t** action){
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
	
	inclination = (inclination < TrunkController::MAX_POSITIVE_MOTOR_ROTATION)? inclination : TrunkController::MAX_POSITIVE_MOTOR_ROTATION;
	inclination = (inclination > TrunkController::MAX_NEGATIVE_MOTOR_ROTATION)? inclination : TrunkController::MAX_NEGATIVE_MOTOR_ROTATION;
    int16_t dif_incl = inclination - _inclination[_sect];
	_inclination[_sect] += dif_incl;
	_next_inclination = _inclination[_sect];
	oi2section(_next_orientation, _inclination[_sect], &_next_pos.degrees[(_sect * TrunkController::SEGMENTS_PER_SECTION)]);
    
    // Computo la acción y devuelvo resultado
    *action = computeAction();
	return result;
}



