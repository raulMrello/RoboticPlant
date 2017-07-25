#include "test.h"
#if TEST_TRUNKCONTROLLER == 1

#include "mbed.h"
#include "Logger.h"
#include "TrunkController.h"
#include "IKModel.h"

//------------------------------------------------------------------------------------------------                                    
#define SIG_ACTION_COMPLETE     (1 << 0)
#define SIG_COMMAND_RECEIVED    (1 << 1)
#define SIG_COMMAND_TIMEOUT     (1 << 2)
#define SIG_COMMAND_OVF         (1 << 3)
//------------------------------------------------------------------------------------------------                                    
#define ACTION_COUNT            5
//------------------------------------------------------------------------------------------------                                    
static TrunkController *tc;
static IKModel *ik;
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
        	
    PRINT_LOG(&logger, "[Release] 25.07.2017 ->commit: Completado IKModel\r\n\r\n");
    PRINT_LOG(&logger, "[TEST] Iniciando test_TrunkController!\r\n");

	/** Configura el robot */
	tc = new TrunkController(PB_12, PB_11, PB_14, PB_13, PB_15, Stepper::FULL_STEP, 100, &logger);
    // instala callback de notificación cuando no queden más operaciones pendientes
    tc->notifyReady(callback(tc_action_complete));   
    action_idx = 0;
    
    /** Configura el modelo kinemático */
    ik = new IKModel(&logger);
    
    for(;;){
        PRINT_LOG(&logger, "[TEST] Esperando comandos...\r\n");
        osEvent oe = th->signal_wait(0, osWaitForever);
        
        if(IS_FLAG(SIG_COMMAND_TIMEOUT) || IS_FLAG(SIG_COMMAND_OVF)){
            PRINT_LOG(&logger, "[TEST] ERROR. Descarto comando.\r\n");
            // descarta los datos recibidos
            logger.recv(0, 0);
        }
        
        
        if(IS_FLAG(SIG_COMMAND_RECEIVED)){        
            cmdsize = logger.recv(cmdbuf, 256);
            /** TEST_I: Realiza movimientos kinemáticos IK
                Test:  "I,Tipo[Stand|Left|Right|Up|Down|HeadUp|HeadDown],Potencia[VerySmall|Small|Medium|High|VeryHigh]"
                  ej:   "I,L,VS" -> Mover a (L)izq (VS)muy poco
                  ej:   "I,U,M"  -> Mover (U)arriba (M)medio
                  ej:   "I,HD,H"  -> Mover (HD)cabeza_abajo (M)bastante
             */
            if(strncmp(cmdbuf,"I", 1)==0){
                char* token = strtok(cmdbuf,",");                
                char* type = strtok(0,",");                
                char* level = strtok(0,",");
                PRINT_LOG(&logger, "[TEST_I] Move %s Power %s\r\n", type, level);
                IKModel::IKLevel_enum iklevel = IKModel::NONE;
                if(level[0] == 'N'){
                    iklevel = IKModel::NONE;
                }
                else if(level[0] == 'V' && level[1] == 'S'){
                    iklevel = IKModel::VERY_SMALL;
                }
                else if(level[0] == 'S'){
                    iklevel = IKModel::SMALL;
                }
                else if(level[0] == 'M'){
                    iklevel = IKModel::MEDIUM;
                }
                else if(level[0] == 'H'){
                    iklevel = IKModel::HIGH;
                }
                else if(level[0] == 'V' && level[1] == 'H'){
                    iklevel = IKModel::VERY_HIGH;
                }
                
                int16_t* action = 0;
                if(type[0] == 'S'){
                    action = ik->goStand();
                }
                else if(type[0] == 'L'){
                    action = ik->goLeft(iklevel);
                }
                else if(type[0] == 'R'){
                    action = ik->goRight(iklevel);
                }
                else if(type[0] == 'U'){
                    action = ik->goUp(iklevel);
                }
                else if(type[0] == 'D'){
                    action = ik->goDown(iklevel);
                }
                else if(type[0] == 'H' && type[1] == 'U'){
                    action = ik->headUp(iklevel);
                }
                else if(type[0] == 'H' && type[1] == 'D'){
                    action = ik->headDown(iklevel);
                }
                if(!action){
                    PRINT_LOG(&logger, "[TEST_I] ERROR. No hay acciones\r\n"); 
                }
                else{                
                    bool done;
                    do{
                        done = tc->actionRequested(action);
                        th->yield();
                    }while(!done);                                        
                    osEvent oe = th->signal_wait(0, osWaitForever);
                    if(IS_FLAG(SIG_ACTION_COMPLETE)){
                        ik->update();
                        PRINT_LOG(&logger, "[TEST_I] Acciones completadas %d!!!!\r\n", tc->getActionCount()); 
                        tc->clearActionCount();
                    }        
                }                    
            }   

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
                bool done;
                do{
                    done = tc->actionRequested((int16_t*)action.degrees);
                    th->yield();
                }while(!done);                                        
                osEvent oe = th->signal_wait(0, osWaitForever);
                if(IS_FLAG(SIG_ACTION_COMPLETE)){
                    PRINT_LOG(&logger, "[TEST_T] Acciones completadas %d!!!!\r\n", tc->getActionCount()); 
                    tc->clearActionCount();
                }                  
            }   
            
//            /** TEST_M: Realiza movimientos por sección, de los motores, uno cada vez con vaivén de ida y vuelta a reposo
//                Test:  "M,Sección,Grados"
//                  ej:   "M,0,45" -> Realiza movimientos de cada motor independientes
//             */
//            if(strncmp(cmdbuf,"M", 1)==0){
//                char* token = strtok(cmdbuf,",");                
//                int section_id = atoi(strtok(0,","));                
//                int16_t degree = atoi(strtok(0,","));
//                tc_action action = {{0,0,0,0,0,0,0,0,0}};                
//                for(uint8_t i=0;i<6;i++){
//                    int motor_id = (TrunkController::SEGMENTS_PER_SECTION * section_id) + (i/2);
//                    action.degrees[motor_id] = ((i & 1)==0)? degree : (-degree);                
//                    PRINT_LOG(&logger, "[TEST_M] Seccion[%d] Motor[%d] Angulo[%d]\r\n", section_id, motor_id, action.degrees[motor_id]);
//                    bool done;
//                    do{
//                        done = tc->actionRequested((int16_t*)action.degrees);
//                        th->yield();
//                    }while(!done); 
//					action.degrees[motor_id] = 0;
//                }                
//                osEvent oe = th->signal_wait(0, osWaitForever);
//                if(IS_FLAG(SIG_ACTION_COMPLETE)){
//                    PRINT_LOG(&logger, "[TEST_M] Acciones completadas %d!!!!\r\n", tc->getActionCount()); 
//                    tc->clearActionCount();
//                }                  
//            }      
//            
//            /** TEST_W: Realiza movimientos de vaivén de varias secciones agrupadas, el movimiento es de un segmento cada
//                vez (un segmento es la alineación de varios motores)
//                Test:  "W,Sección,Grados"
//                  ej:   "W,1,45" -> Realiza movimientos de cada motor independientes de la sección 1 y 2
//                  ej:   "W,0,45" -> Realiza movimientos de cada motor independientes de la sección 0, 1 y 2
//             */
//            if(strncmp(cmdbuf,"W", 1)==0){
//                char* token = strtok(cmdbuf,",");                
//                int section_id = atoi(strtok(0,","));                
//                int16_t degree = atoi(strtok(0,","));
//                tc_action action = {{0,0,0,0,0,0,0,0,0}};                
//                for(uint8_t i=0;i<6;i++){
//                    for(int j=section_id; j<TrunkController::SECTION_COUNT; j++){
//                        int motor_id = (TrunkController::SEGMENTS_PER_SECTION * j) + (i/2);
//                        action.degrees[motor_id] = ((i & 1)==0)? degree : (-degree);                
//                        PRINT_LOG(&logger, "[TEST_W] Seccion[%d] Motor[%d] Angulo[%d]\r\n", section_id, motor_id, action.degrees[motor_id]);
//                        bool done;
//                        do{
//                            done = tc->actionRequested((int16_t*)action.degrees);
//                            th->yield();
//                        }while(!done);         
//						action.degrees[motor_id] = 0;						
//                    }                        
//                }                
//                osEvent oe = th->signal_wait(0, osWaitForever);
//                if(IS_FLAG(SIG_ACTION_COMPLETE)){
//                    PRINT_LOG(&logger, "[TEST_W] Acciones completadas %d!!!!\r\n", tc->getActionCount()); 
//                    tc->clearActionCount();
//                }                  
//            }            
//            
//            /** TEST_G: Realiza movimiento de giro alrededor de una sección y vuelve a reposo
//                Test:  "G,Sección,Grados"
//                  ej:   "G,1,45" -> Realiza movimiento circular de 45º en la sección 1
//             */
//            if(strncmp(cmdbuf,"G", 1)==0){
//                int16_t action_mask[][3] = {{1,0,0},{0,1,0},{-1,0,0},{0,0,1},{0,-1,0},{1,0,0},{0,0,-1},{-1,0,0}};
//                char* token = strtok(cmdbuf,",");                
//                int section_id = atoi(strtok(0,","));                
//                int16_t degree = atoi(strtok(0,","));
//                tc_action action = {{0,0,0,0,0,0,0,0,0}};                
//                for(uint8_t i=0;i<8;i++){
//                    int motor_id = (TrunkController::SEGMENTS_PER_SECTION * section_id);
//                    action.degrees[motor_id] = action_mask[i][motor_id] * degree;                
//                    action.degrees[motor_id+1] = action_mask[i][motor_id+1] * degree;                
//                    action.degrees[motor_id+2] = action_mask[i][motor_id+2] * degree;                
//                    PRINT_LOG(&logger, "[TEST_G] Seccion[%d] Motor_id[%d]=(%d,%d,%d) Angulo[%d]\r\n", 
//                                    section_id, 
//                                    motor_id, 
//                                    action.degrees[motor_id], 
//                                    action.degrees[motor_id+1], 
//                                    action.degrees[motor_id+2], 
//                                    degree);
//                    bool done;
//                    do{
//                        done = tc->actionRequested((int16_t*)action.degrees);
//                        th->yield();
//                    }while(!done);          
//                                            
//                }                
//                osEvent oe = th->signal_wait(0, osWaitForever);
//                if(IS_FLAG(SIG_ACTION_COMPLETE)){
//                    PRINT_LOG(&logger, "[TEST_G] Acciones completadas %d!!!!\r\n", tc->getActionCount()); 
//                    tc->clearActionCount();
//                }                  
//            }                    
//            
//            /** TEST_J: Realiza movimiento de giro alrededor de varias secciones encadenadas y vuelve a reposo
//                Test:  "J,Sección,Grados"
//                  ej:   "J,1,45" -> Realiza movimiento circular de 45º en la sección 1 y 2
//                  ej:   "J,0,45" -> Realiza movimiento circular de 45º en la sección 0, 1 y 2
//             */
//            if(strncmp(cmdbuf,"J", 1)==0){
//                int16_t action_mask[][3] = {{1,0,0},{0,1,0},{-1,0,0},{0,0,1},{0,-1,0},{1,0,0},{0,0,-1},{-1,0,0}};
//                char* token = strtok(cmdbuf,",");                
//                int section_id = atoi(strtok(0,","));                
//                int16_t degree = atoi(strtok(0,","));
//                tc_action action = {{0,0,0,0,0,0,0,0,0}};                
//                for(uint8_t i=0;i<8;i++){
//                    for(int j=section_id; j<TrunkController::SECTION_COUNT; j++){
//                        int motor_id = (TrunkController::SEGMENTS_PER_SECTION * j);
//                        action.degrees[motor_id] = action_mask[i][motor_id] * degree;                
//                        action.degrees[motor_id+1] = action_mask[i][motor_id+1] * degree;                
//                        action.degrees[motor_id+2] = action_mask[i][motor_id+2] * degree;                
//                        PRINT_LOG(&logger, "[TEST_J] Seccion[%d] Motor_id[%d]=(%d,%d,%d) Angulo[%d]\r\n", 
//                                    section_id, 
//                                    motor_id, 
//                                    action.degrees[motor_id], 
//                                    action.degrees[motor_id+1], 
//                                    action.degrees[motor_id+2], 
//                                    degree);
//                        bool done;
//                        do{
//                            done = tc->actionRequested((int16_t*)action.degrees);
//                            th->yield();
//                        }while(!done);          
//                    }                        
//                }                
//                osEvent oe = th->signal_wait(0, osWaitForever);
//                if(IS_FLAG(SIG_ACTION_COMPLETE)){
//                    PRINT_LOG(&logger, "[TEST_J] Acciones completadas %d!!!!\r\n", tc->getActionCount()); 
//                    tc->clearActionCount();
//                }                  
//            }                    
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

