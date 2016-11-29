/*
 * MsgBroker.h
 *
 *  Created on: 20/04/2015
 *      Author: raulMrello
 */

#include "MsgBroker.h"

/** \var topic_list
 *  \brief Topic list pointer, initialy undefined
 */
List<MsgBroker::Topic> * MsgBroker::topic_list = 0;

/** Global mutex for MsgBroker under Mbed infrastructure */
#if __MBED__ == 1
#include "Mutex.h"
Mutex broker_mutex;
#endif

