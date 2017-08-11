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
    
    @file          IK9.h
    @purpose       Especialización IKModel con 9 grados de libertad 3sections x 3segments
    @version       v1.0
    @date          Jul 2017
    @author        @raulMrello
*/

#ifndef __IK9_H
#define __IK9_H

#include "mbed.h"
#include "IKModel.h"

/**
 * @author raulMrello
 * @see API 
 *
 * <b>IKModel</b> Clase que proporciona una serie de modelos IK para la generación de movimiento
 * del TrunkRobot.
 */
class IK9 : public IKModel {
public:

    /** IKModel()
     *
     * Constructor.
     * Configura la máquina de estados del modelo IK por defecto
     * @param logger Objeto logger
     */
	IK9(Logger *logger = 0) : IKModel(logger){}

    /** goStand()
     *
     * Deshace cualquier inclinación para volver al punto inicial
     * @return array de acción con los grados a girar cada motor
     */
	virtual int16_t* goStand();

    /** goLeft()
     *
     * Inclina hacia la izquierda
     * @param level Cantidad (potencia) del movimiento a realizar
     * @return array de acción con los grados a girar cada motor
     */
	virtual int16_t* goLeft(IKLevel_enum level);

    /** goRight()
     *
     * Inclina hacia la derecha
     * @param level Cantidad (potencia) del movimiento a realizar
     * @return array de acción con los grados a girar cada motor
     */
	virtual int16_t* goRight(IKLevel_enum level);

    /** goUp()
     *
     * Inclina menos en la última dirección seleccionada
     * @param level Cantidad (potencia) del movimiento a realizar
     * @return array de acción con los grados a girar cada motor
     */
	virtual int16_t* goUp(IKLevel_enum level);

    /** goDown()
     *
     * Inclina más hacia la última dirección seleccionada
     * @param level Cantidad (potencia) del movimiento a realizar
     * @return array de acción con los grados a girar cada motor
     */
     virtual int16_t* goDown(IKLevel_enum level);

    /** headUp()
     *
     * Inclina menos (la cabeza) 
     * @param level Cantidad (potencia) del movimiento a realizar
     * @return array de acción con los grados a girar cada motor
     */
	virtual int16_t* headUp(IKLevel_enum level);

    /** headDown()
     *
     * Inclina más (la cabeza)
     * @param level Cantidad (potencia) del movimiento a realizar
     * @return array de acción con los grados a girar cada motor
     */
     virtual int16_t* headDown(IKLevel_enum level);    

	/** setPosition
     *
	 *	Fuerza a colocar el efector en una posición concreta según la orientación e inclinación. Como máximo
	 *	podrá hacer movimientos dentro de un sector AB, BC, CA, por lo que para hacer un movimiento en varios
	 *	sectores, habrá que invocar a la función varias veces, hasta que el resultado sea True (posición alcanzada)
	 *	@param orientation Orientación 0..359º
	 *	@param inclination Inclinación 0.. MAX_RANGE
	 *	@param action Puntero a la acción a realizar
	 *	@return True si la posición se ha alcanzado.
     */
	virtual bool setPosition(int16_t orientation, int16_t inclination, int16_t **action);
         
protected:    
};

#endif
