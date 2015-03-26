#include <adk.h>
#include <usbhub.h>
#include "TimerOne.h"
#include <SoftwareSerial.h>

#define MAX_SERIAL 2
#define MAX_EVENTS 10

typedef enum {
    BASIC_IO = 1,
    COMMS = 2,
    EVENT = 3
} Format;

typedef enum {
    ANALOG_WRITE,
    DIGITAL_WRITE,
    ANALOG_READ,
    DIGITAL_READ 
} BasicIOAction;

typedef enum {
    INIT,
    READ,
    WRITE
} SerialComm;

struct Event {
    int action;
    int buffer;
    int time;
};

uint8_t rc;
byte cmd, par;
int pin = 0, num = 0, res = 0, i = 0, time = 0, eventCount = 0; 
uint8_t msg[100] = { 0x00 };
Event event_list[MAX_EVENTS];

const int bitrate[] = {300, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600,  115200};

SoftwareSerial mySerial[MAX_SERIAL] = {SoftwareSerial(6,7), SoftwareSerial(7,8)};

USB Usb;
USBHub hub0(&Usb);
USBHub hub1(&Usb);

ADK adk(&Usb, "UPC, BarcelonaTech",
              "Android Beyond the Stratoshpere",
              "Android Beyond the Stratoshpere",
              "1.0",
              "http://www.upc.edu",
              "000000000000000001");
            

void setup(void)
{
	Serial.begin(115200);
	//while (!Serial); 
	Serial.println("\r\nADK demo start");
        
    if(Usb.Init() == -1) {
        Serial.println("OSCOKIRQ failed to assert");
        while(Usb.Init()==-1); /* retry */
    }
        Timer1.initialize(500000);        
        Timer1.pwm(9, 512);                
        Timer1.attachInterrupt(events);
}

void loop(void)
{  
    Usb.Task(); 
    if(!adk.isReady()) {
        return;
    }
    uint16_t length = sizeof(msg);
    rc = adk.RcvData(&length, msg);
    if(length > 0) {
        noInterrupts();
        cmd = (msg[0] >> 5) & 0x07;
        switch(cmd) {
            case BASIC_IO:
                /* Command type: BASIC_IO */
                par = (msg[0] >> 3) & 0x03;
                pin = (msg[1] >> 1) & 0xFF;
                switch(par) {    
                    case ANALOG_WRITE:
                        /* Analog Write */
                        pinMode(pin, OUTPUT);
                        analogWrite(pin, (msg[0] << 5) & 0xC0);
                        break;
                    case DIGITAL_WRITE:
                        /* Digital Write */
                        pinMode(pin, OUTPUT); 
                        digitalWrite(pin, (msg[0] >> 2) & 0x01);
                        break;
                    case ANALOG_READ:
                        /* Analog Read */
                        res = analogRead(pin);
                        break;
                    case DIGITAL_READ:
                        /* Digital Read */
                        pinMode(pin, INPUT);
                        res = digitalRead(pin);
                        break;
                }  
                break;    
            case COMMS:
                /* Command type: Serial Comms */
                par = (msg[0] >> 3) & 0x03; 
                num = (msg[1] << 1) & 0xFF;
                switch(par) {     
                    case INIT:
                        /* Serial setup */
                        if(num <= MAX_SERIAL) {
                            mySerial[num].begin(bitrate[(msg[0] << 1) & 0x07]);
                        }
                        break;
                    case READ:
                        /* Serial read */
                        if(num <= MAX_SERIAL){
                            mySerial[num].read();
                        }
                        break;
                    case WRITE:
                        /* Serial write */
                        if(num <= MAX_SERIAL){
                            mySerial[num].println(msg[2]);
                        }
                        break;
                }
                break;
            case EVENT:
                /* Command type: Events */
                Serial.println("Events"); 
                //TODO
                event_list[eventCount].action = 1; 
                event_list[eventCount].buffer = 1;
                event_list[eventCount].time = (int)(msg[0] << 1) & 0x0F; 
                eventCount++;
                //TODO
                break;
            default:
                Serial.println("Unknown");
                break;
        }
        /* Send response packet */
        interrupts();
    }      
    delay(100);       
}

void events(void)
{  
   int i = 0;
   for(i=0; i < eventCount; i++) {
     if(time%event_list[i].time == 0) {
       Serial.println("Event num: "+String(i));
       //TODO
     }
   }
   time++;
}
