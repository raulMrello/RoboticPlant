/*
    Copyright (c) 2016 raulMrello
 
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
    
    @file          SerialMon.h 
    @purpose       Monitor Serie tx/rx con buffer
    @version       see ChangeLog.c
    @date          Nov 2016
    @author        raulMrello
*/

#ifndef SERIALMON_H
#define SERIALMON_H


/** \def SERIALMON_ENABLE_THREAD
 *  \brief Habilita todo lo relacionado con su propio hilo de ejecuci�n.
 *		   Si es 0 est� desactivado y funciona en modo pasivo + ISR
 *		   Si es 1 est� activado y se ejecuta en su propia tarea
 */
#define SERIALMON_ENABLE_THREAD		0


/** \def SERIALMON_ENABLE_MSGBOX
 *  \brief Habilita la comunicaci�n pub-sub mediante topics
 */
#define SERIALMON_ENABLE_MSGBOX		0


/** \def SERIALMON_ENABLE_SIMBUF
 *  \brief Habilita un buffer RAM en el que se simulan operaciones tx/rx 
 *		   V�lido para depuraci�n. Comentar para desactivar
 */
//#define SERIALMON_ENABLE_SIMBUF


/** \def SERIALMON_MAX_COMMAND_LENGTH
 *  \brief Tama�o m�ximo por defecto de los comandos Rx
 */
#define SERIALMON_MAX_COMMAND_LENGTH    16u


/** \def SERIALMON_DEFAULT_RX_BUFFER_SIZE
 *  \brief Tama�o por defecto del buffer de recepci�n
 */
#ifndef SERIALMON_DEFAULT_RX_BUFFER_SIZE
#define SERIALMON_DEFAULT_RX_BUFFER_SIZE    32
#endif


/** \def SERIALMON_DEFAULT_TX_BUFFER_SIZE
 *  \brief Tama�o por defecto del buffer de transmisi�n
 */
#ifndef SERIALMON_DEFAULT_TX_BUFFER_SIZE
#define SERIALMON_DEFAULT_TX_BUFFER_SIZE    256
#endif


/** Archivos de cabecera */
#include "mbed.h"
#if SERIALMON_ENABLE_MSGBOX==1
#include "MsgBroker.h"
#endif


/** \class SerialMon
 *  \brief Este componente permite gestionar comunicaciones serie, proporcionando buffers
 *         de env�o y recepci�n. Puede ejecutarse en su propio hilo, utilizar callbacks de
 *         notificaci�n y/o mensajer�a pub-sub 
 */

class SerialMon {
public:
    
    /**
     * Constructor
     * @param tx PinName TX pin.
     * @param rx PinName RX pin.
     * @param txBufferSize Tama�o buffer env�o
     * @param rxBufferSize Tama�o buffer recepci�n
     * @param baud Baudrate
     * @param name Nombre del objeto
     * @param rx_detect Caracter de fin de recepci�n
     */    
    SerialMon(PinName tx, PinName rx, int txBufferSize = SERIALMON_DEFAULT_TX_BUFFER_SIZE, int rxBufferSize = SERIALMON_DEFAULT_RX_BUFFER_SIZE, int baud=115200, const char* name = (const char*)"NO NAME", char rx_detect = 0);
 
    
    /** Destructor */    
    virtual ~SerialMon();
    
	/** 
	 * Cambia el flag de fin de recepci�n
     * @param rx_detect Flag a detectar como fin de recepci�n
     */    
    void setRxDetectChar(char rx_detect){
		_auto_detect_char = rx_detect;
	}
    
	/** 
	 * Listener que se invoca cuando se recibe una actualizaci�n de un topic
     * en el modelo de comunicaci�n pub-sub.
     * @param topicname Descripci�n del topic actualizado
     * @param topicdata Datos asociados al topic
     */    
    void onNewTopic(const char * topicname, void * topicdata);
	
	/**
	 * M�todo de ejecuci�n del hilo propio en caso de estar permitida la ejecuci�n
     * como thread. En otro caso, no hace nada
     */
    void start(); 
	
