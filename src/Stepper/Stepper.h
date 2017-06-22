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
    
    @file          Stepper.h
    @purpose       Stepper motor controller
    @version       v1.0
    @date          Jun 2017
    @author        @raulMrello
*/

#ifndef __STEPPER_H
#define __STEPPER_H

#include "mbed.h"
#include "Logger.h"

/**
 * @author raulMrello
 * @see API 
 *
 * <b>Stepper</b> Clase encargada de generar las siguientes activaciones de un motor paso a paso, basadas en una orden
 * previamente solicitada. Las activaciones tendr�n en cuenta el modo de trabajo del motor para saber el n�mero de pasos
 * a realizar, as� como la configuraci�n de los pines (wave_drive, full_step o half_step).
 */
class Stepper {
public:
	typedef enum{
		WAVE_DRIVE,
		FULL_STEP,
		HALF_STEP
	}Stepper_mode_t;
    
    /** Stepper()
     *
     * Constructor.
     * Configura el modo de funcionamiento, por defecto FULL_STEP y asigna un identificador
     * @param id Identificador
     * @param mode	Modo de funcionamiento
     * @param logger Objeto logger    
     */
	Stepper(uint8_t id, Stepper_mode_t mode = Stepper::FULL_STEP, Logger* logger = 0);

    /** ready()
     *
     * Obtiene el estado del driver, indicando si est� listo para una nueva operaci�n o si por lo
     * contrario, est� ocupado, realizando una pendiente.
     * @return Estado actual [true|false] [listo|ocupado]
     */
	bool ready(){return ((_steps > 0)? false : true);}

    /** request()
     *
     * Solicita un nuevo movimiento basado en un n�mero de grados y un sentido concreto
     * @param degrees N�mero de grados a girar
     * @param clockwise Flag para indicar el sentido de giro
     * @return Estado actual de las salidas a las que conectar el driver (x x x x IN1 IN2 IN3 IN4)
     */
	uint8_t request(uint16_t degrees, bool clockwise);

    /** next()
     *
     * Obtiene la siguiente configuraci�n de las salidas en la operaci�n en curso. En caso de que
     * la operaci�n haya conclu�do, se obtendr� siempre el estado final.
     * @return Estado actual de las salidas a las que conectar el driver (x x x x IN1 IN2 IN3 IN4)
     */
	uint8_t next();

    /** getSteps()
     *
     * Obtiene el n�mero de pasos que quedan para finalizar la orden actual
     * @return N�mero de pasos pendientes
     */
	uint16_t getSteps(){return _steps;}
protected:
	uint8_t _id;
	bool _clockwise;
	uint16_t _steps;
	Stepper_mode_t _mode;
	uint8_t _step;
	const uint8_t * _sequence;
    Logger* _logger;
private:
};

#endif
