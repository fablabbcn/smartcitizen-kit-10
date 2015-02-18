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
  
  delay(10000);
  
  sckBegin();
  sckConfig();
  sckSleep();
  #if debuggEnabled
    Serial.print(F("Initializing SD card..."));
  #endif 
    if (!sd.begin(11)) {
  #if debuggEnabled
      Serial.println(F("initialization failed!"));
  #endif 
      return;
    }
  #if debuggEnabled
    Serial.println(F("initialization done."));   
  #endif 
    if (!sd.exists("post.csv")) {
      #if debuggEnabled
        Serial.println(F("Creating post.csv..."));
      #endif 
      myFile.open("post.csv", FILE_WRITE);
      myFile.close();
      delay(1000);
      txHeader();
      
    } else{
      #if debuggEnabled
        Serial.println(F("post.csv exists ..."));
      #endif 
    }
    
    timetransmit = millis();
    TimeUpdate = atol(sckReadData(EE_ADDR_TIME_UPDATE, 0, 0)); //Tiempo entre transmision y transmision en segundos
}

void loop() {  
#if sensorEnabled  
  if ((millis()-timetransmit) >= (unsigned long)TimeUpdate*1000)
  {  
    Serial.println("*** loop ***");
    timetransmit = millis();
    TimeUpdate = atol(sckReadData(EE_ADDR_TIME_UPDATE, 0, 0)); 
    updateSensorsSD();
    txSD();
    #if USBEnabled
        txDebugSD();
    #endif
  }
#endif
}
















