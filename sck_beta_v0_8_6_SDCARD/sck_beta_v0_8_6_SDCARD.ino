#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>
#include "Constants.h"

#define USBEnabled      true 
#define sensorEnabled   true
#define debuggEnabled   false
#define ADXLEnabled     false

uint32_t timetransmit = 0;  
uint32_t TimeUpdate   = 0;  //Variable temporal de tiempo entre actualizacion y actualizacion de los sensensores
uint32_t NumUpdates   = 0;  //Numero de actualizaciones antes de postear

File myFile;
long SENSORvalue[8];


void setup() {
  sckBegin();
  sckConfig();
  sckSleep();
  #if debuggEnabled
    Serial.print(F("Initializing SD card..."));
  #endif 
    if (!SD.begin(11)) {
  #if debuggEnabled
      Serial.println(F("initialization failed!"));
  #endif 
      return;
    }
  #if debuggEnabled
    Serial.println(F("initialization done."));   
  #endif 
    if (!SD.exists("post.csv")) {
  #if debuggEnabled
      Serial.println(F("Creating post.csv..."));
  #endif 
      myFile = SD.open("post.csv", FILE_WRITE);
      myFile.close();
    }  
}
void loop() {  
#if sensorEnabled  
  if ((millis()-timetransmit) >= (unsigned long)TimeUpdate*1000)
  {  
    timetransmit = millis();
    TimeUpdate = atol(sckReadData(EE_ADDR_TIME_UPDATE, 0, 0)); //Tiempo entre transmision y transmision en segundos
    updateSensorsSD();
    txSD();
    #if USBEnabled
        txDebugSD();
    #endif
  }
#endif
}
















