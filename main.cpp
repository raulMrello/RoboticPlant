#include "mbed.h"
#include "SerialMon.h"
#include "MsgBroker.h"
 
SerialMon *logger;

volatile int l1=0,l2=0;
 

/************************************************************************************/
void led2_thread() {
	static char txt[128];
	int count = 0;
	MsgBroker::Exception e;
	SerialMon::topic_t topic;
	while (true) {
 		count++;
		sprintf(txt, "Task2 = %d\r\n", count);
		topic.txt = txt;
		MsgBroker::publish("/log", &topic, sizeof(SerialMon::topic_t), &e);    
//		logger->send(txt);
        Thread::wait(150);
    }
}
 

/************************************************************************************/
int main() {
	/** Configura la infraestructura de publicacion/subscripción */
	MsgBroker::init();
	
	/** Configura el módulo de depuración */	
	logger = new SerialMon(PA_9, PA_10, 512, 512, 115200, "Logger_UART2");
	
	/** Envía traza */
	MsgBroker::Exception e;
	SerialMon::topic_t topic;
	topic.txt = (char*)"Iniciando...\r\n";
	MsgBroker::publish("/log", &topic, sizeof(SerialMon::topic_t), &e);
//	logger->send((char*)"Iniciando...\r\n");
	static char txt[128];
	int count = 0;
	Thread tled;
	tled.start(led2_thread);
    while (true) {	
		count++;		
		std::sprintf(txt, "Task1 = %d\r\n", count);
		topic.txt = txt;
		MsgBroker::publish("/log", &topic, sizeof(SerialMon::topic_t), &e);
//		logger->send(txt);
        Thread::wait(100);
    }
}

