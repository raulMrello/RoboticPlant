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
static Logger logger(PA_2, PA_3);
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
    logger.attach(callback(onRxData), RawSerial::RxIrq);    
    logger.attach(0, RawSerial::TxIrq);
        	
	/** Configura el robot */
	tc = new TrunkController(PB_12, PB_11, PB_14, PB_13, PB_15, Stepper::FULL_STEP, 100, &logger);
    // instala callback de notificación cuando no queden más operaciones pendientes
    tc->notifyReady(callback(tc_action_complete));   
    action_idx = 0;
    
    for(;;){
        PRINT_LOG("[TEST] Esperando comandos...\r\n");
        osEvent oe = th->signal_wait(SIG_COMMAND_RECEIVED, osWaitForever);
        if((oe.value.signals & SIG_COMMAND_RECEIVED) == SIG_COMMAND_RECEIVED){
            /** Los comandos pueden ser de la siguiente forma:
                Test:  "T,Seccion,Motor,Grados"
                  ej:   "T,0,2,45" -> Mover 45º (clockwise) el motor 2 de la sección 0
                  ej:   "T,1,0,-20"  -> Mover 30º (anti-clockwise) el motor 0 de la sección 1
             */
            if(strncmp(cmdbuf,"T", 1)==0){
                char* token = strtok(cmdbuf,",");                
                int section_id = atoi(strtok(0,","));                
                int motor_id = atoi(strtok(0,","));
                int16_t degree = atoi(strtok(0,","));      
                tc_action action = {{0,0,0,0,0,0,0,0,0}};
                action.degrees[(TrunkController::MOTOR_COUNT * section_id) + motor_id] = degree;
                PRINT_LOG("[TEST] Seccion[%d] Motor[%d] Angulo[%d]\r\n", section_id, motor_id, degree);
                bool done;
                do{
                    done = tc->actionRequested((int16_t*)action.degrees);
                    th->yield();
                }while(!done);                        
                
                
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

