#include "mbed.h"
#include "SerialMon.h"
 
DigitalOut led1(PA_0);
DigitalOut led2(PA_1);
SerialMon *logger;

volatile int l1=0,l2=0;
 

/************************************************************************************/
void led2_thread() {
	char *txt = (char*)"Task2 = 0000\r\n";	
    while (true) {
        led2 = !led2;
		l1++;
		sprintf(txt, "Task2 = %d\r\n", l2);
		logger->send(txt);
        Thread::wait(1000);
    }
}
 

/************************************************************************************/
int main() {
//	Serial s(PA_9,PA_10);
//	s.baud(115200);
//	s.printf("HELLO");
	logger = new SerialMon(PA_9, PA_10, 512, 512, 115200, "Logger_UART2");
	logger->send((char*)"Iniciando...\r\n");
//	char *txt = (char*)"Task1 = 000000000000\r\n";
//    Thread tled;
//	tled.start(led2_thread);
    while (true) {	
//        led1 = !led1;
//		l2++;
//		sprintf(txt, "Task1 = %d\r\n", l2);
		logger->send("t\r\n");
		while(logger->busy());
//        Thread::wait(500);
    }
}

