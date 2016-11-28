#include "mbed.h"
#include "SerialMon.h"
 
SerialMon *logger;

volatile int l1=0,l2=0;
 

/************************************************************************************/
void led2_thread() {
	static char txt[128];
	int count = 0;
    while (true) {
 		count++;
		sprintf(txt, "Task2 = %d\r\n", count);
		logger->send(txt);
        Thread::wait(150);
    }
}
 

/************************************************************************************/
int main() {
	logger = new SerialMon(PA_9, PA_10, 512, 512, 115200, "Logger_UART2");
	logger->send((char*)"Iniciando...\r\n");
	static char txt[128];
	int count = 0;
	Thread tled;
	tled.start(led2_thread);
    while (true) {	
		count++;
		std::sprintf(txt, "Task1 = %d\r\n", count);
		logger->send(txt);
        Thread::wait(100);
    }
}

