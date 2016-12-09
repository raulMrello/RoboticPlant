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


/** \def SERIALMON_ENABLE_SIMBUF
 *  \brief Enable data buffer for debugging tx isr execution
 */
#define SERIALMON_ENABLE_SIMBUF


/** \def SERIALMON_MAX_COMMAND_LENGTH
 *  \brief Max command length
 */
#define SERIALMON_MAX_COMMAND_LENGTH    64u


/** \def SERIALMON_DEFAULT_RX_BUFFER_SIZE
 *  \brief Default reception buffer size
 */
#ifndef SERIALMON_DEFAULT_RX_BUFFER_SIZE
#define SERIALMON_DEFAULT_RX_BUFFER_SIZE    256
#endif


/** \def SERIALMON_DEFAULT_TX_BUFFER_SIZE
 *  \brief Default transmission buffer size
 */
#ifndef SERIALMON_DEFAULT_TX_BUFFER_SIZE
#define SERIALMON_DEFAULT_TX_BUFFER_SIZE    256
#endif


#include "mbed.h"
#include "MsgBroker.h"


/** \class SerialMon
 *  \brief This class provides an ISR based buffered serial port IO operation. In combination with MsgBroker
 *  class, it is able to handle topic update event notifications. For that purpose, it must provide its own
 *  implementation of MsgBroker::TopicHandler interface, who exposes a unique pure virtual method for such
 *  purpose (updateCallback method).
 */

class SerialMon : public MsgBroker::TopicHandler {
public:
    
    /**
     * SerialMon constructor. It initialises the serial object.
     *
     * @param tx PinName of the TX pin.
     * @param rx PinName of the RX pin.
     * @param txBufferSize Integer of the TX buffer sizes.
     * @param rxBufferSize Integer of the RX buffer sizes.
     * @param baud Baudrate
     * @param name Name of this instance
     * @param rx_detect Character to detect on reception mode, as command EOL
     */    
    SerialMon(PinName tx, PinName rx, int txBufferSize = SERIALMON_DEFAULT_TX_BUFFER_SIZE, int rxBufferSize = SERIALMON_DEFAULT_RX_BUFFER_SIZE, int baud=9600, const char* name = (const char*)"NO NAME", char rx_detect = 0);
 
    
    /**
     * SerialMon destructor
     */    
    virtual ~SerialMon();
    
    /**
     * TopicHandler updateCallback pure virtual function implementation
     *
     * @param topicname Name of the handled topic
     * @param topicdata Topic data pointer
     */    
    virtual void updateCallback(const char * topicname, void * topicdata);


    /**
     * Starts internal thread
     *
     */
    void start(); 

    
    /**
     * Reception ISR callback
     */
    void rxCallback();

    
    /**
     * Transmission ISR callback
     */
    void txCallback(); 
    
    
    //! Topic structure for /log and /cmd topics
    struct topic_t{
        char * txt;
    };
    
protected:
    
    //! Common functions return code.
    enum Result {
          Ok = 0                /*!< Ok. */
        , NoMemory       = -1   /*!< Memory allocation failed. */
        , NoChar         = -1   /*!< No character in buffer. */
        , BufferOversize = -2   /*!< Oversized buffer. */
    };
    
    //! Signals for thread control
    enum Signal {
          CMD_EOL_RECV = 1      // Command EOL received flag
    };

    
    /**
     * Determine if there is a byte available to  from the reception buffer
     *
     * @return true if there is a character available to read, else false
     */
    bool readable() { return (((_rxbuf.in != _rxbuf.ou) || (_rxbuf.in == _rxbuf.ou && _f_rxfull))? true : false); }

    
    /**
     * Function: move (overloaded)
     *
     * Move contents of RX buffer to external buffer. Stops if auto_detect_char detected.
     *
     * @param int max The maximum number of chars to move.
     * @param char *s The destination buffer address
     */
    int move(char *s, int max) {
        return move(s, max, _auto_detect_char);
    }
    
    
    /**
     * Function: move
     *
     * Move contents of RX buffer to external buffer. Stops if "end" detected.
     *
     * @param char *s The destination buffer address
     * @param int max The maximum number of chars to move.
     * @param char end If this char is detected stop moving.
     */
    int move(char *s, int max, char end);
    
    
    /**
     * Write a string
     *
     * @param string to send.
     */
    Result send(char* text);
    
    
    /**
     * Extract a byte from the reception buffer
     *
     * @return byte extracted
     */
    char remove();  
    
    
    //! Buffer structure
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
    Mutex _tx_mut;
    Mutex _rx_mut;
    Thread _thread;
    RawSerial *_serial;
    #if defined(SERIALMON_ENABLE_SIMBUF)
    char * _simbuf;
    int    _simbuf_n;
    #endif
};


#endif
