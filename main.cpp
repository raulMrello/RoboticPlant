#include "mbed.h"
#include "test.h" 

int main() {    
    DO_TEST();
    return 0;
}

void mbed_die(void){
    NVIC_SystemReset();
}

void HardFault_Handler(){
    NVIC_SystemReset();
}


