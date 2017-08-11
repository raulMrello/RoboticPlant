#include "test.h"
#if TEST_PROXIMITY == 1

#include "mbed.h"
#include "Logger.h"
#include "TrunkController.h"
#include "IK9.h"
#include "PirDetector.h"

//------------------------------------------------------------------------------------------------                                    
#define SIMULATE_MOVEMENT		0
//------------------------------------------------------------------------------------------------                                    
#define SIG_ACTION_COMPLETE     (1 << 0)
#define SIG_COMMAND_RECEIVED    (1 << 1)
#define SIG_COMMAND_TIMEOUT     (1 << 2)
#define SIG_COMMAND_OVF         (1 << 3)
#define SIG_LEFT_DETECTOR       (1 << 4)
#define SIG_FRONT_DETECTOR      (1 << 5)
#define SIG_RIGHT_DETECTOR      (1 << 6)
//------------------------------------------------------------------------------------------------                                    
static TrunkController *tc;
static IK9 *ik;
#define LEFT	0
#define FRONT	1
#define RIGHT	2
static PirDetector *pir[3];
static int16_t orientation = TrunkController::MAX_NEGATIVE_MOTOR_ROTATION;
static int16_t inclination = TrunkController::MAX_NEGATIVE_MOTOR_ROTATION;
//------------------------------------------------------------------------------------------------                                    
static SerialTerminal logger(PA_9, PA_10);
char cmdbuf[256];
int cmdsize = 0;
//------------------------------------------------------------------------------------------------                                    
typedef struct{
	int16_t degrees[TrunkController::MOTOR_COUNT];
}tc_action;
//------------------------------------------------------------------------------------------------                                    
static Thread* th;                                                                       
//------------------------------------------------------------------------------------------------                                    
static void tc_action_complete(){
	th->signal_set(SIG_ACTION_COMPLETE);
}
//------------------------------------------------------------------------------------------------                                    
static void onLeftDetector(){
	th->signal_set(SIG_LEFT_DETECTOR);
}
static void onFrontDetector(){
	th->signal_set(SIG_FRONT_DETECTOR);
}
static void onRightDetector(){
	th->signal_set(SIG_RIGHT_DETECTOR);
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
#define IS_FLAG(f)  (oe.status == osEventSignal && (oe.value.signals & (f)) == (f))

//------------------------------------------------------------------------------------------------                                    
static void thread_func(){    
    // Configuro el logger para que permita recibir comandos a través suyo (terminados en '\0')
    logger.config(stream_received, error_timeout, error_ovf, 500, 0);
    logger.startReceiver();
        	
    PRINT_LOG(&logger, "[Release] 10.08.2017 ->commit: Testeando proximidad\r\n\r\n");
    PRINT_LOG(&logger, "[TEST] Iniciando test_PirDetector!\r\n");

	/** Configura el robot */
	bool simulate = false;
	#if SIMULATE_MOVEMENT==1
	simulate = true;
	#warning "MOVIMIENTO SIMULADO"
	#endif
	// Crea controlador motor e instala callback de notificación cuando no queden más operaciones pendientes
    tc = new TrunkController(PB_12, PB_11, PB_14, PB_13, PB_15, Stepper::FULL_STEP, simulate, 100, &logger);
    tc->notifyReady(callback(tc_action_complete));   
	    
    /** Crea el modelo kinemático */
    ik = new IK9(&logger);
	
	// Crea controladores de proximidad e instala callbacks de notificación
	pir[LEFT] = new PirDetector(PA_2, true, &logger);
	pir[FRONT] = new PirDetector(PA_4, true, &logger);
	pir[RIGHT] = new PirDetector(PA_6, true, &logger);
	pir[LEFT]->enable(true, callback(onLeftDetector));
	pir[FRONT]->enable(true, callback(onFrontDetector));
	pir[RIGHT]->enable(true, callback(onRightDetector));
	
	int timeout = osWaitForever;
    
    for(;;){
		PRINT_LOG(&logger, "[TEST] Esperando eventos de proximidad...\r\n");
        osEvent oe = th->signal_wait(0, timeout);
		
		if(oe.status == osEventTimeout){
			int16_t* action;        
			PRINT_LOG(&logger, "[TEST] STAND!\r\n");
			orientation = 0;
			inclination = 0;
			action = ik->goStand();
			if(!action){
				PRINT_LOG(&logger, "[TEST] ERROR no action\r\n");
			}
			else if(!tc->actionRequested(action)){
				PRINT_LOG(&logger, "[TEST] ERROR standing up\r\n");
			}
			else{
				ik->update();
			}
			timeout = osWaitForever;
		}
        
		if(IS_FLAG(SIG_LEFT_DETECTOR)){
			PRINT_LOG(&logger, "[TEST] LEFT!\r\n");
			inclination = 60;
			orientation = 0;
			int16_t *action;
			bool done = false;
			while(!done){
				done = ik->setPosition(orientation, inclination, &action);
				if(!action){
					PRINT_LOG(&logger, "[TEST] ERROR no action\r\n");
					done = true;
				}
				else if(!tc->actionRequested(action)){
					PRINT_LOG(&logger, "[TEST] ERROR left detector\r\n");
					done = true;
				}
				else{
					ik->update();
				}
			}
			timeout = 10000;
		}
		
        if(IS_FLAG(SIG_FRONT_DETECTOR)){
			PRINT_LOG(&logger, "[TEST] FRONT!\r\n");
			inclination = 60;
			orientation = 270;
			int16_t *action;
			bool done = false;
			while(!done){
				done = ik->setPosition(orientation, inclination, &action);
				if(!action){
					PRINT_LOG(&logger, "[TEST] ERROR no action\r\n");
					done = true;
				}
				else if(!tc->actionRequested(action)){
					PRINT_LOG(&logger, "[TEST] ERROR front detector\r\n");
					done = true;
				}
				else{
					ik->update();
				}
			}
			timeout = 10000;
		}
		
        if(IS_FLAG(SIG_RIGHT_DETECTOR)){
			PRINT_LOG(&logger, "[TEST] RIGHT!\r\n");
			inclination = 60;
			orientation = 180;
			int16_t *action;
			bool done = false;
			while(!done){
				done = ik->setPosition(orientation, inclination, &action);
				if(!action){
					PRINT_LOG(&logger, "[TEST] ERROR no action\r\n");
					done = true;
				}
				else if(!tc->actionRequested(action)){
					PRINT_LOG(&logger, "[TEST] ERROR right detector\r\n");
					done = true;
				}
				else{
					ik->update();
				}
			}
			timeout = 10000;
		}
		
        if(IS_FLAG(SIG_COMMAND_TIMEOUT) || IS_FLAG(SIG_COMMAND_OVF)){
            PRINT_LOG(&logger, "[TEST] ERROR. Descarto comando.\r\n");
            // descarta los datos recibidos
            logger.recv(0, 0);
        }
		
		if(IS_FLAG(SIG_ACTION_COMPLETE)){
			PRINT_LOG(&logger, "[TEST_J] Acciones completadas %d!!!!\r\n", tc->getActionCount()); 
			tc->clearActionCount();
		}   

		if(IS_FLAG(SIG_COMMAND_RECEIVED)){        
			cmdsize = logger.recv(cmdbuf, 256);
			/** TEST_T: Realiza movimientos simples por motor
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
                action.degrees[(TrunkController::SEGMENTS_PER_SECTION * section_id) + motor_id] = degree;
                PRINT_LOG(&logger, "[TEST_T] Seccion[%d] Motor[%d] Angulo[%d]\r\n", section_id, motor_id, degree);
				tc->actionRequested((int16_t*)action.degrees);                                
            } 			
		}        		
    }
}

//------------------------------------------------------------------------------------------------                                    
int test_Proximity() {
    th = new Thread();
    th->start(thread_func);
    return 0;
}

#endif

