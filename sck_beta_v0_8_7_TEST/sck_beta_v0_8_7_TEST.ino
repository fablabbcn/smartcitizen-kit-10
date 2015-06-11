#include <Wire.h>
#include <SPI.h>
#include <SdFat.h>
#include <EEPROM.h>
#include "Constants.h"

#define USBEnabled      true
#define sensorEnabled   true
#define debuggEnabled   true
#define ADXLEnabled     false

uint32_t timetransmit = 0;
uint32_t TimeUpdate   = 0;  //Variable temporal de tiempo entre actualizacion y actualizacion de los sensensores
uint32_t NumUpdates   = 0;  //Numero de actualizaciones antes de postear

SdFat sd;
SdFile myFile;
long SENSORvalue[8];
boolean csvInit = false;

void setup() {
  sckBegin();
  sckConfig();
}

void loop() {
  updateSensorsSD();
  txDebugSD();
}
















