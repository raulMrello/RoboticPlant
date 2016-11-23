#include "mbed.h"

#include "mbed.h"
#include "rtos.h"
 
DigitalOut led1(PA_0);
DigitalOut led2(PA_1);
volatile int l1=0,l2=0;
 
void led2_thread() {
    while (true) {
        led2 = !led2;
		l1++;
        Thread::wait(1000);
    }
}
 
int main() {
    Thread t;
	osStatus oserr = t.start(led2_thread);
    
    while (true) {
        led1 = !led1;
		l2++;
        Thread::wait(500);
    }
}

