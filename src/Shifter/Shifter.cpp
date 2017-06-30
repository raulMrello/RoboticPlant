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
    
    @file          Shifter.cpp
    @purpose       Serial to parallel shift register controller
    @version       v1.0
    @date          Jun 2017
    @author        @raulMrello
*/

#include "Shifter.h"

//- PRIVATE -----------------------------------------------------------------------



//- IMPL. -------------------------------------------------------------------------

Shifter::Shifter(PinName gpio_oe, PinName gpio_srclr, PinName gpio_rclk, PinName gpio_srclk, PinName gpio_ser, Logger* logger){
    _logger = logger;
	PRINT_LOG(_logger, "[Shifter] Configurando GPIOs...\r\n");

	// referencio gpio pins
	_out_oe = new DigitalOut(gpio_oe);
	_out_srclr = new DigitalOut(gpio_srclr);
	_out_rclk = new DigitalOut(gpio_rclk);
	_out_srclk = new DigitalOut(gpio_srclk);
	_out_ser = new DigitalOut(gpio_ser);

	// Inicializo desactivando salidas, reseteando y preparando relojes
	_out_oe->write(1);
	_out_srclr->write(1);
	_out_rclk->write(0);
	_out_srclk->write(0);
	_out_ser->write(0);
}


void Shifter::write(uint8_t* data, uint8_t count){
	const uint8_t mask[]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

	// Cargo valores en los registros de desplazamiento
    
	for(int i=0;i<count;i++){
        volatile uint8_t val = (data)? ((data[i/8] & mask[i%8])? 1 : 0) : 0;
		_out_ser->write(val);
		_out_srclk->write(1);
		_out_srclk->write(0);
	}

	// Cargo valores en los registro de carga
	_out_rclk->write(1);
	_out_rclk->write(0);
    
	// habilito salidas (puede no tener efecto si ya est? previamente a 0).
	_out_oe->write(0);
}
