#include "mbed.h"
#include "test.h" 

int main() {    
    test_TrunkController();
    for(;;){
        Thread::yield();
    }
    return 0;
}



