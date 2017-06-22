#include "test.h"
#if TEST_TRUNKCONTROLLER == 1

#include "mbed.h"
#include "Logger.h"
#include "TrunkController.h"

//------------------------------------------------------------------------------------------------                                    
#define SIG_ACTION_COMPLETE 1
#define ACTION_COUNT        5
//------------------------------------------------------------------------------------------------                                    
static TrunkController *tc;
//------------------------------------------------------------------------------------------------                                    
static RawSerial serial(PA_9, PA_10);
static Mutex serial_mutex;
static Logger logger = {&serial, &serial_mutex};
#define PRINT_LOG(format, ...)   logger.mtx->lock(); logger.serial->printf(format, ##__VA_ARGS__); logger.mtx->unlock();

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
static void thread_func(){
    PRINT_LOG("[TEST] Iniciando test_TrunkController!\r\n");
        	
	/** Configura el robot */
	tc = new TrunkController(PB_12, PB_11, PB_14, PB_13, PB_15, Stepper::FULL_STEP, 100, &logger);
    // instala callback de notificación cuando no queden más operaciones pendientes
    tc->notifyReady(callback(tc_action_complete));   
    action_idx = 0;
    
    for(;;){
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
}

//------------------------------------------------------------------------------------------------                                    
int test_TrunkController() {
    th = new Thread();
    th->start(thread_func);
    return 0;
}

#endif

