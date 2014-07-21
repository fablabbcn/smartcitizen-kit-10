#include <EEPROM.h>
#include <Wire.h>
#include "SCKlibs.h"

sck sck;
void setup() { 
  sck.begin();
  delay(5000);
  Serial.println("Trying to connect.");
  if (!sck.repair()) sck.recovery();
  else Serial.println("Device detected.");
  Serial.print("Firmware version: "); 
  char *Version = sck.getWiFlyVersion(1000);
  Serial.println(Version);
  int state = sck.checkWiFlyVersion(Version);
  if (state==1) Serial.println("Wifi device is ok :)");
  else if (state==-1) Serial.println("Wifi device is corrupted :(");
  else if ((state)==0) if (!sck.webAppRepair())Serial.println("Wifi device is corrupted :(");
  Serial.println(F("Want to test the EEPROM? (y/n)"));
  while (!Serial.available());
  byte inByte = Serial.read();
  if (inByte =='y')
    {
      if (sck.checkEEPROM()) Serial.println("EEPROM is OK");
      else  Serial.println("EEPROM fail :(");
    }
  else if (inByte !='n') Serial.println("Request invalid.");
  if (sck.checkRTC()) Serial.println("RTC is OK");
  else Serial.println("RTC fail, RTC battery check");
  Serial.print("Panel: "); Serial.print(sck.getPanel()); Serial.println("mV");
  Serial.print("Battery: "); Serial.print(sck.getBattery()); Serial.println("%");
  Serial.print("Speed charge: "); Serial.print(sck.readCharge()); Serial.println("mA");
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
