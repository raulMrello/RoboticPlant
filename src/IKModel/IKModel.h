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
        VERY_HIGH
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
	int16_t* goStand();

    /** goLeft()
     *
     * Inclina hacia la izquierda
     * @param level Cantidad (potencia) del movimiento a realizar
     * @return array de acci�n con los grados a girar cada motor
     */
	int16_t* goLeft(IKLevel_enum level);

    /** goRight()
     *
     * Inclina hacia la derecha
     * @param level Cantidad (potencia) del movimiento a realizar
     * @return array de acci�n con los grados a girar cada motor
     */
	int16_t* goRight(IKLevel_enum level);

    /** goUp()
     *
     * Inclina menos en la �ltima direcci�n seleccionada
     * @param level Cantidad (potencia) del movimiento a realizar
     * @return array de acci�n con los grados a girar cada motor
     */
	int16_t* goUp(IKLevel_enum level);

    /** goDown()
     *
     * Inclina m�s hacia la �ltima direcci�n seleccionada
     * @param level Cantidad (potencia) del movimiento a realizar
     * @return array de acci�n con los grados a girar cada motor
     */
     int16_t* goDown(IKLevel_enum level);

    /** headUp()
     *
     * Inclina menos (la cabeza) 
     * @param level Cantidad (potencia) del movimiento a realizar
     * @return array de acci�n con los grados a girar cada motor
     */
	int16_t* headUp(IKLevel_enum level);

    /** headDown()
     *
     * Inclina m�s (la cabeza)
     * @param level Cantidad (potencia) del movimiento a realizar
     * @return array de acci�n con los grados a girar cada motor
     */
     int16_t* headDown(IKLevel_enum level);    
     
    /** update()
     *
     * Actualiza la posici�n. Esta rutina se invoca para notificar que
     * la acci�n correspondiente a la posici�n _next_pos se ha llevado a 
     * cabo y por lo tanto hay que actualizar _curr_pos.
     */
	void update();

    
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
    
    
    int16_t     _orientation;
    int16_t     _inclination[TrunkController::SECTION_COUNT];
	IKAction_t  _action;
    IKAction_t  _curr_pos;
    IKAction_t  _next_pos;
    Logger*     _logger;
    
private:
    int16_t* computeAction();
};

#endif
