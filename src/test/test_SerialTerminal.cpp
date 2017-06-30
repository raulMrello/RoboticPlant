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
    
    @file          test_SerialTerminal.cpp
    @purpose       Clase que implementa un terminal serie para recibir comandos
                   remotos. Utiliza únicamente el pin Rx. Proporciona callbacks
                   para notificar el fin de recepción, el fallo por timeout y el 
                   fallo por desborde de buffer. Utiliza un caracter para detectar
                   el fin de trama.
    @date          Jul 2017
    @author        raulMrello
*/


#include "test.h"
#if TEST_SERIALTERMINAL == 1

#include "mbed.h"
#include "SerialTerminal.h"

#define IS_FLAG(f)  ((oe.value.signals & (f)) == (f))
//------------------------------------------------------------------------------------------------                                    
//static Logger logger(PA_2, PA_3);
//#define PRINT_LOG(format, ...)   logger.printf(format, ##__VA_ARGS__);
char cmdbuf[256];
int cmdsize = 0;

//------------------------------------------------------------------------------------------------                                    
Thread* th;   

//------------------------------------------------------------------------------------------------                                    
static const uint32_t SIG_DATA_READY    = (1 << 0);
static const uint32_t SIG_ERR_TIMEOUT   = (1 << 1);
static const uint32_t SIG_ERR_OVF       = (1 << 2);
static const uint32_t SIG_DATA_SENT     = (1 << 3);

//------------------------------------------------------------------------------------------------                                    
static void stream_sent(){
	th->signal_set(SIG_DATA_SENT);
}
static void stream_received(){
	th->signal_set(SIG_DATA_READY);
}
static void error_timeout(){
	th->signal_set(SIG_ERR_TIMEOUT);
}
static void error_ovf(){
	th->signal_set(SIG_ERR_OVF);
}

//------------------------------------------------------------------------------------------------                                    
static void thread_func(){
    // Configuro el terminal y lo inicio
    SerialTerminal* st = new SerialTerminal(PA_2, PA_3, 64);  
    st->config(callback(stream_received), callback(error_timeout), callback(error_ovf), 500, 0);  
    st->startReceiver();
    st->printf("Test iniciado\r\n");
    for(;;){
        if(!st->isTxManaged()){
            st->printf("Esperando comando...\r\n");    
        }        
        osEvent oe = th->signal_wait(0, osWaitForever);
        
        if(IS_FLAG(SIG_DATA_READY)){
            cmdsize = st->recv(cmdbuf, 256);
            st->printf("OK. Se han recibido %d bytes [%s]\r\n", cmdsize, cmdbuf);   
            if(!st->isTxManaged()){
                st->startTransmitter();
                th->wait(100);
            }        
            st->send((uint8_t*)"Te contesto esto", 17, stream_sent);
        }  

        if(IS_FLAG(SIG_ERR_TIMEOUT)){
            cmdsize = st->recv(cmdbuf, 256);
            st->printf("ERROR_TIMEOUT. Se han recibido %d bytes\r\n", cmdsize);    
        }  

        if(IS_FLAG(SIG_ERR_OVF)){
            cmdsize = st->recv(cmdbuf, 256);
            st->printf("ERROR_OVERFLOW. Se han recibido %d bytes\r\n", cmdsize);    
        }  

        if(IS_FLAG(SIG_DATA_SENT)){
            if(st->isTxManaged()){
                st->stopTransmitter();
            }        
            st->printf("DATA_SENT.\r\n");    
        }  
        
    }
}

//------------------------------------------------------------------------------------------------                                    
int test_SerialTerminal() {
    th = new Thread();
    th->start(thread_func);
    return 0;
}

#endif

