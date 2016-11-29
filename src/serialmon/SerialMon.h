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

#ifndef SERIALMON_H
#define SERIALMON_H

/** @defgroup API The SERIALMON API */
/** @defgroup MISC Misc SERIALMON functions */
/** @defgroup INTERNALS SERIALMON Internals */

#ifndef SERIALMON_DEFAULT_RX_BUFFER_SIZE
#define SERIALMON_DEFAULT_RX_BUFFER_SIZE    256
#endif

#ifndef SERIALMON_DEFAULT_TX_BUFFER_SIZE
#define SERIALMON_DEFAULT_TX_BUFFER_SIZE    256
#endif

#include "mbed.h"


#define SERIALMON_ENABLE_SIMBUF

class SerialMon {
public:
    
    //! Non-blocking functions return code.
    enum Result {
          Ok = 0                /*!< Ok. */
        , NoMemory       = -1   /*!< Memory allocation failed. */
        , NoChar         = -1   /*!< No character in buffer. */
        , BufferOversize = -2   /*!< Oversized buffer. */
    };
    
    /**
     * The SerialMon constructor is used to initialise the serial object.
     *
     * @param tx PinName of the TX pin.
     * @param rx PinName of the TX pin.
     * @param txBufferSize Integer of the TX buffer sizes.
     * @param rxBufferSize Integer of the RX buffer sizes.
     */    
    SerialMon(PinName tx, PinName rx, int txBufferSize = SERIALMON_DEFAULT_TX_BUFFER_SIZE, int rxBufferSize = SERIALMON_DEFAULT_RX_BUFFER_SIZE, int baud=9600, const char* name = (const char*)"NO NAME");
    
    virtual ~SerialMon();


    /**
     * Function: start
     *  
     * Starts send/recv async operations
     *
     * @ingroup API
     * @param char c The character to detect.
     */
    void start(char c = 0);


    /**
     * Function: writeable
     *  
     * Determine if there is space available to write a byte
     *
     * @ingroup API
     * @return 1 if there is space to write a character, else 0
     */
    int writeable() { return ((_txbuf.in != _txbuf.ou)? 1 : 0); }
    
    /**
     * Function: readable
     *  
     * Determine if there is a byte available to read
     *
     * @ingroup API
     * @return 1 if there is a character available to read, else 0
     */
    int readable() { return (((_rxbuf.in != _rxbuf.ou) || (_rxbuf.in == _rxbuf.ou && _f_rxfull))? 1 : 0); }
    
    
    /**
     * Function: txBufferGetCount
     *  
     * Returns how many bytes are in the TX buffer
     *
     * @ingroup API
     * @return The number of bytes in the TX buffer
     */
    int txBufferGetCount() { return (_txbuf.sz - (_txbuf.in >= _txbuf.ou)? ((_txbuf.limit-_txbuf.in)+(_txbuf.ou-_txbuf.mem)) : (_txbuf.ou-_txbuf.in)); }
    
    /**
     * Function: rxBufferGetCount
     *  
     * Returns how many bytes are in the RX buffer
     *
     * @ingroup API
     * @return The number of bytes in the RX buffer
     */
    int rxBufferGetCount() { return (_rxbuf.sz - (_rxbuf.in >= _rxbuf.ou)? ((_rxbuf.limit-_rxbuf.in)+(_rxbuf.ou-_rxbuf.mem)) : (_rxbuf.ou-_rxbuf.in)); }
    
    /**
     * Function: txBufferGetSize
     *  
     * Returns the current size of the TX buffer
     *
     * @ingroup API
     * @return The length iof the TX buffer in bytes
     */
    int txBufferGetSize() { return _txbuf.sz; } 
    
    /**
     * Function: rxBufferGetSize
     *  
     * Returns the current size of the RX buffer
     *
     * @ingroup API
     * @return The length iof the RX buffer in bytes
     */
    int rxBufferGetSize() { return _rxbuf.sz; } 
    
