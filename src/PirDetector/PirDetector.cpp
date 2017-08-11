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
    
    @file          PirDetector.cpp
    @purpose       Controlador de un detector PIR de proximidad.
    @version       v1.0
    @date          Ago 2017
    @author        @raulMrello
*/

#include "PirDetector.h"

//- PRIVATE -----------------------------------------------------------------------

static void defaultCallback(){}

//- IMPL. -------------------------------------------------------------------------

PirDetector::PirDetector(PinName gpio_pir, bool activeHigh, Logger* logger){
    _logger = logger;
	PRINT_LOG(_logger, "[PirDetector] Configurando GPIOs...\r\n");

	// Inicializo resto de objetos
	_cb.attach(callback(defaultCallback));
	_logic = activeHigh;
	_enabled = false;
	
	// referencio gpio pins
	_iin = new InterruptIn(gpio_pir);
	_iin->rise(0);
	_iin->fall(0);
	
}


void PirDetector::isrHandler(){
	if(_enabled){
		_cb.call();
	}
}


void PirDetector::enable(bool flag, Callback<void()> cb){
	_cb = (cb)? callback(cb) : callback(defaultCallback);
	if(_logic){
		_iin->fall(0);
		_iin->rise(callback(this, &PirDetector::isrHandler));
	}
	else{
		_iin->rise(0);
		_iin->fall(callback(this, &PirDetector::isrHandler));
	}
	_enabled = flag;
}
