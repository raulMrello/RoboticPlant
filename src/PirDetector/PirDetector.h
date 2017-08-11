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
    
    @file          PirDetector.h
    @purpose       Controlador de un detector PIR de proximidad.
    @version       v1.0
    @date          Ago 2017
    @author        @raulMrello
*/

#ifndef __PIRDETECTOR_H
#define __PIRDETECTOR_H

#include "mbed.h"
#include "Logger.h"

/**
 * @author raulMrello
 * @see API 
 *
 * <b>PirDetector</b> Clase que notifica eventos de detección en un sensor PIR de proximidad.
 */
class PirDetector {
public:
    
    /** PirDetector()
     *
     * Constructor.
     * Configura los gpio necesarios para controlar el registro de desplazamiento
     * @param gpio_pir Entrada del sensor PIR
     * @param activeHigh Nivel lógico de la detección True=nivel_alto 
     * @param logger Objeto logger
     */
	PirDetector(PinName gpio_pir, bool activeHigh, Logger* logger);

    /** enable()
     *
     * Habilita o deshabilita  la notificación de enventos
     * @param flag Flag para activar o desactivar la notificación
     */
	void enable(bool flag, Callback<void()> cb=0);

    /** getValue()
     *
     * Lee el estado de detección
     * @return Valor de detección 0 o 1.
     */
	int getValue(){ return _iin->read();}
	
    /** isrHandler()
     *
     * Manejador propio de las interrupciones de detección
     */
	void isrHandler();		
protected:
	InterruptIn* _iin;
	bool _logic;
	bool _enabled;
	Callback<void()> _cb;
    Logger* _logger;
private:
};

#endif
