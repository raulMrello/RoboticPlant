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
 *  \brief Habilita todo lo relacionado con su propio hilo de ejecución.
 *		   Si es 0 está desactivado y funciona en modo pasivo + ISR
 *		   Si es 1 está activado y se ejecuta en su propia tarea
 */
#define SERIALMON_ENABLE_THREAD		0


/** \def SERIALMON_ENABLE_MSGBOX
 *  \brief Habilita la comunicación pub-sub mediante topics
 */
#define SERIALMON_ENABLE_MSGBOX		0


/** \def SERIALMON_ENABLE_SIMBUF
 *  \brief Habilita un buffer RAM en el que se simulan operaciones tx/rx 
 *		   Válido para depuración. Comentar para desactivar
 */
//#define SERIALMON_ENABLE_SIMBUF


/** \def SERIALMON_MAX_COMMAND_LENGTH
 *  \brief Tamaño máximo por defecto de los comandos Rx
 */
#define SERIALMON_MAX_COMMAND_LENGTH    16u


/** \def SERIALMON_DEFAULT_RX_BUFFER_SIZE
 *  \brief Tamaño por defecto del buffer de recepción
 */
#ifndef SERIALMON_DEFAULT_RX_BUFFER_SIZE
#define SERIALMON_DEFAULT_RX_BUFFER_SIZE    32
#endif


/** \def SERIALMON_DEFAULT_TX_BUFFER_SIZE
 *  \brief Tamaño por defecto del buffer de transmisión
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
 *         de envío y recepción. Puede ejecutarse en su propio hilo, utilizar callbacks de
 *         notificación y/o mensajería pub-sub 
 */

class SerialMon {
public:
    
    /**
     * Constructor
     * @param tx PinName TX pin.
     * @param rx PinName RX pin.
     * @param txBufferSize Tamaño buffer envío
     * @param rxBufferSize Tamaño buffer recepción
     * @param baud Baudrate
     * @param name Nombre del objeto
     * @param rx_detect Caracter de fin de recepción
     */    
    SerialMon(PinName tx, PinName rx, int txBufferSize = SERIALMON_DEFAULT_TX_BUFFER_SIZE, int rxBufferSize = SERIALMON_DEFAULT_RX_BUFFER_SIZE, int baud=115200, const char* name = (const char*)"NO NAME", char rx_detect = 0);
 
    
    /** Destructor */    
    virtual ~SerialMon();
    
	/** 
	 * Cambia el flag de fin de recepción
     * @param rx_detect Flag a detectar como fin de recepción
     */    
    void setRxDetectChar(char rx_detect){
		_auto_detect_char = rx_detect;
	}
    
	/** 
	 * Listener que se invoca cuando se recibe una actualización de un topic
     * en el modelo de comunicación pub-sub.
     * @param topicname Descripción del topic actualizado
     * @param topicdata Datos asociados al topic
     */    
    void onNewTopic(const char * topicname, void * topicdata);
	
	/**
	 * Método de ejecución del hilo propio en caso de estar permitida la ejecución
     * como thread. En otro caso, no hace nada
     */
    void start(); 
	
    /**
	 * Método para registrar callback a invocar tras recibir el caracter de fin de
	 * recepción (flag EOR)
     * @param func callback a registrar para la recepción
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
	 * Idem que el anterior, pero registrando un método de un objeto
     * @param obj objeto
     * @param method método público
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
	 * Método para registrar callback a invocar tras terminar de enviar los datos
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
	 * Idem que el anterior, pero registrando un método de un objeto
     * @param obj objeto
     * @param method método público
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
     * Método para enviar un buffer de datos con un tamaño concreto
     * @param data buffer a enviar
	 * @param size tamaño del buffer a enviar
	 * @return Resultado: true si se pudo enviar, false si no se pudo enviar
     */
    bool send(uint8_t *data, int size);	
	
    /**
     * Método para enviar un string
     * @param text string a enviar
	 * @param size número de bytes a enviar
	 * @param ntc flag para indicar si hay que enviar el caracter de terminación '\0' o no.
	 * @return Resultado: true si se pudo enviar, false si no se pudo enviar
     */
    bool send(char *text, int size, bool ntc){
		return send((uint8_t*)text, ((ntc)? (size+1) : size));
	}		
	
    /**
     * Método para enviar un string completo
     * @param text string a enviar
	 * @return Resultado: true si se pudo enviar, false si no se pudo enviar
     */
    bool send(char *text){
		return send((uint8_t*)text, strlen(text));
	}		

    /**
     * Idem que el anterior, pero bloqueante hasta que no se detecta el flag EOT
     * @param data buffer a enviar
	 * @param size tamaño del buffer a enviar
	 * @return Resultado: true si se pudo enviar, false si no se pudo enviar
     */
    bool sendComplete(uint8_t *data, int size);
		
    /**
     * Idem que el anterior, pero bloqueante hasta que no se detecta el flag EOT
     * @param text string a enviar
	 * @param size tamaño del buffer a enviar
	 * @param ntc flag para indicar si hay que enviar el caracter de terminación '\0' o no.
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
     * Método para leer los datos recibidos del buffer de recepción, hasta un máximo y sólo
     * hasta el primer caracter EOR leído
	 * @param s buffer para recibir los datos
     * @param max Máximo tamaño del buffer a recibir
	 * @param eor Caracter a detectar, si es distinto del registrado por defecto
     * @return Número de bytes leídos
     */
    int move(char *s, int max, char eor);	
    int move(char *s, int max){
		return move(s, max, _auto_detect_char);
	}
	
    /**
     * Callback propia para manejar las interrupciones de recepción del puerto serie
     */
    void rxCallback();
    
    /**
     * Callback propia para manejar las interrupciones de transmisión del puerto serie
     */
    void txCallback(); 
    
    /**
     * Callback propia para manejar las interrupciones de timeout en recepción
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
     * Rutina para determinar si hay datos pendientes por leer en el buffer de recepción
     * @return true si hay datos, false en caso contrario
     */
    bool readable() { return (((_rxbuf.in != _rxbuf.ou) || (_rxbuf.in == _rxbuf.ou && (_stat & FLAG_RXFULL)!=0))? true : false); }
    
    /**
     * Extrae un byte del buffer de recepción
     * @return byte extraído
     */
    char remove();  
    
    /**
     * Estructura de datos del buffer utilizado en transmisión y recepción
     */
     typedef struct {
		#if SERIALMON_ENABLE_THREAD==1
        Mutex mtx;			///< Mutex de control de acceso, únicamente válido en entornos multithread
		#endif
        char* mem;			///< Puntero a la memoria reservada al buffer
        char* limit;		///< Ultima posición del buffer
        char* in;			///< Posición de escritura en el buffer
        char* ou;			///< Posición de lectura del buffer
        int sz;				///< Tamaño del buffer
    }buffer_t;
   
    /**
     * Flags de señalización de estados 
     */
    enum Flags {
        FLAG_EOT = 		1,  ///< Flag de fin de transmisión
		FLAG_EOR = 		2,	///< Flag de fin de recepción
		FLAG_RTIMED =   4,	///< Flag para indicar que ha ocurrido timeout en recepción
		FLAG_SENDING = 	8,	///< Flag para indicar que hay un proceso de envío en marcha
		FLAG_RXFULL =  16,	///< Flag para indicar que el buffer de recepción está lleno
		FLAG_STARTED = 32,	///< Flag para indicar que el objeto está iniciado
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
