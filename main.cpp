#include "mbed.h"
#include "test.h" 

int main() {    
    DO_TEST();
    return 0;
}

void mbed_die(void){
    volatile int i=0;
    i++;
}



