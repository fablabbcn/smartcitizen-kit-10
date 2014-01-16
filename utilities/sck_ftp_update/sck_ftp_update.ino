#include <Wire.h>

#include "SmartCitizenAmbient.h"


//#define mySSID           ""  
//#define myPassword       ""
#define wifiEncript      WPA2
#define antenna          INT_ANT // ANTENNA INTEGRADA

SmartCitizen sck;

void setup() {

  Serial.begin(9600);
  Serial1.begin(9600);
  sck.begin();

  pinMode(IO0, OUTPUT); //VH_MICS5525
  pinMode(IO1, OUTPUT); //VH_MICS2710
  pinMode(IO2, OUTPUT); //MICS2710_ALTAIMPEDANCIA
  pinMode(AWAKE, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT);
  digitalWrite(IO0, LOW); //VH_MICS5525
  digitalWrite(IO1, LOW); //VH_MICS2710
  digitalWrite(IO2, LOW); //RADJ_MICS2710
  digitalWrite(AWAKE, HIGH); 
  
  sck.config(mySSID, myPassword, wifiEncript, antenna); 
  
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













