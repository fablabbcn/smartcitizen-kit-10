/*

  Smart Citizen Kit
  Ambient Board Beta Firmware v.0.9

  http://smartcitizen.me/


  Compatible:

    Smart Citizen Kit v.1.0 (Goteo)       (ATMEGA32U4 @ 16Mhz - Arduino Leonardo profile)
    Smart Citizen Kit v.1.1 (Kickstarter) (ATMEGA32U4 @ 8Mhz  - Lylipad Arduino USB)

  Structure:
    
    sck_beta_v0_9.ino - Core Runtime.

    SCKAmbient.h    - Supports the sensor reading and calibration functions.
    SCKBase.h       - Supports the data management functions (WiFi,  RTClock and EEPROM storage)
    SCKServer.h     - Supports data publishing to the SmartCitizen Platform over WiFi.

    Constants.h             - Defines pins configuration and other static parameters.
    AccumulatorFilter.h     - Used for battery temperature decoupling in  Smart Citizen Kit v.1.0 
    TemperatureDecoupler.h  - Used for battery temperature decoupling in  Smart Citizen Kit v.1.0 

  Check REAMDE.md for more information.
    
*/

#include <Wire.h>
#include <EEPROM.h>
#include "SCKAmbient.h"

SCKAmbient ambient;
  
void setup() {
  ambient.begin();
  ambient.ini();
}

void loop() {  
  ambient.execute();
}
















