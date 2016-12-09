#include "mbed.h"
#include "SerialMon.h"
#include "MsgBroker.h"

#include "TCPSocket.h"
#include "ESP8266Interface.h"
ESP8266Interface wifi(PA_2, PA_3);
#define STR(x) STR2(x)
#define STR2(x) #x
int test_esp8266();
 
SerialMon *logger;

volatile int l1=0,l2=0;
 

/************************************************************************************/
void led2_thread() {
    static char txt[128];
    int count = 0;
    MsgBroker::Exception e;
    SerialMon::topic_t topic;
    while (true) {
        count++;
        sprintf(txt, "Task2 = %d\r\n", count);
        topic.txt = txt;
        MsgBroker::publish("/log", &topic, sizeof(SerialMon::topic_t), &e);    
//      logger->send(txt);
        Thread::wait(150);
    }
}


/************************************************************************************/
int main() {
    test_esp8266();
    
    /** Configura la infraestructura de publicacion/subscripción */
    MsgBroker::init();
    
    /** Configura el módulo de depuración */    
    logger = new SerialMon(PA_9, PA_10, 512, 512, 115200, "Logger_UART2");
    
    /** Envía traza */
    MsgBroker::Exception e;
    SerialMon::topic_t topic;
    topic.txt = (char*)"Iniciando...\r\n";
    MsgBroker::publish("/log", &topic, sizeof(SerialMon::topic_t), &e);
//  logger->send((char*)"Iniciando...\r\n");
    static char txt[128];
    int count = 0;
    Thread tled;
    tled.start(led2_thread);
    while (true) {  
        count++;        
        std::sprintf(txt, "Task1 = %d\r\n", count);
        topic.txt = txt;
        MsgBroker::publish("/log", &topic, sizeof(SerialMon::topic_t), &e);
//      logger->send(txt);
        Thread::wait(100);
    }
}





/************************************************************************************/
// Socket demo
void http_demo(NetworkInterface *net) {
    TCPSocket socket;

    // Show the network address
    const char *ip = net->get_ip_address();
    printf("IP address is: %s\n", ip ? ip : "No IP");

    // Open a socket on the network interface, and create a TCP connection to mbed.org
    socket.open(net);
    socket.connect("developer.mbed.org", 80);

    // Send a simple http request
    char sbuffer[] = "GET / HTTP/1.1\r\nHost: developer.mbed.org\r\n\r\n";
    int scount = socket.send(sbuffer, sizeof sbuffer);
    printf("sent %d [%.*s]\r\n", scount, strstr(sbuffer, "\r\n")-sbuffer, sbuffer);

    // Recieve a simple http response and print out the response line
    char rbuffer[64];
    int rcount = socket.recv(rbuffer, sizeof rbuffer);
    printf("recv %d [%.*s]\r\n", rcount, strstr(rbuffer, "\r\n")-rbuffer, rbuffer);

    // Close the socket to return its memory and bring down the network interface
    socket.close();
}


/************************************************************************************/
// Example with the ESP8266 interface
int test_esp8266() {
    // Brings up the esp8266
    printf("ESP8266 socket example\n");
    wifi.connect(STR(MBED_DEMO_WIFI_SSID), STR(MBED_DEMO_WIFI_PASS));

    // Invoke the demo
    http_demo(&wifi);

    // Brings down the esp8266 
    wifi.disconnect();

    printf("Done\n");
    return 0;
}

