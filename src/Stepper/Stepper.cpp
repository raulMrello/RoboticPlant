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

#define PRINT_LOG(format, ...)   //if(_serial){_serial->printf(format, ##__VA_ARGS__);}

const uint8_t wave_drive[]={0x08, 0x04, 0x02, 0x01, 0x08, 0x04, 0x02, 0x01};
const uint8_t full_step[] ={0x0c, 0x06, 0x03, 0x09, 0x0c, 0x06, 0x03, 0x09};
const uint8_t half_step[] ={0x08, 0x0c, 0x04, 0x06, 0x02, 0x03, 0x01, 0x09};


//- IMPL. -------------------------------------------------------------------------

Stepper::Stepper(uint8_t id, Stepper_mode_t mode, RawSerial* serial){
    _serial = serial;
	PRINT_LOG("[Stepper %d]  Configurando...\r\n",id);

	_id = id;
	_steps = 0;
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
	PRINT_LOG("[Stepper %d]  Listo\r\n", _id);
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
		PRINT_LOG("[Stepper %d]  NO MAS PASOS\r\n",_id);
		return _sequence[_step];
	}
	_steps--;
	PRINT_LOG("[Stepper %d]  Quedan %d pasos\r\n", _id, _steps);
	if(_clockwise){
		_step = (_step < 7)? (_step+1) : 0;
	}
	else{
		_step = (_step > 0)? (_step-1) : 7;
	}
	return _sequence[_step];
}
