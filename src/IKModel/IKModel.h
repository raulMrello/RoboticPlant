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
     * @return array de acci�n con los grados a girar cada motor
     */
	int16_t* goLeft();

    /** goRight()
     *
     * Inclina hacia la derecha
     * @return array de acci�n con los grados a girar cada motor
     */
	int16_t* goRight();

    /** goUp()
     *
     * Inclina menos en la �ltima direcci�n seleccionada
     * @return array de acci�n con los grados a girar cada motor
     */
	int16_t* goUp();

    /** goDown()
     *
     * Inclina m�s hacia la �ltima direcci�n seleccionada
     * @return array de acci�n con los grados a girar cada motor
     */
	int16_t* goDown();


protected:
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
    
	Logger* _logger;
    IKAction_t _action;
    IKAction_t _curr_pos;
    IKDirection_enum _direction;
    IKDirection_enum _last_direction;
    IKCommand_enum _command;
    IKCommand_enum _last_command;
private:
};

#endif
