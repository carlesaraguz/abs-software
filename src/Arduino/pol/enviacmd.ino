#include <Wire.h>
  
byte slaveaddress = 0x00;
byte command = 0x0A;
volatile uint16_t rcvdData;
  
void setup() {
  
  Wire.begin();

  Serial.begin(9600);
}

void loop() {
  
  //Wait for some keyboard key pressed to start the routine
  while(Serial.available() == 0);
  Serial.read();
  Serial.println("Starting transmission with slave address...");
  
  //Start the I2C transmision
  Wire.beginTransmission(slaveaddress);
  Serial.println("Slave Address sent... sending the command...");
  Wire.write(command);
  Wire.endTransmission();

  delay(0.7);

  //Slave's answer
  Wire.requestFrom(slaveaddress, (uint8_t)2);
  Serial.println("I've reached the requestFrom");

  while(Wire.available() < 2);
  
  for(int i=0; i<2; i++) 
    rcvdData = Wire.read()<<8*i;
    
  Serial.println("I've recived some data");
  Serial.println((int)rcvdData);
}
