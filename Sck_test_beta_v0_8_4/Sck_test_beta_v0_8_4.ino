#include "I2c.h"
#include "Constants.h"

#define USBEnabled      true 
#define wiflyEnabled    true
#define wiflySleep      true
#define sensorEnabled   true
#define debuggEnabled   true
#define MICSEnabled     true
#define SDEnabled       false

boolean wait        = false;
boolean sleep       = true; 
boolean iphone_mode = false;
byte server_mode    = 0;

uint32_t timetransmit = 0;
uint32_t TimeUpdate   = 10;

#if SDEnabled
  #include <SD.h>
  File myFile;
  long SENSORvalue[8];
#endif

void setup() {

  sckBegin();
  
#if wiflySleep
  sckSleep();
#endif

#if SDEnabled
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
  if (!SD.exists("post.txt")) {
    #if debuggEnabled
        Serial.println(F("Creating post.csv..."));
    #endif 
    myFile = SD.open("post.txt", FILE_WRITE);
    myFile.close();
  }
  
  //sckRTCadjust("2013-10-24 08:00:00");  // Ajuste RTC manualmente!
    
#endif   

  /*init WiFly*/
#if wiflyEnabled
  digitalWrite(AWAKE, HIGH); 
  server_mode = 1;  //Modo normal
  sckConfig(); 
  TimeUpdate = atol(sckReadData(EE_ADDR_TIME_UPDATE, 0, 0)); //Tiempo entre transmision y transmision en segundos, minimo 60 segundos
  if (TimeUpdate < 60) sleep = false;
  else sleep = true; 
  if (!sckConnect())
  {
    sleep = false;  
    server_mode = 0; //Modo AP
    sckAPmode(sckid());
    #if debuggEnabled
        Serial.println(F("AP initialized!"));
    #endif 
  }
  else
  {
    #if debuggEnabled
        Serial.println(F("SCK Connected!!"));
    #endif
    byte retry = 0;
    if (sckCheckRTC())
     {
      while (!sckRTCadjust(sckWIFItime())&&(retry<5))
       {
         retry = retry + 1;
       }
     }
  }  
    
  if(sleep)
    {
      sckSleep();
      #if debuggEnabled
          Serial.println(F("SCK Sleeping...")); 
      #endif
      digitalWrite(AWAKE, LOW); 
    }
  timer1Initialize(); // set a timer of length 1000000 microseconds (or 1 sec - or 1Hz)
  timer1AttachInterrupt();
  #if !MICSEnabled
    server_mode = 3; 
  #endif  
#else 
  #if !MICSEnabled
    server_mode = 4; 
  #else 
    server_mode = 2; 
  #endif  
#endif  
}

void loop() {     
#if sensorEnabled  
    #if wiflyEnabled
      if (iphone_mode) // Telnet  (#data + *OPEN* detectado )
      {
        timer1Stop();
        sleep = false;
        digitalWrite(AWAKE, HIGH);
        sckJson_update(0);
        iphone_mode = false;
        timer1Initialize(); // set a timer of length 1000000 microseconds (or 1 sec - or 1Hz)
      }
    #endif
    if ((millis()-timetransmit) >= (unsigned long)TimeUpdate*1000)
    {  
      timetransmit = millis();
      #if wiflyEnabled
        sckUpdateSensors(server_mode); 
        if (!wait) // command mode false
        {
          #if wiflyEnabled
            timer1Stop();
            if (server_mode) txWiFly();
          #endif
          #if USBEnabled
            txDebug();
          #endif
          #if wiflyEnabled
            timer1Initialize(); // set a timer of length 1000000 microseconds (or 1 sec - or 1Hz)
          #endif
        }
      #else
        #if SDEnabled
          updateSensorsSD();
          txSD();
          #if USBEnabled
            txDebugSD();
          #endif
        #endif
        #if (USBEnabled && !SDEnabled)
          sckUpdateSensors(server_mode); 
          txDebug();
        #endif
      #endif
    }
#endif
}















