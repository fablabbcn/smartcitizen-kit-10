/*

  Smart Citizen Kit
  Ambient Board Beta Firmware v.0.8.6

  http://smartcitizen.me/


  Compatible:

    Smart Citizen Kit v.1.0 (Goteo)       (ATMEGA32U4 @ 16Mhz - Arduino Leonardo profile)
    Smart Citizen Kit v.1.1 (Kickstarter) (ATMEGA32U4 @ 8Mhz  - Lylipad Arduino USB)

  Structure:
    
    sck_beta_v0_8_6.ino - Core Runtime.

    SCKAmbient.ino    - Supports the sensor reading and calibration functions.
    SCKBase.ino       - Supports the data management functions (WiFi, SD storage, RTClock and EEPROM storage)
    ServerUpdate.ino  - Supports data publishing to the SmartCitizen Platform over WiFi. Supports also SD storage.

    Constants.h             - Defines pins configuration and other static parameters.
    AccumulatorFilter.h     - Used for battery temperature decoupling in  Smart Citizen Kit v.1.0 
    TemperatureDecoupler.h  - Used for battery temperature decoupling in  Smart Citizen Kit v.1.0 

  Check REAMDE.md for more information.
    
*/

#include <Wire.h>

/* 

GLOBAL FIRMWARE CONFIGURATION FLAGS

*/

#define USBEnabled      true 
#define wiflyEnabled    true
#define wiflySleep      true
#define sensorEnabled   true
#define debuggEnabled   false
#define MICSEnabled     true
#define autoUpdateWiFly true
#define ADXLEnabled     false
#define DataRaw         true

#include "Constants.h"


/*  

GLOBAL toggles and counters

*/

boolean wait        = false;
boolean sleep       = true; 
boolean terminal_mode = false;
boolean wait_moment = true;
boolean usb_mode      = false;
boolean serial_bridge = false;
byte server_mode    = 0;
uint16_t  nets      = 0;

uint32_t timetransmit = 0;  
uint32_t TimeUpdate   = 0;  // Sensor Readings time interval in sec.
uint32_t NumUpdates   = 0;  // Min. number of sensor readings before publishing

/* 

GLOBAL SETUP

*/

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
  TimeUpdate = atol(sckReadData(EE_ADDR_TIME_UPDATE, 0, 0));    // Time between transmissions in sec.
  NumUpdates = atol(sckReadData(EE_ADDR_NUMBER_UPDATES, 0, 0)); // Number of readings before batch update
  nets = sckReadintEEPROM(EE_ADDR_NUMBER_NETS);
  if (TimeUpdate < 60) sleep = false;
  else sleep = true; 
//  delay(5000);
  if (nets==0)
  {
    sleep = false;  
    server_mode = 0; //AP mode
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
      TimeUpdate = atol(sckReadData(EE_ADDR_TIME_UPDATE, 0, 0));    // Time between transmissions in sec.
  #if wiflyEnabled
      NumUpdates = atol(sckReadData(EE_ADDR_NUMBER_UPDATES, 0, 0)); // Number of readings before batch update
      sckUpdateSensors(server_mode); 
      if (!wait)                                                    // CMD Mode False
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
















