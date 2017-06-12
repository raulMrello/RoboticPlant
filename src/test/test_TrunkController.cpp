#include "test.h"
#if TEST_TRUNKCONTROLLER == 1

#include "mbed.h"
#include "TrunkController.h"

#define SIG_ACTION_COMPLETE 1
#define SIG_STEP_DONE       2

//------------------------------------------------------------------------------------------------                                    
static TrunkController *tc;
//------------------------------------------------------------------------------------------------                                    
RawSerial serial(PA_2, PA_3);
//------------------------------------------------------------------------------------------------                                    
typedef struct{
	uint16_t degrees[9];
	bool clockwise[9];
}tc_action;
//------------------------------------------------------------------------------------------------                                    
uint8_t action_idx=0;
//------------------------------------------------------------------------------------------------                                    
const tc_action action_list[] =  {	{{1,1,1,1,1,1,1,1,1},{true,true,true,true,true,true,true,true,true}},
									{{2,2,2,2,2,2,2,2,2},{false,false,false,false,false,false,false,false,false}},
									{{30,30,30,30,30,30,30,30,30},{false,false,false,false,false,false,false,false,false}},
									{{10,10,10,10,10,10,10,10,10},{false,false,false,false,false,false,false,false,false}},
									{{30,30,30,30,30,30,30,30,30},{true,true,true,true,true,true,true,true,true}}};
//------------------------------------------------------------------------------------------------                                    
Thread* th;
                                    
//------------------------------------------------------------------------------------------------                                    
static void tc_step_done(){
	//th->signal_set(SIG_STEP_DONE);
}
                                    
//------------------------------------------------------------------------------------------------                                    
static void tc_action_complete(){
	th->signal_set(SIG_ACTION_COMPLETE);
}

//------------------------------------------------------------------------------------------------                                    
static void thread_func(){
    serial.printf("[TEST] Iniciando test_TrunkController!\r\n");
        	
	/** Configura el robot */
	tc = new TrunkController(PA_12, PA_11, PA_14, PA_13, PA_15, Stepper::FULL_STEP, 100, &serial);
    tc->notifyReady(callback(tc_action_complete));
    tc->notifyStep(callback(tc_step_done));
    action_idx = 0;
    
	serial.printf("[TEST] Sending action %d\r\n", action_idx);
    tc->exec((uint16_t*)action_list[action_idx].degrees, (bool*)action_list[action_idx].clockwise);

    for(;;) {  
        osEvent oe = th->signal_wait(0, osWaitForever);
        if((oe.value.signals & SIG_ACTION_COMPLETE) == SIG_ACTION_COMPLETE){
            action_idx = (action_idx < 4)? (action_idx+1) : 0;
            serial.printf("[TEST] Sending action %d\r\n", action_idx);
            tc->exec((uint16_t*)action_list[action_idx].degrees, (bool*)action_list[action_idx].clockwise);
        }
        if((oe.value.signals & SIG_STEP_DONE) == SIG_STEP_DONE){
            serial.printf("[TEST] Step done!\r\n");            
        }
    }
}

//------------------------------------------------------------------------------------------------                                    
int test_TrunkController() {
    th = new Thread();
    th->start(thread_func);
    return 0;
}

#endif

