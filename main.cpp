#include "mbed.h"
#include "SerialMon.h"
#include "MsgBroker.h"
#include "CloudManager.h"
#include "TrunkController.h"

#include "TCPSocket.h"
#include "ESP8266Interface.h"
#define STR(x) STR2(x)
#define STR2(x) #x
int test_esp8266();
 
SerialMon *logger;
CloudManager *cloudm;
TrunkController *tc;

volatile int l1=0,l2=0;
 

/************************************************************************************/
void led2_thread() {
//    static char txt[128];
//    int count = 0;
//    MsgBroker::Exception e;
//    SerialMon::topic_t topic;
//	CloudManager::topic_iot_t iot;
//    while (true) {
//        count++;
//        sprintf(txt, "Task2 = %d\r\n", count);
//        topic.data = (uint8_t*)txt;
//		topic.size = strlen(txt)+1;
//        MsgBroker::publish("/log", &topic, sizeof(SerialMon::topic_t), &e);  
//		if(count == 5){
//			iot.cmd = CloudManager::CMD_IOT_CONN;
//			const char *wifiargs = "MOVISTAR_9BCC,hh9DNmVvV3Km6ZzdKrkx,4";
//			iot.data = (void*)wifiargs;
//			iot.size = strlen(wifiargs);
//			MsgBroker::publish("/iot", &iot, sizeof(CloudManager::topic_iot_t), &e);
//			
//		}
//		if(count == 50){
//			iot.cmd = CloudManager::CMD_IOT_HTTPGET;
//			const char *httpget = "GET / HTTP/1.1\r\nHost: developer.mbed.org\r\n\r\n";
//			iot.data = (void*)httpget;
//			iot.size = strlen(httpget);
//			MsgBroker::publish("/iot", &iot, sizeof(CloudManager::topic_iot_t), &e);
//			
//		}
//		if(count == 100){
//			iot.cmd = CloudManager::CMD_IOT_DISC;
//			iot.data = 0;
//			iot.size = 0;
//			MsgBroker::publish("/iot", &iot, sizeof(CloudManager::topic_iot_t), &e);
//			
//		}
////      logger->send(txt);
//        Thread::wait(150);
//    }
}


/************************************************************************************/
int main() {
//    test_esp8266();
    
    /** Configura la infraestructura de publicacion/subscripción */
    MsgBroker::init();
    
    /** Configura el módulo de depuración */    
    logger = new SerialMon(PA_2, PA_3, 512, 512, 115200, "Logger_UART2");
	
//	/** Configutra el módulo de comunicaciones iot */
//	cloudm = new CloudManager(PA_9, PA_10);
//	cloudm->setLogger(logger);
	
	/** Configura el robot */
	tc = new TrunkController(PA_0, PA_1, PA_2, PA_3, PA_4, Stepper::FULL_STEP);
    
    /** Envía traza */
    MsgBroker::Exception e;
    SerialMon::topic_t topic;
    topic.data = (uint8_t*)"Iniciando...\r\n";
	topic.size = strlen((char*)topic.data)+1;
    MsgBroker::publish("/log", &topic, sizeof(SerialMon::topic_t), &e);
//  logger->send((char*)"Iniciando...\r\n");
    static char txt[128];
    int count = 0;
    Thread tled;
    tled.start(led2_thread);
    while (true) {  
        count++;        
        std::sprintf(txt, "Task1 = %d\r\n", count);
        topic.data = (uint8_t*)txt;
		topic.size = strlen(txt)+1;
        MsgBroker::publish("/log", &topic, sizeof(SerialMon::topic_t), &e);
//      logger->send(txt);
        Thread::wait(100);
    }
}





///************************************************************************************/
//// Socket demo
//char logg[128];
//void http_demo(NetworkInterface *net) {
//    TCPSocket socket;

//    // Show the network address
//    const char *ip = net->get_ip_address();
//    //printf("IP address is: %s\n", ip ? ip : "No IP");
//	sprintf(logg,"IP address is: %s\n", ip ? ip : "No IP");

//    // Open a socket on the network interface, and create a TCP connection to mbed.org
//    socket.open(net);
//    socket.connect("192.168.1.38",8888);//"developer.mbed.org", 80);

//    // Send a simple http request
//    char sbuffer[] = "GET / HTTP/1.1\r\nHost: developer.mbed.org\r\n\r\n";
//    int scount = socket.send(sbuffer, sizeof sbuffer);
//    //printf("sent %d [%.*s]\r\n", scount, strstr(sbuffer, "\r\n")-sbuffer, sbuffer);
//	sprintf(logg,"sent %d [%.*s]\r\n", scount, strstr(sbuffer, "\r\n")-sbuffer, sbuffer);

//    // Recieve a simple http response and print out the response line
//    char rbuffer[64];
//    int rcount = socket.recv(rbuffer, sizeof rbuffer);
//    //printf("recv %d [%.*s]\r\n", rcount, strstr(rbuffer, "\r\n")-rbuffer, rbuffer);
//	sprintf(logg,"recv %d [%.*s]\r\n", rcount, strstr(rbuffer, "\r\n")-rbuffer, rbuffer);

//    // Close the socket to return its memory and bring down the network interface
//    socket.close();
//}


///************************************************************************************/
//// Example with the ESP8266 interface
//int test_esp8266() {
//	ESP8266Interface wifi(PA_9, PA_10);
//    // Brings up the esp8266
////    printf("ESP8266 socket example\n");
//    int err = wifi.connect("MOVISTAR_9BCC","hh9DNmVvV3Km6ZzdKrkx", NSAPI_SECURITY_WPA_WPA2);

//	if(!err){
//		// Invoke the demo
//		http_demo(&wifi);

//		// Brings down the esp8266 
//		wifi.disconnect();
//	}

////    printf("Done\n");
//	while(1){}
//    return 0;
//}