    /**
	 * M�todo para registrar callback a invocar tras recibir el caracter de fin de
	 * recepci�n (flag EOR)
     * @param func callback a registrar para la recepci�n
	 * @param timedfunc callback a registrar para el timeout
	 * @param us timeout para recibir una trama completa. Dejar a 0 para desactivar el timeout
     */
    void attachRxCallback(void (*func)(), void (*timedfunc)()=0, uint32_t us=0){
		#if MBED_LIBRARY_VERSION >= 130
		_fp_rx.attach(callback(func));
		_fp_rx_timeout.attach(callback(timedfunc));
		if(us==0){
			_rx_tick.detach();
		}
		else{
			//_rx_tick.attach_us(this, &SerialMon::timeoutCallback, us);
			_rx_tick.attach(callback(this, &SerialMon::timeoutCallback), us*1000000);
		}
		#else
		_fp_rx.attach(func);
		_fp_rx_timeout.attach(timedfunc);
		if(us==0){
			_rx_tick.detach();
		}
		else{
			_rx_tick.attach_us(this, &SerialMon::timeoutCallback, us);			
		}
		#endif
		
	}

    /**
	 * Idem que el anterior, pero registrando un m�todo de un objeto
     * @param obj objeto
     * @param method m�todo p�blico
	 * @param us timeout para recibir una trama completa. Dejar a 0 para desactivar el timeout
     */
    template<typename T, typename M>
    void attachRxCallback(T *obj, M method, M timedmethod=0, uint32_t us=0) {
		#if MBED_LIBRARY_VERSION >= 130
		_fp_rx.attach(callback(obj,method));
		_fp_rx_timeout.attach(callback(obj,timedmethod));
		if(us==0){
			_rx_tick.detach();
		}
		else{
			_rx_tick.attach(callback(this, &SerialMon::timeoutCallback), us*1000000);
		}
   		#else
		_fp_rx.attach(obj,method);
		_fp_rx_timeout.attach(obj,timedmethod);
		if(us==0){
			_rx_tick.detach();
		}
		else{
			_rx_tick.attach_us(this, &SerialMon::timeoutCallback, us);
		}
		#endif
		
    }
	
    /**
	 * M�todo para registrar callback a invocar tras terminar de enviar los datos
	 * pendientes (flag EOT)
     * @param func callback a registrar
     */
    void attachTxCallback(void (*func)()){
		#if MBED_LIBRARY_VERSION >= 130
		_fp_tx.attach(callback(func));
		#else
		_fp_tx.attach(func);
		#endif
	}

    /**
	 * Idem que el anterior, pero registrando un m�todo de un objeto
     * @param obj objeto
     * @param method m�todo p�blico
     */
    template<typename T, typename M>
    void attachTxCallback(T *obj, M method) {
		#if MBED_LIBRARY_VERSION >= 130
		_fp_tx.attach(callback(obj,method));
		#else
		_fp_tx.attach(obj,method);
		#endif
    }    
	
    /** 
     * M�todo para enviar un buffer de datos con un tama�o concreto
     * @param data buffer a enviar
	 * @param size tama�o del buffer a enviar
	 * @return Resultado: true si se pudo enviar, false si no se pudo enviar
     */
    bool send(uint8_t *data, int size);	
	
    /**
     * M�todo para enviar un string
     * @param text string a enviar
	 * @param size n�mero de bytes a enviar
	 * @param ntc flag para indicar si hay que enviar el caracter de terminaci�n '\0' o no.
	 * @return Resultado: true si se pudo enviar, false si no se pudo enviar
     */
    bool send(char *text, int size, bool ntc){
		return send((uint8_t*)text, ((ntc)? (size+1) : size));
	}		
	
    /**
     * M�todo para enviar un string completo
     * @param text string a enviar
	 * @return Resultado: true si se pudo enviar, false si no se pudo enviar
     */
    bool send(char *text){
		return send((uint8_t*)text, strlen(text));
	}		

    /**
     * Idem que el anterior, pero bloqueante hasta que no se detecta el flag EOT
     * @param data buffer a enviar
	 * @param size tama�o del buffer a enviar
	 * @return Resultado: true si se pudo enviar, false si no se pudo enviar
     */
    bool sendComplete(uint8_t *data, int size);
		
    /**
     * Idem que el anterior, pero bloqueante hasta que no se detecta el flag EOT
     * @param text string a enviar
	 * @param size tama�o del buffer a enviar
	 * @param ntc flag para indicar si hay que enviar el caracter de terminaci�n '\0' o no.
	 * @return Resultado: true si se pudo enviar, false si no se pudo enviar
     */
    bool sendComplete(char* text, int size, bool ntc){
		return sendComplete((uint8_t*)text, ((ntc)? (size+1) : size));
	}		
		
