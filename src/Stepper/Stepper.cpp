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
    
    @file          Stepper.cpp
    @purpose       Stepper motor controller
    @version       v1.0
    @date          Jun 2017
    @author        @raulMrello
*/

#include "Stepper.h"

//- PRIVATE -----------------------------------------------------------------------

#define STEP_RESOLUTION     0.087890625f        // (360º)/4096steps

const uint8_t wave_drive[]={0x08, 0x04, 0x02, 0x01, 0x08, 0x04, 0x02, 0x01};
const uint8_t full_step[] ={0x0c, 0x06, 0x03, 0x09, 0x0c, 0x06, 0x03, 0x09};
const uint8_t half_step[] ={0x08, 0x0c, 0x04, 0x06, 0x02, 0x03, 0x01, 0x09};


//- IMPL. -------------------------------------------------------------------------

Stepper::Stepper(uint8_t id, Stepper_mode_t mode, Logger* logger){
    _logger = logger;
	PRINT_LOG(_logger, "[Stepper %d]  Configurando...\r\n",id);

	_id = id;
	_steps = 0;
    _max_deg_limit = MAX_DEG_DEFAULT;
    _min_deg_limit = MIN_DEG_DEFAULT;
    _degrees = 0; 
	_clockwise = true;
	_step = 0;
	_sequence = full_step;
	_mode = mode;
	if(mode == Stepper::WAVE_DRIVE){
		_sequence = wave_drive;
	}
	else if(mode == Stepper::HALF_STEP){
		_sequence = half_step;
	}
	PRINT_LOG(_logger, "[Stepper %d]  Listo\r\n", _id);
}


uint8_t Stepper::request(uint16_t degrees, bool clockwise){
	_clockwise = clockwise;
	if(_mode == Stepper::HALF_STEP){
		_steps = ((degrees * 4096)/360);
	}
	else{
		_steps = ((degrees * 2048)/360);
	}
	return _sequence[_step];
}


uint8_t Stepper::next(){
	if(!_steps){
		PRINT_LOG(_logger, "[Stepper %d]  NO MAS PASOS\r\n",_id);
		return _sequence[_step];
	}
    if(isOOL()){
        _steps = 0;
        PRINT_LOG(_logger, "[Stepper %d]  OUT OF RANGE\r\n",_id);
		return _sequence[_step];
    }
	_steps--;
	if(_clockwise){
		_step = (_step < 7)? (_step+1) : 0;
        _degrees += STEP_RESOLUTION;
	}
	else{
		_step = (_step > 0)? (_step-1) : 7;
        _degrees -= STEP_RESOLUTION;
	}
	PRINT_LOG(_logger, "[Stepper %d] Ang=%f  Quedan %d pasos\r\n", _id, _degrees, _steps);
	return _sequence[_step];
}


bool Stepper::isOOL(){
    int16_t ideg = (int16_t)_degrees;
    if(ideg <= _min_deg_limit || ideg >= _max_deg_limit){
        return true;
    }
    return false;
}
