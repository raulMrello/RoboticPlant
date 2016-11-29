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
#include "MsgBroker.h"


/*************************************************************************************/
static void topicUpdateCallback(void *subscriber, const char * topicname){
	SerialMon *me = (SerialMon*)subscriber;
	// si se recibe un update del topic /keyb...
	if(strcmp(topicname, "/log") == 0){
		// obtiene un puntero a los datos del topic con el formato correspondiente
		SerialMon::topic_t *topic = (SerialMon::topic_t *)MsgBroker::getTopicData("/log");
		// chequea el tipo de topic y activa los eventos habilitados en este m�dulo
		if(topic){
			me->send(topic->txt);
			MsgBroker::consumed("/log");
		}
	}
}

/*************************************************************************************/
SerialMon::SerialMon( PinName tx, PinName rx, int txSize, int rxSize, int baud, const char* name ){    
	_serial = new RawSerial(tx, rx, baud);
	_txbuf.mem = (char*)malloc(txSize);
	if(_txbuf.mem){
		_txbuf.limit = (char*)&_txbuf.mem[txSize];
		_txbuf.in = _txbuf.mem;
		_txbuf.ou = _txbuf.in;
		_txbuf.sz = txSize;
		memset(_txbuf.mem, 0, txSize);
	}
	#if defined(SERIALMON_ENABLE_SIMBUF)
	_simbuf = (char*)malloc(txSize);
	if(_simbuf){
		memset(_simbuf, '#', txSize);
	}
	_simbuf_n = 0;
	#endif
	_rxbuf.mem = (char*)malloc(rxSize);
	if(_rxbuf.mem){
		_rxbuf.limit = (char*)&_rxbuf.mem[rxSize];
		_rxbuf.in = _rxbuf.mem;
		_rxbuf.ou = _rxbuf.in;
		_rxbuf.sz = rxSize;
		memset(_rxbuf.mem, 0, rxSize);
	}
	if(name){
		char* _name = (char*)malloc(strlen(name)+1);
		if(_name){
			strcpy(_name, name);
		}
	}
	MsgBroker::Exception e;
	// install a topic
	MsgBroker::installTopic("/log", sizeof(SerialMon::topic_t));
	// attaches to topic updates 
	MsgBroker::attach("/log", this, &topicUpdateCallback, &e);

	start('\n');
}

/*************************************************************************************/
SerialMon::~SerialMon(){
    _serial->attach(0, (SerialBase::IrqType)TxIrq);
	_serial->attach(0, (SerialBase::IrqType)RxIrq);
	free(_name);
	free(_txbuf.mem);
	free(_rxbuf.mem);
}

/*************************************************************************************/
void SerialMon::start(char c){
	_serial->attach(0, (SerialBase::IrqType)TxIrq);
	_serial->attach(0, (SerialBase::IrqType)RxIrq);
	_auto_detect_char = c;
	_f_sending = false;
	_f_rxfull = false;
	_err_rx = 0;
	_serial->attach(callback(this, &SerialMon::rxCallback), (SerialBase::IrqType)RxIrq);
	_f_started = true;
}

/*************************************************************************************/
bool SerialMon::busy() {
	if(_txbuf.in == _txbuf.ou && !_f_sending)
		return false;
	return true;
}

/*************************************************************************************/
int SerialMon::move(char *s, int max, char end) {
	int counter = 0;
	char c;
	_rxbuf.mtx.lock();
	while(readable()) {
		c = *_rxbuf.ou++;
		_rxbuf.ou = (_rxbuf.ou >= _rxbuf.limit)? _rxbuf.mem : _rxbuf.ou;
		if(_rxbuf.in != _rxbuf.ou){
			_f_rxfull = false;
		}
		if (c == end) break;
		*(s++) = c;
		counter++;
		if (counter == max) break;
	}
	_rxbuf.mtx.unlock();
	return counter;
}

/*************************************************************************************/
SerialMon::Result SerialMon::send(char* text){
	_txbuf.mtx.lock();	
	int remaining = (_txbuf.in >= _txbuf.ou)? ((_txbuf.limit-_txbuf.in)+(_txbuf.ou-_txbuf.mem)) : (_txbuf.ou-_txbuf.in);
	if(strlen(text) > remaining){
		return BufferOversize;
	}
	for(int i=0;i<strlen(text);i++){
		*_txbuf.in++ = text[i];
		_txbuf.in = (_txbuf.in >= _txbuf.limit)? _txbuf.mem : _txbuf.in;
	}
	if(!_f_sending){
		_f_sending = true;
		_serial->attach(0, (SerialBase::IrqType)TxIrq);
		txCallback();
		_serial->attach(callback(this, &SerialMon::txCallback), (SerialBase::IrqType)TxIrq);
	}
	_txbuf.mtx.unlock();
	return Ok;
	
}

/*************************************************************************************/

void SerialMon::txCallback(){
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
	_f_sending = false;
	_serial->attach(0, (SerialBase::IrqType)TxIrq);
}

/*************************************************************************************/
char SerialMon::remove(){
	char c = *_rxbuf.ou++;
	_rxbuf.ou = (_rxbuf.ou >= _rxbuf.limit)? _rxbuf.mem : _rxbuf.ou;
	if(_rxbuf.in != _rxbuf.ou){
		_f_rxfull = false;
	}
	return c;	
}

/*************************************************************************************/
void SerialMon::rxCallback(){
	char c = _serial->getc();
	if(_f_rxfull){
		_err_rx++;
		return;
	}
	_err_rx = 0;
	if(_rxbuf.in == (_rxbuf.ou-1) || (_rxbuf.in == _rxbuf.limit && _rxbuf.ou == _rxbuf.mem)){
		*_rxbuf.in++ = c;
		_f_rxfull = true;
	}
	_rxbuf.in = (_rxbuf.in >= _rxbuf.limit)? _rxbuf.mem : _rxbuf.in;
}

