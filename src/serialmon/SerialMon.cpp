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

/*************************************************************************************/
SerialMon::SerialMon( PinName tx, PinName rx, int txSize, int rxSize, int baud, const char* name ){    
	_serial = new RawSerial(tx, rx, baud);
	_txbuf.mem = (char*)malloc(txSize);
	if(_txbuf.mem){
		_txbuf.limit = (char*)&_txbuf.mem[txSize];
		_txbuf.in = _txbuf.mem;
		_txbuf.ou = _txbuf.in;
	}
	_rxbuf.mem = (char*)malloc(rxSize);
	if(_rxbuf.mem){
		_rxbuf.limit = (char*)&_rxbuf.mem[rxSize];
		_rxbuf.in = _rxbuf.mem;
		_rxbuf.ou = _rxbuf.in;
	}
	if(name){
		char* _name = (char*)malloc(strlen(name)+1);
		if(_name){
			strcpy(_name, name);
		}
	}
	_f_sending = false;	
	_err_rx = 0;
	_f_started = false;
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
	if(!_f_started){
		start('\n');
	}
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
		_serial->attach(this, &SerialMon::txCallback, (SerialBase::IrqType)TxIrq);
		//_serial->attach<SerialMon >(callback(this, &SerialMon::txCallback), (SerialBase::IrqType)TxIrq);
	}
	_txbuf.mtx.unlock();
	return Ok;
	
}

/*************************************************************************************/
void SerialMon::txCallback(){
	if(_txbuf.ou != _txbuf.in){
		_serial->putc(*_txbuf.ou);
		_txbuf.ou++;
		_txbuf.ou = (_txbuf.ou >= _txbuf.limit)? _txbuf.mem : _txbuf.ou;
		return;
	}
	_serial->attach(0, (SerialBase::IrqType)TxIrq);
	_f_sending = false;
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
