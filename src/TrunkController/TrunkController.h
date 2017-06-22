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
#include "Logger.h"
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
        
    /** Parámetros de configuración públicos */
    static const uint8_t SECTION_COUNT = 3;
	static const uint8_t SEGMENTS_PER_SECTION = 3;
	static const uint8_t MOTOR_COUNT = SECTION_COUNT * SEGMENTS_PER_SECTION;


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
     * @param logger Objeto logger
     */
	TrunkController(PinName gpio_oe, PinName gpio_srclr, PinName gpio_rclk,
					PinName gpio_srclk, PinName gpio_ser,
					Stepper::Stepper_mode_t mode, uint8_t freq = 100, Logger *logger = 0);

    /** ~TrunkController()
     *
     * Destructor.
     * Libera los recursos utilizados
     */
	~TrunkController();

    /** task()
     *
     * Función asociada al hilo de ejecución.
     * @return Estado actual [true|false] [listo|ocupado]
     */
	void task();

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

    /** setLogger()
     *
     * Registra un objeto RawSerial para poder imprimir mensajes de logging
     * @param serial Objeto RawSerial
     */
	void setLogger(Logger* logger){_logger = logger;}

	/** actionRequested
     *
	 *	Permite solicitar la ejecución de una nueva acción. Esta solicitud se insertará en la cola de
     *  ejecución
     *  @param degrees Array con los grados a mover cada motor. Valores positivos giran en el sentido de
     *         de las agujas del reloj. Valores negativos, giran en sentido contrario.
     *  @return True si la acción se ha podido añadir a la cola de procesos o False si no se puede añadir
     */
	bool actionRequested(int16_t* degrees);

	/** stop
     *
	 *	Permite detener la ejecución de acciones pendientes. Si hay una acción en curso, se finalizará y las
     *  siguientes serán descartadas. Será un servicio bloqueante que no devolverá el control hasta que el
     *  proceso no haya finalizado completamente. Las acciones en la cola de espera se eliminarán.
     */
	void stop(){ _th->signal_set(TYPE_SIGNAL_ACTION_CANCELLED); }

	/** getActionCount
     *
	 *	Permite obtener el número de acciones ejecutadas.
     */
	uint32_t getActionCount(){ return _action_count;}

	/** clearActionCount
     *
	 *	Permite borrar (reiniciar) el número de acciones ejecutadas.
     */
	void clearActionCount(){ _action_count=0;}
    
protected:
	static const uint8_t GPIO_PER_MOTOR = 4;
	static const uint8_t ACTION_BITS_COUNT = 8;
	static const uint8_t SHIFTER_OUTPUTS = MOTOR_COUNT * GPIO_PER_MOTOR;
	static const uint8_t ACTION_ARRAY_LENGTH = (SHIFTER_OUTPUTS/ACTION_BITS_COUNT)+1;

	/** Recursos asociados a los mensajes insertables en la cola asociada al hilo de ejecución */
	static const uint32_t TYPE_SIGNAL_ACTION_REQUESTED = (1<<0);
	static const uint32_t TYPE_SIGNAL_ACTION_COMPLETED = (1<<1);
    static const uint32_t TYPE_SIGNAL_ACTION_CANCELLED = (1<<2);
	typedef struct {
		int16_t degrees[SECTION_COUNT][SEGMENTS_PER_SECTION];
	}Action_t;
	static const uint8_t MAX_QUEUE_SIZE = 8;	
	Mail< Action_t, MAX_QUEUE_SIZE > _mail;

    /** Recursos asociados a la activación sincronizada de los motores paso a paso */
	Stepper* _steppers[SECTION_COUNT][SEGMENTS_PER_SECTION];
	uint8_t _actions[SECTION_COUNT][SEGMENTS_PER_SECTION];
    uint8_t _next_action[ACTION_ARRAY_LENGTH];
	uint16_t _max_steps;
    uint32_t _action_count;
	float _wait_sec;
	Callback<void()> _cb_ready;
    Callback<void()> _cb_step;
	Callback<void()> _cb_tmr;
	Ticker _tmr;
    
	Thread* _th;
    Logger* _logger;
    
    
    /** Métodos protegidos de este componente */
    
    
    /** ready()
     *
     * Consulta si hay alguna operación en curso para saber si está listo o no para otra.
     * @return Estado actual [true|false] [listo|ocupado]
     */
	bool ready();

    /** buildAction()
     *
     * Forma la siguiente acción a enviar al shifter, a partir de las lecturas obtenidas
     * de los Steppers. Opcionalmente se puede invocar a "next" de cada stepper.
     * @param do_next Flag para invocar o no previamente a "next".
     */
	void buildAction(bool do_next = false);
    
    /** nextAction()
     *
     * Ejecuta una nueva acción que se traduce en un env?o al shifter. En caso de que la
     * operación anterior generara un "ready", se invocaría a "_cb_ready" y el proceso de
     * generación de acciones se detendría.
     */
	void nextAction();

    /** exec()
     *
     * Ejecuta una acción concreta en el robot solicitada mediante un 'actionRequest'
     * @param degrees Array con el número de grados a girar, para cada motor. Valores 
     *        positivos giran en el sentido de las agujas del reloj. Valores negativos
     *        giran en sentido contrario.
     */
	void exec(int16_t* degrees);
};

#endif
