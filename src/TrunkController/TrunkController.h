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
    
    @file          TrunkController.h
    @purpose       Hyper-redundant segmented trunk robot controller
    @version       v1.0
    @date          Jun 2017
    @author        @raulMrello
*/

#ifndef __TRUNKCONTROLLER_H
#define __TRUNKCONTROLLER_H

#include "mbed.h"
#include "Shifter.h"
#include "Stepper.h"

/**
 * @author raulMrello
 * @see API 
 *
 * <b>TrunkController</b> Clase que inicia órdenes de actuación sobre 9 motores paso a paso conectacdos en un robot
 * segmentado de 3 secciones, con 3 tendones por sección. Cada motor se controla de forma independiente a través de
 * un Shifter de 36 salidas (9 motores x 4 gpios por motor = 36 gpios).
 */
class TrunkController : Shifter {
public:
    
    /** TrunkController()
     *
     * Constructor.
     * Configura el modo de funcionamiento del robot, asignando los gpio utilizados por el
     * shifter, el modo de funcionamiento de los steppers y la frecuencia de refresco de los
     * motores (max. 100Hz).
     * @param gpio_oe	Salida para controla el registro de salida a los pines físicos /OE
     * @param gpio_srclr Salida para controlar el reset del registro de desplazamiento /SRCLR
     * @param gpio_rclk Salida de reloj del registro de desplazamiento RCLK _/\_
     * @param gpio_srclk Salida de reloj del registro de acumulaci?n SRCLK _/\_
     * @param gpio_ser Salida para inyectar datos serie
     * @param mode	Modo de funcionamiento
     * @param freq	Velocidad de refresco (max 100Hz)
     */
	TrunkController(PinName gpio_oe, PinName gpio_srclr, PinName gpio_rclk,
					PinName gpio_srclk, PinName gpio_ser,
					Stepper::Stepper_mode_t mode, uint8_t freq = 100, RawSerial *serial = 0);

    /** ~TrunkController()
     *
     * Destructor.
     * Libera los recursos utilizados
     */
	~TrunkController();

    /** ready()
     *
     * Consulta si hay alguna operación en curso para saber si está listo o no para otra.
     * @return Estado actual [true|false] [listo|ocupado]
     */
	bool ready();

    /** notifyReady()
     *
     * Permite instalar una callback, que será invocada al finalizar una acción en curso.
     * @param cb_ready Callback a instalar. Puede ser un método o estático o un método de clase.
     */
	void notifyReady(Callback<void()> cb_ready);

    /** notifyStep()
     *
     * Permite instalar una callback, que será invocada al finalizar cada uno de los pasos de una acción.
     * @param cb_step Callback a instalar. Puede ser un método o estático o un método de clase.
     */
	void notifyStep(Callback<void()> cb_step);

    /** exec()
     *
     * Ejecuta una acción concreta en el robot. Lógicamente los motores que no se deseen
     * modificar, basta con pasarles un número de grados nulo (degrees = 0). Cuando la
     * acción finalice, se invocará la callback registrada mediante "notifyReady", para
     * indicar que está listo para una nueva acción.
     * @param degrees Array con el número de grados a girar, para cada motor
     * @param clockwise Array con el flag para indicar el sentido de giro, de cada motor
     */
	void exec(uint16_t* degrees, bool* clockwise);

    /** nextAction()
     *
     * Ejecuta una nueva acción que se traduce en un env?o al shifter. En caso de que la
     * operación anterior generara un "ready", se invocaría a "_cb_ready" y el proceso de
     * generación de acciones se detendría.
     */
	void nextAction();

    /** setLogger()
     *
     * Registra un objeto RawSerial para poder imprimir mensajes de logging
     * @param serial Objeto RawSerial
     */
	void setLogger(RawSerial* serial){_serial = serial;}


protected:
    /** buildAction()
     *
     * Forma la siguiente acción a enviar al shifter, a partir de las lecturas obtenidas
     * de los Steppers. Opcionalmente se puede invocar a "next" de cada stepper.
     * @param do_next Flag para invocar o no previamente a "next".
     */
	void buildAction(bool do_next = false);

	static const uint8_t SECTION_COUNT = 3;
	static const uint8_t SEGMENTS_PER_SECTION = 3;
	static const uint8_t MOTOR_COUNT = SECTION_COUNT * SEGMENTS_PER_SECTION;
	static const uint8_t GPIO_PER_MOTOR = 4;
	static const uint8_t ACTION_BITS_COUNT = 8;
	static const uint8_t SHIFTER_OUTPUTS = MOTOR_COUNT * GPIO_PER_MOTOR;
	static const uint8_t ACTION_ARRAY_LENGTH = (SHIFTER_OUTPUTS/ACTION_BITS_COUNT)+1;

	Stepper* _steppers[SECTION_COUNT][SEGMENTS_PER_SECTION];
	uint8_t _actions[SECTION_COUNT][SEGMENTS_PER_SECTION];
	uint8_t _next_action[ACTION_ARRAY_LENGTH];
	uint16_t _max_steps;
	float _wait_sec;
	Callback<void()> _cb_ready;
    Callback<void()> _cb_step;
	Ticker _tmr;
	Callback<void()> _cb_tmr;
	Thread* _th;
    RawSerial* _serial;
};

#endif
