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



//- IMPL. -------------------------------------------------------------------------

IKModel::IKModel(Logger* logger){
    _logger = logger;
	PRINT_LOG(_logger, "[IKModel] Configurando...\r\n");

	// inicializo acción
	_curr_pos = (IKAction_t){{0,0,0,0,0,0,0,0,0}};
    _action = _curr_pos;
    _direction = STAND;
    _last_direction = _direction;
    _command = STOP;
    _last_command = _command;
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IKModel::goStand(){
    _action = (IKAction_t){{0,0,0,0,0,0,0,0,0}};
    _direction = STAND;
    _command = MORE;
    for(uint8_t i=0;i<TrunkController::MOTOR_COUNT;i++){
        if(_curr_pos.degrees[i] >= 10){
            _action.degrees[i] -= 10;
        }
        else if(_curr_pos.degrees[i] > 0){
            _action.degrees[i] -= _curr_pos.degrees[i];
        }
    }
    return _action.degrees;
}




//------------------------------------------------------------------------------------------------                                    
int16_t* IKModel::goLeft(){
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IKModel::goRight(){
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IKModel::goUp(){
}


//------------------------------------------------------------------------------------------------                                    
int16_t* IKModel::goDown(){
}


