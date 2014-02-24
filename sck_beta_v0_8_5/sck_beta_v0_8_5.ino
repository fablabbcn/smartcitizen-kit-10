#include <Wire.h>
#include "Constants.h"

#define USBEnabled      false 
#define wiflyEnabled    true
#define wiflySleep      true
#define sensorEnabled   true
#define debuggEnabled   false
#define MICSEnabled     true
#define SDEnabled       false

boolean wait        = false;
boolean sleep       = true; 
boolean terminal_mode = false;
boolean usb_mode      = false;
boolean serial_bridge = false;
byte server_mode    = 0;
uint16_t  nets      = 0;

uint32_t timetransmit = 0;  
uint32_t TimeUpdate   = 0;  //Variable temporal de tiempo entre actualizacion y actualizacion de los sensensores
uint32_t NumUpdates   = 0;  //Numero de actualizaciones antes de postear

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
  if (!SD.exists("post.csv")) {
    #if debuggEnabled
        Serial.println(F("Creating post.csv..."));
    #endif 
    myFile = SD.open("post.csv", FILE_WRITE);
    myFile.close();
  } 
    
#endif   

  /*init WiFly*/
#if wiflyEnabled
  digitalWrite(AWAKE, HIGH); 
  server_mode = 1;  //Modo normal
  sckConfig(); 
  TimeUpdate = atol(sckReadData(EE_ADDR_TIME_UPDATE, 0, 0)); //Tiempo entre transmision y transmision en segundos
  NumUpdates = atol(sckReadData(EE_ADDR_NUMBER_UPDATES, 0, 0)); //Numero de actualizaciones antes de postear a la web
  nets = sckReadintEEPROM(EE_ADDR_NUMBER_NETS);
  
  if (TimeUpdate < 60) sleep = false;
  else sleep = true; 
  
  
  if (nets==0)
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
    if (sckConnect())
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
          #if debuggEnabled
            Serial.println(F("Updating RTC..."));
          #endif
       }
    }
  }  
    
  if((sleep)&&(wiflySleep))
    {
      sckSleep();
      #if debuggEnabled
          Serial.println(F("SCK Sleeping...")); 
      #endif
      digitalWrite(AWAKE, LOW); 
    }
  timer1Initialize();
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
      if (terminal_mode) // Telnet  (#data + *OPEN* detectado )
      {
        timer1Stop();
        sleep = false;
        digitalWrite(AWAKE, HIGH);
        sckJson_update(0, usb_mode);
        usb_mode = false;
        terminal_mode = false;
        timer1Initialize(); // set a timer of length 1000000 microseconds (or 1 sec - or 1Hz)
      }
    #endif
    if ((millis()-timetransmit) >= (unsigned long)TimeUpdate*1000)
    {  
      timetransmit = millis();
      TimeUpdate = atol(sckReadData(EE_ADDR_TIME_UPDATE, 0, 0)); //Tiempo entre transmision y transmision en segundos
      #if wiflyEnabled
        NumUpdates = atol(sckReadData(EE_ADDR_NUMBER_UPDATES, 0, 0)); //Numero de actualizaciones antes de postear a la web
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















