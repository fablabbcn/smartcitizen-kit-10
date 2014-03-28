#include <Wire.h>
#include "Constants.h"

#define USBEnabled      true 
#define wiflyEnabled    true
#define wiflySleep      true
#define sensorEnabled   true
#define debuggEnabled   true
#define MICSEnabled     true
#define autoUpdateWiFly true
#define ADXLEnabled     false

boolean wait        = false;
boolean sleep       = true; 
boolean terminal_mode = false;
boolean wait_moment = true;
boolean usb_mode      = false;
boolean serial_bridge = false;
byte server_mode    = 0;
uint16_t  nets      = 0;

uint32_t timetransmit = 0;  
uint32_t TimeUpdate   = 0;  //Variable temporal de tiempo entre actualizacion y actualizacion de los sensensores
uint32_t NumUpdates   = 0;  //Numero de actualizaciones antes de postear

void setup() {

  sckBegin();

#if wiflySleep
  sckSleep();
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
//  delay(5000);
  if (nets==0)
  {
    sleep = false;  
    server_mode = 0; //Modo AP
    sckRepair();

    sckAPmode(sckid());
#if debuggEnabled
    if (!wait) Serial.println(F("AP initialized!"));
#endif 
  }
  else
  {

    if (sckConnect())
    {
      
#if debuggEnabled
      if (!wait) Serial.println(F("SCK Connected!!"));
#endif
#if autoUpdateWiFly
      sckCheckWiFly();
#endif
      byte retry = 0;
      if (sckCheckRTC())
      {
        while (!sckRTCadjust(sckWIFItime())&&(retry<5))
        {
          retry = retry + 1;
        }
#if debuggEnabled
        if (!wait) Serial.println(F("Updating RTC..."));
#endif
      }
    }
  }  



  if((sleep)&&(wiflySleep))
  {
    sckSleep();
#if debuggEnabled
    if (!wait) Serial.println(F("SCK Sleeping...")); 
#endif
    digitalWrite(AWAKE, LOW); 
  }
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
wait_moment = false;
}

void loop() {  
#if sensorEnabled  
  #if wiflyEnabled
    if (terminal_mode) // Telnet  (#data + *OPEN* detectado )
    {
      sleep = false;
      digitalWrite(AWAKE, HIGH);
      sckJson_update(0, usb_mode);
      usb_mode = false;
      terminal_mode = false;
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
        if (server_mode) txWiFly();
  #endif
  #if USBEnabled
        txDebug();
  #endif
      }
  #else
    #if (USBEnabled)
        sckUpdateSensors(server_mode); 
        txDebug();
    #endif
  #endif
    }
#endif
}
















