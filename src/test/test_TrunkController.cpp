#include "test.h"
#if TEST_TRUNKCONTROLLER == 1

#include "mbed.h"
#include "Logger.h"
#include "TrunkController.h"

//------------------------------------------------------------------------------------------------                                    
#define SIG_ACTION_COMPLETE     (1 << 0)
#define SIG_COMMAND_RECEIVED    (1 << 1)
#define SIG_COMMAND_TIMEOUT     (1 << 2)
#define SIG_COMMAND_OVF         (1 << 3)
//------------------------------------------------------------------------------------------------                                    
#define ACTION_COUNT            5
//------------------------------------------------------------------------------------------------                                    
static TrunkController *tc;
//------------------------------------------------------------------------------------------------                                    
static SerialTerminal logger(PA_9, PA_10);
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
static void stream_received(){
	th->signal_set(SIG_COMMAND_RECEIVED);
}
static void error_timeout(){
	th->signal_set(SIG_COMMAND_TIMEOUT);
}
static void error_ovf(){
	th->signal_set(SIG_COMMAND_OVF);
}

//------------------------------------------------------------------------------------------------                                    
#define IS_FLAG(f)  ((oe.value.signals & (f)) == (f))

//------------------------------------------------------------------------------------------------                                    
static void thread_func(){    
    // Configuro el logger para que permita recibir comandos a través suyo (terminados en '\0')
    logger.config(stream_received, error_timeout, error_ovf, 500, 0);
    logger.startReceiver();
        	
    PRINT_LOG(&logger, "[TEST] Iniciando test_TrunkController!\r\n");

	/** Configura el robot */
	tc = new TrunkController(PB_12, PB_11, PB_14, PB_13, PB_15, Stepper::FULL_STEP, 100, &logger);
    // instala callback de notificación cuando no queden más operaciones pendientes
    tc->notifyReady(callback(tc_action_complete));   
    action_idx = 0;
    
    for(;;){
        PRINT_LOG(&logger, "[TEST] Esperando comandos...\r\n");
        osEvent oe = th->signal_wait(0, osWaitForever);
        
        if(IS_FLAG(SIG_COMMAND_TIMEOUT) || IS_FLAG(SIG_COMMAND_OVF)){
            PRINT_LOG(&logger, "[TEST] ERROR. Descarto comando.\r\n");
            // descarta los datos recibidos
            logger.recv(0, 0);
        }
        
        
        if(IS_FLAG(SIG_COMMAND_RECEIVED)){        
            /** Los comandos pueden ser de la siguiente forma:
                Test:  "T,Seccion,Motor,Grados"
                  ej:   "T,0,2,45" -> Mover 45º (clockwise) el motor 2 de la sección 0
                  ej:   "T,1,0,-20"  -> Mover 30º (anti-clockwise) el motor 0 de la sección 1
             */
            cmdsize = logger.recv(cmdbuf, 256);
            if(strncmp(cmdbuf,"T", 1)==0){
                char* token = strtok(cmdbuf,",");                
                int section_id = atoi(strtok(0,","));                
                int motor_id = atoi(strtok(0,","));
                int16_t degree = atoi(strtok(0,","));      
                tc_action action = {{0,0,0,0,0,0,0,0,0}};
                action.degrees[(TrunkController::MOTOR_COUNT * section_id) + motor_id] = degree;
                PRINT_LOG(&logger, "[TEST] Seccion[%d] Motor[%d] Angulo[%d]\r\n", section_id, motor_id, degree);
                bool done;
                do{
                    done = tc->actionRequested((int16_t*)action.degrees);
                    th->yield();
                }while(!done);                        
                
                osEvent oe = th->signal_wait(0, osWaitForever);
                if(IS_FLAG(SIG_ACTION_COMPLETE)){
                    PRINT_LOG(&logger, "[TEST] Acciones completadas %d!!!!\r\n", tc->getActionCount()); 
                    tc->clearActionCount();
                }                  
            }            
        }  
        cmdsize = 0;              
    }
}

//------------------------------------------------------------------------------------------------                                    
int test_TrunkController() {
    th = new Thread();
    th->start(thread_func);
    return 0;
}

#endif

