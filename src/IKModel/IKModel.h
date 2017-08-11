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
    
    @file          IKModel.h
    @purpose       Librer�a con diversos modelos IK para el TrunkRobot
    @version       v1.0
    @date          Jul 2017
    @author        @raulMrello
*/

#ifndef __IKMODEL_H
#define __IKMODEL_H

#include "mbed.h"
#include "TrunkController.h"
#include "Logger.h"

/**
 * @author raulMrello
 * @see API 
 *
 * <b>IKModel</b> Clase que proporciona una serie de modelos IK para la generaci�n de movimiento
 * del TrunkRobot.
 */
class IKModel {
public:

    /** Enumeraci�n para calibrar el movimiento a realizar */
    typedef enum{
        NONE,
        VERY_SMALL,
        SMALL,
        MEDIUM,
        HIGH,
        VERY_HIGH,
		ULTRA_HIGH,
		EXTREME,
		DEGx90,
    }IKLevel_enum;    

    /** IKModel()
     *
     * Constructor.
     * Configura la m�quina de estados del modelo IK por defecto
     * @param logger Objeto logger
     */
	IKModel(Logger *logger = 0);

    /** goStand()
     *
     * Deshace cualquier inclinaci�n para volver al punto inicial
     * @return array de acci�n con los grados a girar cada motor
     */
	virtual int16_t* goStand() = 0;

    /** goLeft()
     *
     * Inclina hacia la izquierda
     * @param level Cantidad (potencia) del movimiento a realizar
     * @return array de acci�n con los grados a girar cada motor
     */
	virtual int16_t* goLeft(IKLevel_enum level) = 0;

    /** goRight()
     *
     * Inclina hacia la derecha
     * @param level Cantidad (potencia) del movimiento a realizar
     * @return array de acci�n con los grados a girar cada motor
     */
	virtual int16_t* goRight(IKLevel_enum level) = 0;

    /** goUp()
     *
     * Inclina menos en la �ltima direcci�n seleccionada
     * @param level Cantidad (potencia) del movimiento a realizar
     * @return array de acci�n con los grados a girar cada motor
     */
	virtual int16_t* goUp(IKLevel_enum level) = 0;

    /** goDown()
     *
     * Inclina m�s hacia la �ltima direcci�n seleccionada
     * @param level Cantidad (potencia) del movimiento a realizar
     * @return array de acci�n con los grados a girar cada motor
     */
     virtual int16_t* goDown(IKLevel_enum level) = 0;

    /** headUp()
     *
     * Inclina menos (la cabeza) 
     * @param level Cantidad (potencia) del movimiento a realizar
     * @return array de acci�n con los grados a girar cada motor
     */
	virtual int16_t* headUp(IKLevel_enum level) = 0;

    /** headDown()
     *
     * Inclina m�s (la cabeza)
     * @param level Cantidad (potencia) del movimiento a realizar
     * @return array de acci�n con los grados a girar cada motor
     */
     virtual int16_t* headDown(IKLevel_enum level) = 0;    

	/** setPosition
     *
	 *	Fuerza a colocar el efector en una posici�n concreta seg�n la orientaci�n e inclinaci�n. Como m�ximo
	 *	podr� hacer movimientos dentro de un sector AB, BC, CA, por lo que para hacer un movimiento en varios
	 *	sectores, habr� que invocar a la funci�n varias veces, hasta que el resultado sea True (posici�n alcanzada)
	 *	@param orientation Orientaci�n 0..359�
	 *	@param inclination Inclinaci�n 0.. MAX_RANGE
	 *	@param action Puntero a la acci�n a realizar
	 *	@return True si la posici�n se ha alcanzado.
     */
	virtual bool setPosition(int16_t orientation, int16_t inclination, int16_t **action) = 0;
	
    /** update()
     *
     * Actualiza la posici�n. Esta rutina se invoca para notificar que
     * la acci�n correspondiente a la posici�n _next_pos se ha llevado a 
     * cabo y por lo tanto hay que actualizar _curr_pos.
     */
	void update();
	
    /** restart()
     *
     * Reinicia los valores a la posici�n por defecto
     */
	void restart();
	
    /** getOrientation()
     *
     * Obtiene la orientaci�n actual
	 *	@return Orientaci�n actual
     */
	int16_t getOrientation(){return _curr_orientation;}
	
    /** getInclination()
     *
     * Obtiene la inclinaci�n de la secci�n indicada
	 *	@param sect Secci�n
	 *	@return incl Inclinaci�n de la secci�n
     */
	int16_t getInclination(uint8_t sect){return _inclination[sect];}  

	
protected:
    /** Resoluci�n del paso de rotaci�n */
    static const int16_t ORIENTATION_STEP = 5;
    /** Resoluci�n del paso de inclinaci�n */
    static const int16_t INCLINATION_STEP = 5;

    /** Estructura de datos de una acci�n tipo */
    typedef struct{
        int16_t degrees[TrunkController::MOTOR_COUNT];
    }IKAction_t;
    
    /** Lista de comandos hacia los que puede dirigirse */
    typedef enum{
        STAND,
        AHEAD,
        LEFT,
        RIGHT,
        BACK
    }IKDirection_enum;
    
    /** Lista de comandos para generar movimientos */
    typedef enum{
        STOP,
        MORE,
        LESS
    }IKCommand_enum;
    
    
    int16_t     _curr_orientation;
	int16_t     _next_orientation;
    int16_t     _curr_inclination;
    int16_t     _next_inclination;
	int16_t     _inclination[TrunkController::SECTION_COUNT];
	IKAction_t  _action;
    IKAction_t  _curr_pos;
    IKAction_t  _next_pos;
    Logger*     _logger;

    int16_t* computeAction();
	void oi2section(int16_t odeg, int16_t ideg, int16_t* section);
};

#endif
