#include "mbed.h"
#include "test.h" 

int main() {    
    test_TrunkController();
    return 0;
}

void mbed_die(void){
    volatile int i=0;
    i++;
}



