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
    @purpose       Extends Serial to provide fully buffered IO
    @version       see ChangeLog.c
    @date          Nov 2016
    @author        raulMrello
*/

#include "SerialMon.h"


//--------------------------------------------------------------------------------------------------------------
//-- ISR CALLBACKS ---------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------


/*************************************************************************************/
void SerialMon::txCallback(){
	// si hay datos pendientes, los envía
    if ((_serial->writeable()) && (_txbuf.in != _txbuf.ou)) {
        char c = *_txbuf.ou;
        _serial->putc(c);
        _txbuf.ou++;
        _txbuf.ou = (_txbuf.ou >= _txbuf.limit)? _txbuf.mem : _txbuf.ou;
        #if defined(SERIALMON_ENABLE_SIMBUF)
        _simbuf[_simbuf_n] = c;
        _simbuf_n = (_simbuf_n < _txbuf.sz)? (_simbuf_n+1) : 0;
        #endif
        return;
    }
	// si ha terminado, borra el flag de envío y activa el EOT
    _stat &= ~FLAG_SENDING; 
	_stat |= FLAG_EOT;
	// invoca la callback registrada
	_fp_tx.call();
	// se desconecta del dispositivo de transmisión
    _serial->attach(0, (SerialBase::IrqType)TxIrq);
}


/*************************************************************************************/
void SerialMon::rxCallback(){
	// mientras haya datos pendientes, los almacena en el buffer
    while(_serial->readable()){
        char c = _serial->getc();
		// si el buffer está lleno, notifica error
        if((_stat & FLAG_RXFULL)!=0){
            _err_rx++;
            return;
        }
        _err_rx = 0;
        *_rxbuf.in++ = c;
        _rxbuf.in = (_rxbuf.in >= _rxbuf.limit)? _rxbuf.mem : _rxbuf.in;
		// si se completa el buffer, marca estado
        if(_rxbuf.in == _rxbuf.ou){
            _stat |= FLAG_RXFULL;
        }
		// al detectar el caracter de fín de recepción, notifica en callback
        if(c == _auto_detect_char){
			_stat |= FLAG_EOR;
			_fp_rx.call();
			// en caso de trabajar en modo thread, notifica el flag a su hilo
			#if SERIALMON_ENABLE_THREAD==1
            _thread.signal_set(FLAG_EOR);
			#endif
        }
    }
}


//--------------------------------------------------------------------------------------------------------------
//-- TOPIC UPDATE CALLBACKS ------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

/*************************************************************************************/
void SerialMon::onNewTopic(const char * topicname, void * topicdata){
	#if SERIALMON_ENABLE_MSGBOX==1
    // si se recibe un update del topic /keyb...
    if(strcmp(topicname, "/log") == 0){
        // obtiene un puntero a los datos del topic con el formato correspondiente
        SerialMon::topic_t *topic = (SerialMon::topic_t *)topicdata;
        // chequea el tipo de topic y activa los eventos habilitados en este módulo
        if(topic){
            send(topic->data, topic->size);
            MsgBroker::consumed("/log");
        }
    }
	#endif
}


//--------------------------------------------------------------------------------------------------------------
//-- PUBLIC METHODS --------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------


/*************************************************************************************/
SerialMon::SerialMon( PinName tx, PinName rx, int txSize, int rxSize, int baud, const char* name, char rx_detect ){  
	// inicia el dispositivo serie, en función de la versión de mbed
	#if MBED_LIBRARY_VERSION >= 130	
    _serial = new RawSerial(tx, rx, baud);
	#else
	_serial = new RawSerial(tx, rx); _serial->baud(baud);
	#endif
	
	// inicializa buffers
    _txbuf.mem = (char*)malloc(txSize);
    if(_txbuf.mem){
        _txbuf.limit = (char*)&_txbuf.mem[txSize];
        _txbuf.in = _txbuf.mem;
        _txbuf.ou = _txbuf.in;
        _txbuf.sz = txSize;
        memset(_txbuf.mem, 0, txSize);
    }
    _rxbuf.mem = (char*)malloc(rxSize);
    if(_rxbuf.mem){
        _rxbuf.limit = (char*)&_rxbuf.mem[rxSize];
        _rxbuf.in = _rxbuf.mem;
        _rxbuf.ou = _rxbuf.in;
        _rxbuf.sz = rxSize;
        memset(_rxbuf.mem, 0, rxSize);
    }
    #if defined(SERIALMON_ENABLE_SIMBUF)
    _simbuf = (char*)malloc(txSize);
    if(_simbuf){
        memset(_simbuf, '#', txSize);
    }
    _simbuf_n = 0;
    #endif
    if(name){
        char* _name = (char*)malloc(strlen(name)+1);
        if(_name){
			memset(_name, 0, strlen(name)+1);
            strcpy(_name, name);
        }
    }
	#if SERIALMON_ENABLE_MSGBOX==1
    // instala topics para envío (/log) y recepción (/cmd)
    MsgBroker::Exception e;
    MsgBroker::installTopic("/log", sizeof(SerialMon::topic_t));
    MsgBroker::installTopic("/cmd", sizeof(SerialMon::topic_t));
    // se subscribe a topics de envío (/log)
    MsgBroker::attach("/log", this, &SerialMon::onNewTopic, &e);
	#endif
	
    // desactiva las callbacks de nivel superior
	_fp_rx.attach(0);
	_fp_tx.attach(0);
	
    // se desconecta del dispositivo, registra caracter EOR e inicializa estado
    _serial->attach(0, (SerialBase::IrqType)TxIrq);
    _serial->attach(0, (SerialBase::IrqType)RxIrq);
    _auto_detect_char = rx_detect;
	_stat = 0;
    _err_rx = 0;

    // se conecta al módulo de recepción
	#if MBED_LIBRARY_VERSION >= 130
    _serial->attach(callback(this, &SerialMon::rxCallback), (SerialBase::IrqType)RxIrq);
	#else
	_serial->attach(this, &SerialMon::rxCallback, (SerialBase::IrqType)RxIrq);
	#endif
	
	#if SERIALMON_ENABLE_THREAD==1
    // inicia la ejecución del hilo de recepción, en caso de que esté permitido
    _thread.start(callback(this, &SerialMon::start));
    // borra los flags del thread asociado
    _thread.signal_clr(0xffff);
	#endif
}


