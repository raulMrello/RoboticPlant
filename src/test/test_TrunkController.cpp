#include "test.h"
#if TEST_TRUNKCONTROLLER == 1

#include "mbed.h"
#include "Logger.h"
#include "TrunkController.h"

//------------------------------------------------------------------------------------------------                                    
#define SIG_ACTION_COMPLETE     1
#define SIG_COMMAND_RECEIVED    2
#define ACTION_COUNT            5
//------------------------------------------------------------------------------------------------                                    
static TrunkController *tc;
//------------------------------------------------------------------------------------------------                                    
static Logger logger(PA_9, PA_10);
#define PRINT_LOG(format, ...)   logger.printf(format, ##__VA_ARGS__);
char cmdbuf[256];
int cmdsize = 0;
//------------------------------------------------------------------------------------------------                                    
typedef struct{
	int16_t degrees[TrunkController::MOTOR_COUNT];
}tc_action;
//------------------------------------------------------------------------------------------------                                    
uint8_t action_idx=0;
//------------------------------------------------------------------------------------------------                                    
const tc_action action_list[] =  {	{-30,-30,-30,-30,-30,-30,-30,-30,-30},
									{-30,-30,-30,-30,-30,-30,-30,-30,-30},
									{ 45, 45, 45, 45, 45, 45, 45, 45, 45},
									{-45,-45,-45,-45,-45,-45,-45,-45,-45},
									{30,30,30,30,30,30,30,30,30}};
//------------------------------------------------------------------------------------------------                                    
Thread* th;                                                                       
//------------------------------------------------------------------------------------------------                                    
static void tc_action_complete(){
	th->signal_set(SIG_ACTION_COMPLETE);
}
//------------------------------------------------------------------------------------------------                                    
static void onRxData(void){
    while(logger.readable()){
        char d = (char)logger.getc();
        cmdbuf[cmdsize++] = d;
        if(d==0){
            th->signal_set(SIG_COMMAND_RECEIVED);
        }
    }    
}
//------------------------------------------------------------------------------------------------                                    
static void thread_func(){
    PRINT_LOG("[TEST] Iniciando test_TrunkController!\r\n");
    
    // Configuro el logger para que permita recibir comandos a través suyo
    logger.attach(callback(onRxData));    
        	
	/** Configura el robot */
	tc = new TrunkController(PB_12, PB_11, PB_14, PB_13, PB_15, Stepper::FULL_STEP, 100, &logger);
    // instala callback de notificación cuando no queden más operaciones pendientes
    tc->notifyReady(callback(tc_action_complete));   
    action_idx = 0;
    
    for(;;){
        PRINT_LOG("[TEST] Esperando comandos...\r\n");
        osEvent oe = th->signal_wait(SIG_COMMAND_RECEIVED, osWaitForever);
        if((oe.value.signals & SIG_COMMAND_RECEIVED) == SIG_COMMAND_RECEIVED){
            if(strcmp(cmdbuf,"TEST")==0){
                PRINT_LOG("[TEST] Intento enviar todo el bloque de acciones...\r\n");
                for(int i=0;i<ACTION_COUNT;i++){
                    bool done;
                    PRINT_LOG("[TEST] Insertando acción %d\r\n", i);
                    do{
                        done = tc->actionRequested((int16_t*)action_list[i].degrees);
                        th->yield();
                    }while(!done);                        
                }
                
        //        // Esto es para detener las acciones en curso
        //        wait(rand()%((7 - 2) + 1) + 2);
        //        PRINT_LOG("\r\n\r\n[TEST] DETENGO acciones!\r\n");
        //        tc->stop();
                
                
                osEvent oe = th->signal_wait(0, osWaitForever);
                if((oe.value.signals & SIG_ACTION_COMPLETE) == SIG_ACTION_COMPLETE){
                    PRINT_LOG("[TEST] Acciones completadas %d!!!!\r\n", tc->getActionCount()); 
                    tc->clearActionCount();
                }                  
            }
            cmdsize = 0;
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

