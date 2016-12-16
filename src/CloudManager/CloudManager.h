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

#ifndef CLOUDMANAGER_H
#define CLOUDMANAGER_H


///** \def CLOUDMANAGER_ENABLE_SIMBUF
// *  \brief Enable data buffer for debugging tx isr execution
// */
//#define CLOUDMANAGER_ENABLE_SIMBUF


///** \def CLOUDMANAGER_MAX_COMMAND_LENGTH
// *  \brief Max command length
// */
//#define CLOUDMANAGER_MAX_COMMAND_LENGTH	64u


///** \def CLOUDMANAGER_DEFAULT_RX_BUFFER_SIZE
// *  \brief Default reception buffer size
// */
//#ifndef CLOUDMANAGER_DEFAULT_RX_BUFFER_SIZE
//#define CLOUDMANAGER_DEFAULT_RX_BUFFER_SIZE    256
//#endif


///** \def CLOUDMANAGER_DEFAULT_TX_BUFFER_SIZE
// *  \brief Default transmission buffer size
// */
//#ifndef CLOUDMANAGER_DEFAULT_TX_BUFFER_SIZE
//#define CLOUDMANAGER_DEFAULT_TX_BUFFER_SIZE    256
//#endif


#include "mbed.h"
#include "MsgBroker.h"
#include "TCPSocket.h"
#include "ESP8266Interface.h"
#include "SerialMon.h"


/** \class CloudManager
 *  \brief This class provides an interface to access cloud resources. This interfaces inherits an internal thread
 *	to carry out blocking operations.
 */

class CloudManager {
public:
	
    /**
     * CloudManager constructor. It initialises the serial object.
     *
     * @param tx ESP8266 TX pin
	 * @param rx ESP8266 RX pin
     */    
    CloudManager(PinName tx, PinName rx);
 
	
    /**
     * CloudManager destructor
     */    
    virtual ~CloudManager();
	
    /**
     * Topic updates' listener
     *
     * @param topicname Name of the handled topic
     * @param topicdata Topic data pointer
     */    
	virtual void onNewTopic(const char * topicname, void * topicdata);


    /**
     * Starts internal thread
     *
     */
    void start(); 


    /**
     * Starts internal thread
     *
     */
    void setLogger(SerialMon *logger){ _logger = logger; } 
	
	
	//! Topic structure for /iot_* topics
	struct topic_iot_t{
		int cmd;
		void * data;
		int size;
	};
		
	//! Signals for thread control
    enum Signal {
          CMD_IOT_CONN = 1,		// Connect with remote server
          CMD_IOT_DISC = 2,		// Disconnect with remote server
          CMD_IOT_HTTPGET = 3,	// Command for http get tests
          CMD_IOT_SOCKSEND = 4	// Command to request-response data from socket
    };

	
protected:
	
//    //! Common functions return code.
//    enum Result {
//          Ok = 0                /*!< Ok. */
//        , NoMemory       = -1   /*!< Memory allocation failed. */
//        , NoChar         = -1   /*!< No character in buffer. */
//        , BufferOversize = -2   /*!< Oversized buffer. */
//    };

	
//    /**
//     * Determine if there is a byte available to  from the reception buffer
//     *
//     * @return true if there is a character available to read, else false
//     */
//	bool readable() { return (((_rxbuf.in != _rxbuf.ou) || (_rxbuf.in == _rxbuf.ou && _f_rxfull))? true : false); }

//	
//    /**
//     * Function: move (overloaded)
//     *
//     * Move contents of RX buffer to external buffer. Stops if auto_detect_char detected.
//     *
//     * @param int max The maximum number of chars to move.
//     * @param char *s The destination buffer address
//     */
//    int move(char *s, int max) {
//        return move(s, max, _auto_detect_char);
//    }
//	
//	
//    /**
//     * Function: move
//     *
//     * Move contents of RX buffer to external buffer. Stops if "end" detected.
//     *
//     * @param char *s The destination buffer address
//     * @param int max The maximum number of chars to move.
//     * @param char end If this char is detected stop moving.
//     */
//    int move(char *s, int max, char end);
//	
//	
//    /**
//     * Write a string
//     *
//     * @param string to send.
//     */
//    Result send(char* text);
//	
//	
//    /**
//     * Extract a byte from the reception buffer
//     *
//     * @return byte extracted
//     */
//	char remove();	
//	
//	
//	//! Buffer structure
//	struct buffer_t{
//		Mutex mtx;
//		char* mem;
//		char* limit;
//		char* in;
//		char* ou;
//		int sz;
//	};

//	bool _f_sending;
//	bool _f_rxfull;
//	bool _f_started;
//	int _err_rx;
//	char * _name;
//	char _auto_detect_char;
//	buffer_t _txbuf;
//	buffer_t _rxbuf;
//	Mutex _tx_mut;
//	Mutex _rx_mut;
//	RawSerial *_serial;
//	#if defined(CLOUDMANAGER_ENABLE_SIMBUF)
//	char * _simbuf;
//	int    _simbuf_n;
//	#endif
	Thread _thread;
	topic_iot_t * _topic2send;
	Mutex _mut_iot;
	ESP8266Interface *_esp;
	SerialMon * _logger;
	Queue<topic_iot_t, 4> _queue;
};


#endif
