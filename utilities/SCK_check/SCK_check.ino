#include <EEPROM.h>

void setup() { 
  sckBegin();
  delay(5000);
  Serial.println("Trying to connect.");
  if (!sckRepair()) sckRecovery();
  else Serial.println("Device detected.");
  Serial.print("Firmware version: "); 
  char *Version = getWiFlyVersion(1000);
  Serial.println(Version);
  int state = checkWiFlyVersion(Version);
  if (state==1) Serial.print("Wifi device is ok :)");
  else if (state==-1) Serial.print("Wifi device is corrupted :(");
  else if ((state)==0) 
    {
      //Serial.print("Wifi device is corrupted :(");
    }
} 

void loop() {   
  if (Serial.available()) 
  {
    int inByte = Serial.read();
    Serial1.write(inByte); 
  }
  if (Serial1.available()) {
    int inByte = Serial1.read();
    Serial.write(inByte); 
  }
} 
