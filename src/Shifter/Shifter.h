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
    
    @file          Shifter.h
    @purpose       Serial to parallel shift register controller
    @version       v1.0
    @date          Jun 2017
    @author        @raulMrello
*/

#ifndef __SHIFTER_H
#define __SHIFTER_H

#include "mbed.h"
#include "Logger.h"

/**
 * @author raulMrello
 * @see API 
 *
 * <b>Shifter</b> Clase que paraleliza datos seria, controlando una serie de registros dedesplazamiento
 * conectados en cascada, para conseguir un número de salidas N en paralelo y sincronizadas.
 */
class Shifter {
public:
    
    /** Shifter()
     *
     * Constructor.
     * Configura los gpio necesarios para controlar el registro de desplazamiento
     * @param gpio_oe	Salida para controla el registro de salida a los pines físicos /OE
     * @param gpio_srclr Salida para controlar el reset del registro de desplazamiento /SRCLR
     * @param gpio_rclk Salida de reloj del registro de desplazamiento RCLK _/\_
     * @param gpio_srclk Salida de reloj del registro de acumulación SRCLK _/\_
     * @param gpio_ser Salida para inyectar datos serie
     * @param logger Objeto logger
     */
	Shifter(PinName gpio_oe, PinName gpio_srclr, PinName gpio_rclk, PinName gpio_srclk, PinName gpio_ser, Logger* logger);

    /** write()
     *
     * Paraleliza un stream de datos serie, de tamaño "count"
     * @param data	Stream de datos para actualizar las salidas del registro a un valor deseado.
     * @param count Número de bits del registro a controlar
     */
	void write(uint8_t* data, uint8_t count);

protected:
	DigitalOut* _out_oe;
	DigitalOut* _out_srclr;
	DigitalOut* _out_rclk;
	DigitalOut* _out_srclk;
	DigitalOut* _out_ser;
    Logger* _logger;
private:
};

#endif
