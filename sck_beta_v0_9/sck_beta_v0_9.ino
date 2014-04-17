#include <Wire.h>
#include <EEPROM.h>
#include "SCKAmbient.h"
#include "SCKServer.h"
#include "SCKBase.h"

SCKAmbient ambient;
//SCKBase base;
//SCKServer server;
//
//  long value_[9] = {1,2,3,4,5,6,7,8,9};
//  char time_[TIME_BUFFER_SIZE] = "holaa";
  
void setup() {
// base.config(); 
  ambient.begin();
  ambient.ini();
}

void loop() {  
//  delay(1000);
//  server.addFIFO(value_, time_);
//  server.addFIFO(value_, time_);
//  server.readFIFO();
  ambient.execute();
}
















