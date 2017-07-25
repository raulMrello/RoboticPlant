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
 * previamente solicitada. Las activaciones tendrán en cuenta el modo de trabajo del motor para saber el número de pasos
 * a realizar, así como la configuración de los pines (wave_drive, full_step o half_step).
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

    /** setRange()
     *
     * Establece el rango angular permitido
     * @param maxdeg Límite positivo
     * @param mindeg Límite negativo
     */
	void setRange(int16_t maxdeg, int16_t mindeg){_max_deg_limit = maxdeg; _min_deg_limit = mindeg;}
    
    /** ready()
     *
     * Obtiene el estado del driver, indicando si está listo para una nueva operación o si por lo
     * contrario, está ocupado, realizando una pendiente.
     * @return Estado actual [true|false] [listo|ocupado]
     */
	bool ready(){return ((_steps > 0)? false : true);}

    /** request()
     *
     * Solicita un nuevo movimiento basado en un número de grados y un sentido concreto
     * @param degrees Número de grados a girar
     * @param clockwise Flag para indicar el sentido de giro
     * @return Estado actual de las salidas a las que conectar el driver (x x x x IN1 IN2 IN3 IN4)
     */
	uint8_t request(uint16_t degrees, bool clockwise);

    /** next()
     *
     * Obtiene la siguiente configuración de las salidas en la operación en curso. En caso de que
     * la operación haya concluído, se obtendrá siempre el estado final.
     * @return Estado actual de las salidas a las que conectar el driver (x x x x IN1 IN2 IN3 IN4)
     */
	uint8_t next();

    /** getSteps()
     *
     * Obtiene el número de pasos que quedan para finalizar la orden actual
     * @return Número de pasos pendientes
     */
	uint16_t getSteps(){return _steps;}
    
    /** getDegrees()
     *
     * Obtiene el ángulo de giro actual
     * @return Número de pasos pendientes
     */
	float getDegrees(){return _degrees;}
    
    /** isOOL()
     *
     * Chequea si está fuera de rango
     * @return True si está fuera de rango.
     */
	bool isOOL();
    
protected:
    static const int16_t MAX_DEG_DEFAULT = 50;
	static const int16_t MIN_DEG_DEFAULT = 0;
	uint8_t _id;
	bool _clockwise;
	uint16_t _steps;
    float _degrees;
    int16_t _max_deg_limit;
    int16_t _min_deg_limit;
	Stepper_mode_t _mode;
	uint8_t _step;
	const uint8_t * _sequence;
    Logger* _logger;
private:
};

#endif