    /**
     * Function: txBufferFull
     *  
     * Is the TX buffer full?
     *
     * @ingroup API
     * @return true if the TX buffer is full, otherwise false
     */
    bool txBufferFull() { return (((_txbuf.in == _txbuf.ou-1) || (_txbuf.in == _txbuf.mem && _txbuf.ou == _txbuf.limit))? 1 : 0); }
    
    /**
     * Function: rxBufferFull
     *  
     * Is the RX buffer full?
     *
     * @ingroup API
     * @return true if the RX buffer is full, otherwise false
     */
		bool rxBufferFull() { return ((_rxbuf.in == _rxbuf.ou-1 && _f_rxfull)? true : false);}
    
    /**
     * Function: txBufferEmpty
     *  
     * Is the TX buffer empty?
     *
     * @ingroup API
     * @return true if the TX buffer is empty, otherwise false
     */
    bool txBufferEmpty() { return (((char*)_txbuf.in == (char*)_txbuf.ou)? true : false); }
    
    /**
     * Function: rxBufferEmpty
     *  
     * Is the RX buffer empty?
     *
     * @ingroup API
     * @return true if the RX buffer is empty, otherwise false
     */
    bool rxBufferEmpty() { return ((_rxbuf.in == _rxbuf.ou && !_f_rxfull)? true : false);}
    
    /**
     * Function: autoDetectChar
     *
     * Set the char that, if seen incoming, invokes the AutoDetectChar callback.
     *
     * @ingroup API
     * @param int c The character to detect.
     */
    void autoDetectChar(char c) { _auto_detect_char = c; }
    
    /**
     * Function: busy
     *
     * Check is transmisor busy
     *
     * @ingroup API
     */
    bool busy();
     
    /**
     * Function: move
     *
     * Move contents of RX buffer to external buffer. Stops if "end" detected.
     *
     * @ingroup API
     * @param char *s The destination buffer address
     * @param int max The maximum number of chars to move.
     * @param char end If this char is detected stop moving.
     */
    int move(char *s, int max, char end);
    
    /**
     * Function: move (overloaded)
     *
     * Move contents of RX buffer to external buffer. Stops if auto_detect_char detected.
     *
     * @ingroup API
     * @param int max The maximum number of chars to move.
     * @param char *s The destination buffer address
     */
    int move(char *s, int max) {
        return move(s, max, _auto_detect_char);
    }
	
    /**
     * Function: send
     *  
     * Write a string
     *
     * @ingroup API
     * @param string to send.
     */
    Result send(char* text);
    
    #if 0 // Inhereted from Serial/Stream, for documentation only
    /**
     * Function: printf
     *  
     * Write a formated string
     * Inhereted from Serial/Stream
     *
     * @see http://mbed.org/projects/libraries/api/mbed/trunk/Serial#Serial.printf
     * @ingroup API
     * @param format A printf-style format string, followed by the variables to use in formating the string.
     */
    int printf(const char* format, ...);
    #endif
    
    #if 0 // Inhereted from Serial/Stream, for documentation only
    /**
     * Function: scanf
     *  
     * Read a formated string
     * Inhereted from Serial/Stream
     *
     * @see http://mbed.org/projects/libraries/api/mbed/trunk/Serial#Serial.scanf
     * @ingroup API
     * @param format - A scanf-style format string, followed by the pointers to variables to store the results.
     */
    int scanf(const char* format, ...);
    #endif
	void rxCallback();
	void txCallback();
	
	struct topic_t{
		char * txt;
	};
	
protected:
	struct buffer_t{
		Mutex mtx;
		char* mem;
		char* limit;
		char* in;
		char* ou;
		int sz;
	};
	bool _f_sending;
	bool _f_rxfull;
	bool _f_started;
	int _err_rx;
	char * _name;
	char _auto_detect_char;
	buffer_t _txbuf;
	buffer_t _rxbuf;
	char remove();
	Mutex _tx_mut;
	Mutex _rx_mut;
	RawSerial *_serial;
	#if defined(SERIALMON_ENABLE_SIMBUF)
	char * _simbuf;
	int    _simbuf_n;
	#endif
	
};


#endif
