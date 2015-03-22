#include <adk.h>
#include <usbhub.h>
#include "TimerOne.h"

typedef enum {
    BASIC_IO = 1,
    COMMS = 2,
    EVENTS = 3
} Format;

USB Usb;
USBHub hub0(&Usb);
USBHub hub1(&Usb);
ADK adk(&Usb,"UPC, BarcelonaTech",
            "Android Beyond the Stratoshpere",
            "Android Beyond the Stratoshpere",
            "1.0",
            "http://www.upc.edu",
            "000000000000000001");

void setup()
{
	Serial.begin(115200);
	while (!Serial); 
	Serial.println("\r\nADK demo start");
        
        if (Usb.Init() == -1) {
          Serial.println("OSCOKIRQ failed to assert");
        while(1); //halt
        }//if (Usb.Init() == -1...


        Timer1.initialize(500000);        
        Timer1.pwm(9, 512);                
        Timer1.attachInterrupt(stateChange);
}

void loop()
{
  
  
  Usb.Task();
   
   /* if( adk.isReady() == false ) */
    
   int pin = 0;
   int par = 0;
   int res = 0;
   
   uint8_t rcode;
   uint8_t msg[9]; 
   uint16_t len;
   
   rcode = adk.RcvData(&len, msg);
   if(len > 0) {      
      byte cmd = (msg[0]>>5)&0x07; 
      switch(cmd){
      case BASIC_IO:
        /* Command type: BASIC_IO */
        par = (msg[0]>>3)&0x03;
        pin = (msg[1]>>1)&0xFF;
        switch(par){
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
        /* Command type: Serial comms */
        Serial.println("Comms"); 
        //TODO
        break;
      case EVENTS:
        /* Command type: Events */
        Serial.println("Events"); 
        //TODO
        break;
      default:
        Serial.println("Unknown");
    }
   }
   delay( 10 );       
}

void stateChange()
{
   digitalWrite(11, HIGH); 
}
