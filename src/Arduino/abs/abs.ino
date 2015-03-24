#include <adk.h>
#include <usbhub.h>
#include "TimerOne.h"
#include <SoftwareSerial.h>

typedef enum {
    BASIC_IO = 1,
    COMMS = 2,
    EVENTS = 3
} Format;

const int bitrate[] = {300, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600,  115200};

SoftwareSerial mySerial[2] = {SoftwareSerial(6,7),
                              SoftwareSerial(7,8)};

USB Usb;
USBHub hub0(&Usb);
USBHub hub1(&Usb);

ADK adk(&Usb,"Google, Inc.",
            "DemoKit",
            "DemoKit Arduino Board",
            "1.0",
            "http://www.android.com",
            "0000000012345678");
            
       

void setup()
{
	Serial.begin(115200);
	while (!Serial); 
	Serial.println("\r\nADK demo start");
        
        if (Usb.Init() == -1) {
          Serial.println("OSCOKIRQ failed to assert");
        while(1); //halt
        }//if (Usb.Init() == -1...
        

}

void loop()
{
  Usb.Task();
   
   /* if( adk.isReady() == false ) */
    
   int pin = 0;
   int res = 0;
   
   int i = 0;
   
   uint8_t rcode;
   uint8_t msg[9]; 
   uint16_t len=0;
    
   rcode = adk.RcvData(&len, msg);
   if(len > 0) {      
      byte cmd = (msg[0]>>5)&0x07; 
      Serial.println("hello\n");
      switch(cmd){
      case BASIC_IO:
        Serial.println("hello\n");
        /* Command type: BASIC_IO */
        cmd = (msg[0]>>3)&0x03;
        pin = (msg[1]>>1)&0xFF;
        switch(cmd){
          case 0:
            /* Analog Write */
            analogWrite(pin, (msg[0]<<5)&0xC0);
            break;
          case 1:
            /* Digital Write */
            digitalWrite(pin, (msg[0]>>3)&0x01);
            break;
          case 2:
            /* Analog Read */
            res = analogRead(pin);
            break;
          case 3:
            /* Digital Read */
            res = digitalRead(pin);
            break;     
        }
        break;
      case COMMS:
        /* Command type: Serial Comms */
        Serial.println("Comms"); 
        switch(cmd){
          case 0:
              mySerial[i].begin(bitrate[(msg[0]<<1)&0x07]);
              break;
           case 1:
               if(1 <= i)
                 mySerial[1].read();
             break;
           case 2:
               if(1 <= i)
                 mySerial[1].println(msg[2]);
             break;
        }
        break;
      case EVENTS:
        /* Command type: Events */
        Serial.println("Events"); 
        //TODO
        break;
      default:
        Serial.println("Unknown");
    }
    /* Send response packet */
   }
   delay( 10 );       
}

void stateChange()
{
   
}
