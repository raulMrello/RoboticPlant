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
//-- STATIC  ---------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

///*************************************************************************************/
static char* http_get(NetworkInterface *net, char * sbuffer) {
    TCPSocket socket;

    // Show the network address
    const char *ip = net->get_ip_address();
    printf("IP address is: %s\n", ip ? ip : "No IP");

    // Open a socket on the network interface, and create a TCP connection to mbed.org
    socket.open(net);
    socket.connect("developer.mbed.org", 80);

    // Send a simple http request
    int scount = socket.send(sbuffer, sizeof sbuffer);
    printf("sent %d [%.*s]\r\n", scount, strstr(sbuffer, "\r\n")-sbuffer, sbuffer);

    // Recieve a simple http response and print out the response line
    char rbuffer[64];
    int rcount = socket.recv(rbuffer, sizeof rbuffer);
    printf("recv %d [%.*s]\r\n", rcount, strstr(rbuffer, "\r\n")-rbuffer, rbuffer);

    // Close the socket to return its memory and bring down the network interface
    socket.close();
	char *response = (char*)malloc(64);
	if(response){
		memcpy(response, rbuffer, 64);
	}
	return response;
}

///*************************************************************************************/
static char* sock_send(NetworkInterface *net, char *server, int port, char * data, int maxrecvsz, int *recvsz) {
    TCPSocket socket;

    // Show the network address
    const char *ip = net->get_ip_address();

    // Open a socket on the network interface, and create a TCP connection to mbed.org
    socket.open(net);
    socket.connect(server, port);

    // Send a simple http request
    int scount = socket.send(data, strlen(data));

    // Recieve a simple http response and print out the response line
    char *rbuffer = (char*)malloc(maxrecvsz);
	int rcount = 0;
	if(rbuffer){
		rcount = socket.recv(rbuffer, sizeof rbuffer);
	}

    // Close the socket to return its memory and bring down the network interface
    socket.close();
	*recvsz = rcount;
	return rbuffer;
}

//--------------------------------------------------------------------------------------------------------------
//-- ISR CALLBACKS ---------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------
//-- TOPIC UPDATE CALLBACKS ------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------


/*************************************************************************************/
void CloudManager::onNewTopic(const char * topicname, void * topicdata){
	// si se recibe un update del topic /keyb...
	if(strcmp(topicname, "/iot") == 0){
		// obtiene un puntero a los datos del topic con el formato correspondiente
		CloudManager::topic_iot_t *topic = (CloudManager::topic_iot_t *)topicdata;
		// chequea el tipo de topic y activa los eventos habilitados en este módulo
		if(topic){
			_mut_iot.lock();
			topic_iot_t *tpc = (topic_iot_t*)malloc(sizeof(topic_iot_t));
			char *data = 0;
			if(topic->size){
				data = (char*)malloc(topic->size);
				if(data){
					memcpy(data, (char*)topic->data, topic->size);
				}
			}
			if(tpc){
				tpc->cmd = topic->cmd;
				tpc->data = data;
				tpc->size = topic->size;
				_queue.put(tpc);
			}
			MsgBroker::consumed("/iot");
			_mut_iot.unlock();
		}
	}
}


//--------------------------------------------------------------------------------------------------------------
//-- PUBLIC METHODS --------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------


/*************************************************************************************/
CloudManager::CloudManager(PinName tx, PinName rx){    
	MsgBroker::Exception e;
	// install CloudManager topics /log and /cmd
	MsgBroker::installTopic("/iot", sizeof(CloudManager::topic_iot_t));
	// attaches to topic updates 
	MsgBroker::attach("/iot", this, &CloudManager::onNewTopic, &e);
	// initializes logger
	_logger = 0;
	// reference WifiInterface
	_esp = new ESP8266Interface(tx, rx);
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
		// Wait for messages
		osEvent oe = _queue.get();
        if (oe.status == osEventMessage) {
			// get referenced topic
			topic_iot_t *topic = (topic_iot_t*)oe.value.p;	
			
			// process topic according with command type
			switch(topic->cmd){
				case CMD_IOT_CONN:{
					char *essid = strtok((char*)topic->data, ",");
					char *passw = strtok(0,",");
					char *secur = strtok(0,",");
					_esp->connect(essid, passw, (nsapi_security_t)(secur[0]-48));
					break;
				}
				case CMD_IOT_DISC:{
					_esp->disconnect();
					break;
				}
				case CMD_IOT_SOCKSEND:{
					char *server = strtok((char*)topic->data, ",");
					char *port = strtok(0,",");
					char *data = strtok(0,",");
					int recvdata = 0;
					char *response = sock_send(_esp, server, atoi(port), data, 32, &recvdata);
					if(_logger){
						SerialMon::topic_t topic;
						topic.data = (uint8_t*)response;
						topic.size = strlen(response)+1;
						MsgBroker::publish("/log", &topic, sizeof(SerialMon::topic_t), &e);
					}
					free(response);
					break;
				}
				case CMD_IOT_HTTPGET:{
					char *response = http_get(_esp, (char*)topic->data);
					if(_logger){
						SerialMon::topic_t topic;
						topic.data = (uint8_t*)response;
						topic.size = strlen(response)+1;
						MsgBroker::publish("/log", &topic, sizeof(SerialMon::topic_t), &e);
					}
					free(response);
					break;
				}
			}
			
			// deallocates reserved memory
			if(topic->data){
				free(topic->data);
			}
			free(topic);
		}
	}	
}