    /**
     * Idem que el anterior, pero bloqueante hasta que no se detecta el flag EOT
     * @param text string a enviar
	 * @return Resultado: true si se pudo enviar, false si no se pudo enviar
     */
    bool sendComplete(char* text){
		return sendComplete((uint8_t*)text, strlen(text));
	}		
    
    /**
     * M�todo para leer los datos recibidos del buffer de recepci�n, hasta un m�ximo y s�lo
     * hasta el primer caracter EOR le�do
	 * @param s buffer para recibir los datos
     * @param max M�ximo tama�o del buffer a recibir
	 * @param eor Caracter a detectar, si es distinto del registrado por defecto
     * @return N�mero de bytes le�dos
     */
    int move(char *s, int max, char eor);	
    int move(char *s, int max){
		return move(s, max, _auto_detect_char);
	}
	
    /**
     * Callback propia para manejar las interrupciones de recepci�n del puerto serie
     */
    void rxCallback();
    
    /**
     * Callback propia para manejar las interrupciones de transmisi�n del puerto serie
     */
    void txCallback(); 
    
    /**
     * Callback propia para manejar las interrupciones de timeout en recepci�n
     */
    void timeoutCallback(); 
    
    int printf(const char *format, ...);
    
    /**
     * Estructura de datos por defecto para los topics aceptados por este componente en el
	 * mecanismo pub-sub
     */
    typedef struct {
        uint8_t * data;
		int size;
    }topic_t;
	
protected:
     
    /**
     * Rutina para determinar si hay datos pendientes por leer en el buffer de recepci�n
     * @return true si hay datos, false en caso contrario
     */
    bool readable() { return (((_rxbuf.in != _rxbuf.ou) || (_rxbuf.in == _rxbuf.ou && (_stat & FLAG_RXFULL)!=0))? true : false); }
    
    /**
     * Extrae un byte del buffer de recepci�n
     * @return byte extra�do
     */
    char remove();  
    
    /**
     * Estructura de datos del buffer utilizado en transmisi�n y recepci�n
     */
     typedef struct {
		#if SERIALMON_ENABLE_THREAD==1
        Mutex mtx;			///< Mutex de control de acceso, �nicamente v�lido en entornos multithread
		#endif
        char* mem;			///< Puntero a la memoria reservada al buffer
        char* limit;		///< Ultima posici�n del buffer
        char* in;			///< Posici�n de escritura en el buffer
        char* ou;			///< Posici�n de lectura del buffer
        int sz;				///< Tama�o del buffer
    }buffer_t;
   
    /**
     * Flags de se�alizaci�n de estados 
     */
    enum Flags {
        FLAG_EOT = 		1,  ///< Flag de fin de transmisi�n
		FLAG_EOR = 		2,	///< Flag de fin de recepci�n
		FLAG_RTIMED =   4,	///< Flag para indicar que ha ocurrido timeout en recepci�n
		FLAG_SENDING = 	8,	///< Flag para indicar que hay un proceso de env�o en marcha
		FLAG_RXFULL =  16,	///< Flag para indicar que el buffer de recepci�n est� lleno
		FLAG_STARTED = 32,	///< Flag para indicar que el objeto est� iniciado
    };

	int _stat;
    int _err_rx;
    char * _name;
    char _auto_detect_char;
    buffer_t _txbuf;
    buffer_t _rxbuf;
    RawSerial *_serial;
	#if MBED_LIBRARY_VERSION >= 130
	Callback<void()> _fp_rx;
	Callback<void()> _fp_rx_timeout;
	Callback<void()> _fp_tx;
	#else
	FunctionPointer _fp_rx;
	FunctionPointer _fp_rx_timeout;
	FunctionPointer _fp_tx;
	#endif
	Ticker _rx_tick;
	topic_t _cmd_topic;
	char _cmd[SERIALMON_MAX_COMMAND_LENGTH];
	#if SERIALMON_ENABLE_THREAD==1
    Thread _thread;
	#endif
    #if defined(SERIALMON_ENABLE_SIMBUF)
    char * _simbuf;
    int    _simbuf_n;
    #endif
};


#endif
