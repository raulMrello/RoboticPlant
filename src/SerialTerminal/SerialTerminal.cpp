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
    
    @file          SerialTerminal.h 
    @purpose       Clase que implementa un terminal serie para recibir comandos
                   remotos. Utiliza �nicamente el pin Rx. Proporciona callbacks
                   para notificar el fin de recepci�n, el fallo por timeout y el 
                   fallo por desborde de buffer. Utiliza un caracter para detectar
                   el fin de trama.
    @date          Jul 2017
    @author        raulMrello
*/

#include "SerialTerminal.h"

//---------------------------------------------------------------------------------
//- PRIVATE -----------------------------------------------------------------------
//---------------------------------------------------------------------------------

static void unhandled_callback(){}

//---------------------------------------------------------------------------------
//- ISR ---------------------------------------------------------------------------
//---------------------------------------------------------------------------------


//---------------------------------------------------------------------------------
void SerialTerminal::onTxData(){
    if(writeable()){
        if(_sent < _tosend){
            putc(_tbuf[_sent++]);
        }
        else if(_sent){
            _tosend = 0;
            _cb_tx.call();
            _cb_tx = callback(unhandled_callback);
        }
    }
}

//---------------------------------------------------------------------------------
void SerialTerminal::onRxData(){
    while(readable()){
        char d = (char)getc();
        if(_recv >= _bufsize){
            _tmr.detach();
            attach(0, (SerialBase::IrqType)RxIrq);
            _cb_rx_ovf.call();
            return;
        }
        _databuf[_recv++] = d;        
        if(d == _eof){
            _tmr.detach();
            attach(0, (SerialBase::IrqType)RxIrq);
            _cb_rx.call();
        }
        else if(_recv == 1){
            _tmr.attach_us(callback(this, &SerialTerminal::onRxTimeout), _us_timeout);
        }
    }
}

//---------------------------------------------------------------------------------
void SerialTerminal::onRxTimeout(){
    _tmr.detach();
    attach(0, (SerialBase::IrqType)RxIrq);
    _cb_rx_tmr.call();
}



//---------------------------------------------------------------------------------
//- IMPL. -------------------------------------------------------------------------
//---------------------------------------------------------------------------------

SerialTerminal::SerialTerminal(PinName tx, PinName rx, uint16_t maxbufsize, int baud) : RawSerial(tx, rx, baud){
    attach(0, (SerialBase::IrqType)RxIrq);
    attach(0, (SerialBase::IrqType)TxIrq);
    _recv = 0;
    _eof = 0;
    _bufsize = maxbufsize;
    _databuf = (char*)malloc(maxbufsize);
    _sent = 0;
    _tosend = 0;
    _tbuf = 0;
    _cb_tx = callback(unhandled_callback);
    _cb_rx = callback(unhandled_callback);
    _cb_rx_tmr = callback(unhandled_callback);
    _cb_rx_ovf = callback(unhandled_callback);
    tx_managed = false;
    rx_managed = false;
}

//---------------------------------------------------------------------------------
bool SerialTerminal::config(Callback<void()> rx_done, Callback <void()> rx_timeout, Callback <void()> rx_ovf, uint32_t millis, char eof){
    _cb_rx = rx_done;
    _cb_rx_tmr = rx_timeout;
    _cb_rx_ovf = rx_ovf;
    _eof = eof;
    _us_timeout = 1000 * millis;
    return (_databuf && _bufsize)? true : false;
}

//---------------------------------------------------------------------------------
void SerialTerminal::startManaged(bool transmitter, bool receiver){
    if(transmitter){
        tx_managed = true;
        _sent = 0;
        _tosend = 0;
        attach(callback(this, &SerialTerminal::onTxData), (SerialBase::IrqType)TxIrq);
    }
    if(receiver){
        rx_managed = true;
        _recv = 0;
        attach(callback(this, &SerialTerminal::onRxData), (SerialBase::IrqType)RxIrq);
    }
}

//---------------------------------------------------------------------------------
void SerialTerminal::stopManaged(bool transmitter, bool receiver){
    if(transmitter){
        tx_managed = false;
        _sent = 0;
        _tosend = 0;
        attach(0, (SerialBase::IrqType)TxIrq);
    }
    if(receiver){
        rx_managed = false;
        _recv = 0;
        attach(0, (SerialBase::IrqType)RxIrq);
    }
}

//---------------------------------------------------------------------------------
bool SerialTerminal::send(uint8_t* data, uint16_t size, Callback<void()> tx_done){    
    if(data && size){
        _sent = 0;
        _tosend = size;
        _cb_tx = tx_done;
        _tbuf = data;
        putc(_tbuf[_sent++]);
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------------
uint16_t SerialTerminal::recv(char* buf, uint16_t maxsize, bool enable_receiver){
    uint16_t nb = (_recv > maxsize)? maxsize : _recv;
    if(nb && buf){
        memcpy(buf, _databuf, nb);
    }
    if(enable_receiver){
        startReceiver();
    }
    return nb;
}

//---------------------------------------------------------------------------------
bool SerialTerminal::isTxManaged(){
    return tx_managed;
}

//---------------------------------------------------------------------------------
bool SerialTerminal::isRxManaged(){
    return rx_managed;
}
