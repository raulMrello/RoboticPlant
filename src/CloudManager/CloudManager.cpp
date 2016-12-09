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
    
    @file          CloudManager.h 
    @purpose       Provides Cloud data storage and communications
    @version       see ChangeLog.c
    @date          Nov 2016
    @author        raulMrello
*/

#include "CloudManager.h"


//--------------------------------------------------------------------------------------------------------------
//-- ISR CALLBACKS ---------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------


///*************************************************************************************/
//void CloudManager::txCallback(){
//	if ((_serial->writeable()) && (_txbuf.in != _txbuf.ou)) {
//		char c = *_txbuf.ou;
//        _serial->putc(c);
//        _txbuf.ou++;
//		_txbuf.ou = (_txbuf.ou >= _txbuf.limit)? _txbuf.mem : _txbuf.ou;
//		#if defined(CLOUDMANAGER_ENABLE_SIMBUF)
//		_simbuf[_simbuf_n] = c;
//		_simbuf_n = (_simbuf_n < _txbuf.sz)? (_simbuf_n+1) : 0;
//		#endif
//		return;
//    }
//	_f_sending = false;
//	_serial->attach(0, (SerialBase::IrqType)TxIrq);
//}


///*************************************************************************************/
//void CloudManager::rxCallback(){
//	while(_serial->readable()){
//		char c = _serial->getc();
//		if(_f_rxfull){
//			_err_rx++;
//			return;
//		}
//		_err_rx = 0;
//		*_rxbuf.in++ = c;
//		_rxbuf.in = (_rxbuf.in >= _rxbuf.limit)? _rxbuf.mem : _rxbuf.in;
//		if(_rxbuf.in == _rxbuf.ou){
//			_f_rxfull = true;
//		}
//		if(c == _auto_detect_char){
//			_thread.signal_set(CMD_EOL_RECV);
//		}
//	}
//}


//--------------------------------------------------------------------------------------------------------------
//-- TOPIC UPDATE CALLBACKS ------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------


/*************************************************************************************/
void CloudManager::updateCallback(const char * topicname, void * topicdata){
	// si se recibe un update del topic /keyb...
	if(strcmp(topicname, "/iot_send") == 0){
		// obtiene un puntero a los datos del topic con el formato correspondiente
		CloudManager::topic_iot_t *topic = (CloudManager::topic_iot_t *)topicdata;
		// chequea el tipo de topic y activa los eventos habilitados en este módulo
		if(topic){
			_mut_iot_send.lock();
			_topic2send = topic;
			_thread.signal_set(CMD_IOT_SEND);
		}
	}
}


//--------------------------------------------------------------------------------------------------------------
//-- PUBLIC METHODS --------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------


/*************************************************************************************/
CloudManager::CloudManager(){    
	MsgBroker::Exception e;
	// install CloudManager topics /log and /cmd
	MsgBroker::installTopic("/iot_send", sizeof(CloudManager::topic_iot_t));
	// attaches to topic updates 
	MsgBroker::attach("/iot_send", this, &e);
	
	// starts reception decodification task
	_thread.start(callback(this, &CloudManager::start));
	// thread clearing flags
	_thread.signal_clr(0xffff);

}


/*************************************************************************************/
CloudManager::~CloudManager(){
	_thread.join();
	_thread.terminate();
}


/*************************************************************************************/
void CloudManager::start(){
	MsgBroker::Exception e;
	
	for(;;){
		// Wait for a Command EOL character reception 
		osEvent oe = _thread.signal_wait(CMD_IOT_SEND, osWaitForever);

		// if event clear signal and continue to process state machine
		if(oe.status == osEventSignal && (oe.value.signals & CMD_IOT_SEND) != 0){
			/* TODO: process data to send */
			MsgBroker::consumed("/iot_send");
			_thread.signal_clr(CMD_IOT_SEND);			
		}
	}	
}


///*************************************************************************************/
//int CloudManager::move(char *s, int max, char end) {
//	int counter = 0;
//	char c;
//	_rxbuf.mtx.lock();
//	while(readable()) {
//		c = *_rxbuf.ou++;
//		_rxbuf.ou = (_rxbuf.ou >= _rxbuf.limit)? _rxbuf.mem : _rxbuf.ou;
//		if(_rxbuf.in != _rxbuf.ou){
//			_f_rxfull = false;
//		}
//		if (c == end) break;
//		*(s++) = c;
//		counter++;
//		if (counter == max) break;
//	}
//	_rxbuf.mtx.unlock();
//	return counter;
//}

///*************************************************************************************/
//CloudManager::Result CloudManager::send(char* text){
//	_txbuf.mtx.lock();	
//	int remaining = (_txbuf.in >= _txbuf.ou)? ((_txbuf.limit-_txbuf.in)+(_txbuf.ou-_txbuf.mem)) : (_txbuf.ou-_txbuf.in);
//	if(strlen(text) > remaining){
//		return BufferOversize;
//	}
//	for(int i=0;i<strlen(text)+1;i++){
//		*_txbuf.in++ = text[i];
//		_txbuf.in = (_txbuf.in >= _txbuf.limit)? _txbuf.mem : _txbuf.in;
//	}
//	if(!_f_sending){
//		_f_sending = true;
//		if(_f_started){
//			_serial->attach(0, (SerialBase::IrqType)TxIrq);
//			txCallback();
//		}
//		_f_started = true;
//		_serial->attach(callback(this, &CloudManager::txCallback), (SerialBase::IrqType)TxIrq);
//	}
//	_txbuf.mtx.unlock();
//	return Ok;
//	
//}

///*************************************************************************************/
//char CloudManager::remove(){
//	char c = *_rxbuf.ou++;
//	_rxbuf.ou = (_rxbuf.ou >= _rxbuf.limit)? _rxbuf.mem : _rxbuf.ou;
//	if(_rxbuf.in != _rxbuf.ou){
//		_f_rxfull = false;
//	}
//	return c;	
//}