/*************************************************************************************/
SerialMon::~SerialMon(){
    _serial->attach(0, (SerialBase::IrqType)TxIrq);
    _serial->attach(0, (SerialBase::IrqType)RxIrq);
	#if SERIALMON_ENABLE_THREAD==1
    _thread.join();
    _thread.terminate();
	#endif
    free(_name);
    free(_txbuf.mem);
    free(_rxbuf.mem);
}

/*************************************************************************************/
void SerialMon::start(){
	#if SERIALMON_ENABLE_THREAD==1
	#if SERIALMON_ENABLE_MSGBOX==1
    MsgBroker::Exception e;
	#endif
    
    for(;;){
        // Espera un flag EOR
        osEvent oe = _thread.signal_wait(FLAG_EOR, osWaitForever);

        // si es un flag de fin de recepción, lo procesa
        if(oe.status == osEventSignal && (oe.value.signals & FLAG_EOR) != 0){
			_thread.signal_clr(FLAG_EOR);
            // extrae del buffer y lo copia en _cmd
            int n = move(&_cmd[0], SERIALMON_MAX_COMMAND_LENGTH);
			#if SERIALMON_ENABLE_MSGBOX==1
            // publica el mensaje a los subscriptores
			_cmd_topic.data = (uint8_t*)_cmd;
			_cmd_topic.size = n;
            MsgBroker::publish("/cmd", &_cmd_topic, sizeof(topic_t), &e);
			#endif
        }
    }   
	#endif
}


/*************************************************************************************/
int SerialMon::move(char *s, int max, char end) {
    int counter = 0;
    char c;
	#if SERIALMON_ENABLE_THREAD==1
    _rxbuf.mtx.lock();
	#endif
    while(readable()) {
        c = *_rxbuf.ou++;
        _rxbuf.ou = (_rxbuf.ou >= _rxbuf.limit)? _rxbuf.mem : _rxbuf.ou;
        if(_rxbuf.in != _rxbuf.ou){
			_stat &= ~FLAG_RXFULL;
        }
        if (c == end) break;
        *(s++) = c;
        counter++;
        if (counter == max) break;
    }
	#if SERIALMON_ENABLE_THREAD==1
    _rxbuf.mtx.unlock();
	#endif
    return counter;
}

/*************************************************************************************/
bool SerialMon::send(uint8_t* data, int size){
	#if SERIALMON_ENABLE_THREAD==1
    _txbuf.mtx.lock();  
	#endif
    int remaining = (_txbuf.in >= _txbuf.ou)? ((_txbuf.limit-_txbuf.in)+(_txbuf.ou-_txbuf.mem)) : (_txbuf.ou-_txbuf.in);
    if(size > remaining){
		#if SERIALMON_ENABLE_THREAD==1
		_txbuf.mtx.unlock();
		#endif
        return false;
    }
    for(int i=0;i<size;i++){
        *_txbuf.in++ = data[i];
        _txbuf.in = (_txbuf.in >= _txbuf.limit)? _txbuf.mem : _txbuf.in;
    }
    if((_stat & FLAG_SENDING)==0){
        _stat |= FLAG_SENDING;
		_stat &= ~FLAG_EOT;
        if((_stat & FLAG_STARTED)!=0){
            _serial->attach(0, (SerialBase::IrqType)TxIrq);
            txCallback();
        }
        _stat |= FLAG_STARTED;
		#if MBED_LIBRARY_VERSION >= 130
        _serial->attach(callback(this, &SerialMon::txCallback), (SerialBase::IrqType)TxIrq);
		#else
		_serial->attach(this, &SerialMon::txCallback, (SerialBase::IrqType)TxIrq);		
		#endif
    }
	#if SERIALMON_ENABLE_THREAD==1
    _txbuf.mtx.unlock();
	#endif
    return true;
    
}

/*************************************************************************************/
bool SerialMon::sendComplete(uint8_t* data, int size){
	if(!send(data, size)){
		return false;
	}
	while((_stat & FLAG_EOT)==0);
	return true;
}

/*************************************************************************************/
char SerialMon::remove(){
    char c = *_rxbuf.ou++;
    _rxbuf.ou = (_rxbuf.ou >= _rxbuf.limit)? _rxbuf.mem : _rxbuf.ou;
    if(_rxbuf.in != _rxbuf.ou){
        _stat &= ~FLAG_RXFULL;
    }
    return c;   
}
