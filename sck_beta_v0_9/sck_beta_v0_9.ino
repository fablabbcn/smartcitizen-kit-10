#include <Wire.h>
#include <EEPROM.h>
#include "SCKAmbient.h"
#include "SCKServer.h"
#include "SCKBase.h"

SCKAmbient ambient;
  
void setup() {
  ambient.begin();
  ambient.ini();
}

void loop() {  
  ambient.execute();
}
















