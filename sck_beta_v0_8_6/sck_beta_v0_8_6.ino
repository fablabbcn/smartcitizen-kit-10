/*
<<<<<<< HEAD

  SCKBase.ino
  Supports core and data management functions (Power, WiFi, SD storage, RTClock and EEPROM storage)

  - Modules supported:

    - WIFI (Microchip RN131 (WiFly))
    - SD CARD
    - RTC (DS1339U and DS1307Z)
    - EEPROM (24LC256)
    - POWER MANAGEMENT IC's

*/

/* 
=======

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
#include "Constants.h"

/* 

GLOBAL FIRMWARE CONFIGURATION FLAGS

*/

#define USBEnabled      true 
#define wiflyEnabled    true
#define wiflySleep      true
#define sensorEnabled   true
#define debuggEnabled   true
#define MICSEnabled     true
#define autoUpdateWiFly true
#define ADXLEnabled     false
#define DataRaw         true

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
>>>>>>> a8e5615ef2e4f0c572845413fe738ec18471c284

BASE Contants and Defaults

*/

<<<<<<< HEAD
// WI-FI Networks can hardcoded directly here to perform manual setups.

#define networks 0
#if (networks > 0)
char* mySSID[networks]      = { 
  "Red1"        , "Red2"        , "Red3"             };
char* myPassword[networks]  = { 
  "Pass1"      , "Pass2"       , "Pass3"            };
char* wifiEncript[networks] = { 
  WPA2         , WPA2          , WPA2               };
char* antennaExt[networks]  = { 
  INT_ANT      , INT_ANT       , INT_ANT            }; //EXT_ANT
#endif

boolean connected;                  

#define buffer_length        32
static char buffer[buffer_length];

#define TWI_FREQ 400000L        // i2C bus freq.

void sckBegin() {
  Wire.begin();
  TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;  
  Serial.begin(115200);
  Serial1.begin(9600);
  pinMode(IO0, OUTPUT);        //VH_MICS5525
  pinMode(IO1, OUTPUT);        //VH_MICS2710
  pinMode(IO2, OUTPUT);        // MICS2710_HIGH_IMPEDANCE
  pinMode(AWAKE, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(FACTORY, OUTPUT);
  pinMode(CONTROL, INPUT);
  digitalWrite(AWAKE, LOW); 
  digitalWrite(FACTORY, LOW); 
#if ((decouplerComp)&&(F_CPU > 8000000 ))
  decoupler.setup();
#endif
#if F_CPU == 8000000 
  sckWriteCharge(350);

  sckWriteVH(MICS_5525, 2700);    // MICS5525_START
  digitalWrite(IO0, HIGH);        // MICS5525

  sckWriteVH(MICS_2710, 1700);    // MICS2710_START
  digitalWrite(IO1, HIGH);        // MICS2710_HEATHER
  digitalWrite(IO2, LOW);         // MICS2710_HIGH_IMPEDANCE
  pinMode(AWAKE, OUTPUT);

  pinMode(IO3, OUTPUT);
  digitalWrite(IO3, HIGH);         // MICS POWER LINE 
  
  #if ADXLEnabled
    sckWriteADXL(0x2D, 0x08);
    //  sckWriteADXL(0x31, 0x00); //2g
    //  sckWriteADXL(0x31, 0x01); //4g
    sckWriteADXL(0x31, 0x02); //8g
    //  sckWriteADXL(0x31, 0x03); //16g
  #endif
  
#else
  sckWriteVH(MICS_5525, 2400);    // MICS5525_START
  digitalWrite(IO0, HIGH);        // MICS5525

  sckWriteVH(MICS_2710, 1700);    // MICS2710_START
  digitalWrite(IO1, HIGH);        // MICS2710
  digitalWrite(IO2, LOW);         // MICS2710_HIGH_IMPEDANCE
#endif

  sckWriteRL(MICS_5525, 100000);  // START LOADING MICS5525
  sckWriteRL(MICS_2710, 100000);  // START LOADING MICS2710
}

void sckConfig(){
  if (!sckCompareDate(__TIME__, sckReadData(EE_ADDR_TIME_VERSION, 0, 0)))
  {
    sckReset();
#if debuggEnabled
    Serial.println(F("Resetting..."));
#endif
    for(uint16_t i=0; i<DEFAULT_ADDR_MEASURES; i++) sckWriteEEPROM(i, 0x00);  // Memory erasing
    sckWriteData(EE_ADDR_TIME_VERSION, 0, __TIME__);
    sckWriteData(EE_ADDR_TIME_UPDATE, 0, DEFAULT_TIME_UPDATE);
    sckWriteData(EE_ADDR_NUMBER_UPDATES, 0, DEFAULT_MIN_UPDATES);
    sckWriteData(EE_ADDR_MAC, 0, sckMAC());

#if (networks > 0)
    for(byte i=0; i<networks; i++)
    {
      sckWriteData(DEFAULT_ADDR_SSID, i, mySSID[i]);
      sckWriteData(DEFAULT_ADDR_PASS, i, myPassword[i]);
      sckWriteData(DEFAULT_ADDR_AUTH, i, wifiEncript[i]);
      sckWriteData(DEFAULT_ADDR_ANTENNA, i, antennaExt[i]);
    }
    sckWriteintEEPROM(EE_ADDR_NUMBER_NETS, networks);
#endif
  }
  timer1Initialize();
  timer1AttachInterrupt();
}

float average(int anaPin) {
  int lecturas = 100;
  long total = 0;
  float average = 0;
  for(int i=0; i<lecturas; i++)
  {
    //delay(1);
    total = total + analogRead(anaPin);
  }
  average = (float)total / lecturas;  
  return(average);
}

boolean sckCheckText(byte inByte, char* text, byte *check)
{
  if (inByte == text[*check]) 
  {
    *check = *check + 1;
    //Serial.print(*check);
    if (*check == strlen(text)) 
    {
      *check = 0;
      return true;
    }
  }
  else *check = 0;
  return false;
}

boolean sckCompareDate(char* text, char* text1)
{
  if ((strlen(text))!=(strlen(text1))) return false;
  else 
  {
    for(int i=0; i<strlen(text); i++)
    {
      if (text[i]!=text1[i]) return false;
    }
  }
  return true;
}

void sckCheckData()
{  
  uint32_t check_measures = sckReadintEEPROM(EE_ADDR_NUMBER_MEASURES);
  if (check_measures > 8)
  {
    if (((check_measures + 1)%10) != 0) check_measures = 0;
  }
  else check_measures = 0;
  sckWriteintEEPROM(EE_ADDR_NUMBER_MEASURES, check_measures); 
}


void sckWriteMCP(byte deviceaddress, byte address, int data ) {
  if (data>RES) data=RES;
  Wire.beginTransmission(deviceaddress);
  address=(address<<4)|bitRead(data, 8) ;
  Wire.write(address);
  Wire.write(lowByte(data));
  Wire.endTransmission();
  delay(4);
}

int sckReadMCP(int deviceaddress, uint16_t address ) {
  byte rdata = 0xFF;
  int  data = 0x0000;
  Wire.beginTransmission(deviceaddress);
  address=(address<<4)|B00001100;
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress,2);
  unsigned long time = millis();
  while (!Wire.available()) if ((millis() - time)>500) return 0x00;
  rdata = Wire.read(); 
  data=rdata<<8;
  while (!Wire.available()); 
  rdata = Wire.read(); 
  data=data|rdata;
  return data;
}

#if F_CPU == 8000000 
float sckReadCharge() {
  float resistor = kr*sckReadMCP(MCP3, 0x00)/1000;    
  float current = 1000./(2+((resistor * 10)/(resistor + 10)));
#if debuggSCK
  Serial.print("Resistor : ");
  Serial.print(resistor);
  Serial.print(" kOhm, ");  
  Serial.print("Current : ");
  Serial.print(current);
  Serial.println(" mA");  
#endif
  return(current);
}

void sckWriteCharge(int current) {
  if (current < 100) current = 100;
  else if (current > 500) current = 500;
  float Rp = (1000./current)-2;
  float resistor = Rp*10/(10-Rp);
  sckWriteMCP(MCP3, 0x00, (uint8_t)(resistor*1000/kr));    
#if debuggSCK
  Serial.print("Rc : ");
  Serial.print(Rp + 2);
  Serial.print(" kOhm, ");
  Serial.print("Rpot : ");
  Serial.print(resistor);
  Serial.print(" kOhm, ");  
  Serial.print("Current : ");
  Serial.print(current);
  Serial.println(" mA");  
#endif
}
#endif    

void sckWriteEEPROM(uint16_t eeaddress, uint8_t data ) {
  uint8_t retry = 0;
  while ((sckReadEEPROM(eeaddress)!=data)&&(retry<10))
  {  
    Wire.beginTransmission(E2PROM);
    Wire.write((byte)(eeaddress >> 8));   // MSB
    Wire.write((byte)(eeaddress & 0xFF)); // LSB
    Wire.write(data);
    Wire.endTransmission();
    delay(6);
    retry++;
  }
}

byte sckReadEEPROM(uint16_t eeaddress ) {
  byte rdata = 0xFF;
  Wire.beginTransmission(E2PROM);
  Wire.write((byte)(eeaddress >> 8));   // MSB
  Wire.write((byte)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(E2PROM,1);
  while (!Wire.available()); 
  rdata = Wire.read();
  return rdata;
}

void sckWriteintEEPROM(uint16_t eeaddress, uint16_t data )
{
  sckWriteEEPROM(eeaddress , highByte(data));
  sckWriteEEPROM(eeaddress  + 1, lowByte(data)); 
  ;
}

uint16_t sckReadintEEPROM(uint16_t eeaddress)
{
  return (sckReadEEPROM(eeaddress)<<8)+ sckReadEEPROM(eeaddress + 1); 
}

char* sckReadData(uint16_t eeaddress, uint16_t pos, uint8_t dec)
{
  eeaddress = eeaddress + buffer_length * pos;
  uint8_t temp = sckReadEEPROM(eeaddress);
  uint16_t i;
  for ( i = eeaddress; ((temp!= 0x00)&&(temp<0x7E)&&(temp>0x1F)&&((i - eeaddress)<buffer_length)); i++) 
  {
    buffer[i - eeaddress] = sckReadEEPROM(i);
    temp = sckReadEEPROM(i + 1);
  }
  if ((buffer[0] !='0')&&(dec>0))
  {
    if ((i - eeaddress)<dec)
    {
      for (int j = 0 ; j<(dec + 1); j++) buffer[(dec + 1) - j] = buffer[(i - j - eeaddress)];
      for (int j = 0 ; j<((dec + 1) -(i - eeaddress)); j++) buffer[j] = '0';
      i = i + dec + 1 - (i - eeaddress);
    }
    for (int j = 0 ; j<dec; j++) buffer[i - j - eeaddress] = buffer[i - j - 1 - eeaddress];
    buffer[i - dec - eeaddress] = '.';
    buffer[i + 1 - eeaddress] = 0x00;
  }
  else buffer[i - eeaddress] = 0x00;
  return buffer;
}


void sckWriteData(uint16_t eeaddress, uint16_t pos, char* text )
{
  uint16_t eeaddressfree = eeaddress + buffer_length * pos;
  for (uint16_t i = eeaddressfree; i< (eeaddressfree + buffer_length); i++) sckWriteEEPROM(i, 0x00);
  for (uint16_t i = eeaddressfree; text[i - eeaddressfree]!= 0x00; i++) sckWriteEEPROM(i, text[i - eeaddressfree]);
  if (eeaddress == DEFAULT_ADDR_MEASURES) sckWriteintEEPROM(EE_ADDR_NUMBER_MEASURES, pos);
}

boolean sckCheckRTC() {
  Wire.beginTransmission(RTC_ADDRESS);
  Wire.write(0x00); //Address
  Wire.endTransmission();
  delay(4);
  Wire.requestFrom(RTC_ADDRESS,1);
  unsigned long time = millis();
  while (!Wire.available()) if ((millis() - time)>500) return false;
  Wire.read();
  return true;
}

boolean sckRTCadjust(char *time) {    
  byte rtc[6] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00                                      };
  byte count = 0x00;
  byte data_count=0;
  while (time[count]!=0x00)
  {
    if(time[count] == '-') data_count++;
    else if(time[count] == ' ') data_count++;
    else if(time[count] == ':') data_count++;
    else if ((time[count] >= '0')&&(time[count] <= '9'))
    { 
      rtc[data_count] =(rtc[data_count]<<4)|(0x0F&time[count]);
    }  
    else break;
    count++;
  }  
  if (data_count == 5)
  {
#if F_CPU == 8000000 
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write((int)0);
    Wire.write(rtc[5]);
    Wire.write(rtc[4]);
    Wire.write(rtc[3]);
    Wire.write(0x00);
    Wire.write(rtc[2]);
    Wire.write(rtc[1]);
    Wire.write(rtc[0]);
    Wire.endTransmission();
    delay(4);
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x0E); //Address
    Wire.write(0x00); //Value
    Wire.endTransmission();
#else
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write((int)0);
    Wire.write(rtc[5]);
    Wire.write(rtc[4]);
    Wire.write(rtc[3]);
    Wire.write(0x00);
    Wire.write(rtc[2]);
    Wire.write(rtc[1]);
    Wire.write(rtc[0]);
    Wire.write((int)0);
    Wire.endTransmission();
    return true;
#endif
    return true;
=======
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
>>>>>>> a8e5615ef2e4f0c572845413fe738ec18471c284
  }
  return false;  
}

char* sckRTCtime() {
  Wire.beginTransmission(RTC_ADDRESS);
  Wire.write((int)0); 
  Wire.endTransmission();
  Wire.requestFrom(RTC_ADDRESS, 7);
  uint8_t seconds = (Wire.read() & 0x7F);
  uint8_t minutes = Wire.read();
  uint8_t hours = Wire.read();
  Wire.read();
  uint8_t day = Wire.read();
  uint8_t month = Wire.read();
  uint8_t year = Wire.read();
  buffer[0] = '2';
  buffer[1] = '0';
  buffer[2] = (year>>4) + '0';
  buffer[3] = (year&0x0F) + '0';
  buffer[4] = '-';
  buffer[5] = (month>>4) + '0';
  buffer[6] = (month&0x0F) + '0';
  buffer[7] = '-';
  buffer[8] = (day>>4) + '0';
  buffer[9] = (day&0x0F) + '0';
  buffer[10] = ' ';
  buffer[11] = (hours>>4) + '0';
  buffer[12] = (hours&0x0F) + '0';
  buffer[13] = ':';
  buffer[14] = (minutes>>4) + '0';
  buffer[15] = (minutes&0x0F) + '0';
  buffer[16] = ':';
  buffer[17] = (seconds>>4) + '0';
  buffer[18] = (seconds&0x0F) + '0';
  buffer[19] = 0x00;
  return buffer;
}


uint16_t sckGetPanel(){
#if F_CPU == 8000000 
  uint16_t value = 11*average(PANEL)*Vcc/1023.;
  if (value > 500) value = value + 120; //Tension del diodo de proteccion
  else value = 0;
#else
  uint16_t value = 3*average(PANEL)*Vcc/1023.;
  if (value > 500) value = value + 750; //Tension del diodo de proteccion
  else value = 0;
#endif
#if debuggSCK
  Serial.print("Panel = ");
  Serial.print(value);
  Serial.println(" mV");
#endif
  return value;
}

uint16_t sckGetBattery() {
  uint16_t temp = average(BAT);
#if F_CPU == 8000000 
  float voltage = Vcc*temp/1023.;
  voltage = voltage + (voltage/180)*100;
#else
  float voltage = Vcc*temp/1023.;
#endif
  temp = map(voltage, VAL_MIN_BATTERY, VAL_MAX_BATTERY, 0, 1000);
  if (temp>1000) temp=1000;
  if (temp<0) temp=0;
#if debuggSCK
  Serial.print("Vbat: ");
  Serial.print(voltage);
  Serial.print(" mV, ");
  Serial.print("Battery level: ");
  Serial.print(temp/10.);
  Serial.println(" %");
#endif
  return temp; 
}

boolean sckFindInResponse(const char *toMatch,
unsigned int timeOut = 1000) {
  int byteRead;

  unsigned long timeOutTarget; // in milliseconds

  for (unsigned int offset = 0; offset < strlen(toMatch); offset++) {
    timeOutTarget = millis() + timeOut; // Doesn't handle timer wrapping
    while (!Serial1.available()) {
      // Wait, with optional time out.
      if (timeOut > 0) {
        if (millis() > timeOutTarget) {
          return false;
        }
      }
      delay(1); // This seems to improve reliability slightly
    }
    byteRead = Serial1.read();
    //Serial.print((char)byteRead);
    delay(1); // Removing logging may affect timing slightly

    if (byteRead != toMatch[offset]) {
      offset = 0;
      // Ignore character read if it's not a match for the start of the string
      if (byteRead != toMatch[offset]) {
        offset = -1;
      }
      continue;
    }
  }

  return true;
}

void sckSkipRemainderOfResponse(unsigned int timeOut) {
  unsigned long time = millis();
  while (((millis()-time)<timeOut))
  {
    if (Serial1.available())
    { 
      byte temp = Serial1.read();
      //Serial.write(temp);
      time = millis();
    }
  }
}

boolean sckSendCommand(const __FlashStringHelper *command,
boolean isMultipartCommand = false,
const char *expectedResponse = "AOK") {
  Serial1.print(command);
  delay(20);
  if (!isMultipartCommand) {
    Serial1.flush();
    Serial1.println();

    // TODO: Handle other responses
    //       (e.g. autoconnect message before it's turned off,
    //        DHCP messages, and/or ERR etc)
    if (!sckFindInResponse(expectedResponse, 3000)) {
      return false;
    }
    //sckFindInResponse(expectedResponse);
  }
  return true;
}

boolean sckSendCommand(const char *command,
boolean isMultipartCommand = false,
const char *expectedResponse = "AOK") {
  Serial1.print(command);
  delay(20);
  if (!isMultipartCommand) {
    Serial1.flush();
    Serial1.println();

    // TODO: Handle other responses
    //       (e.g. autoconnect message before it's turned off,
    //        DHCP messages, and/or ERR etc)
    if (!sckFindInResponse(expectedResponse, 3000)) {
      return false;
    }
    //findInResponse(expectedResponse);
  }
  return true;
}

#define COMMAND_MODE_ENTER_RETRY_ATTEMPTS 2

#define COMMAND_MODE_GUARD_TIME 250 // in milliseconds

boolean sckEnterCommandMode() {
  for (int retryCount = 0; retryCount < COMMAND_MODE_ENTER_RETRY_ATTEMPTS; retryCount++) 
  {
    delay(COMMAND_MODE_GUARD_TIME);
    Serial1.print(F("$$$"));
    delay(COMMAND_MODE_GUARD_TIME);
    Serial1.println();
    Serial1.println();
    if (sckFindInResponse("\r\n<", 1000))
    {
      return true;
    }
  }
  return false;
}


boolean sckSleep() {
  sckEnterCommandMode();
  sckSendCommand(F("sleep"));
}

boolean sckReset() {
  sckEnterCommandMode();
  sckSendCommand(F("factory R"), false, "Set Factory Defaults"); // Store settings
  sckSendCommand(F("save"), false, "Storing in config"); // Store settings
  sckSendCommand(F("reboot"), false, "*READY*");
}

boolean sckExitCommandMode() {
  for (int retryCount = 0; retryCount < COMMAND_MODE_ENTER_RETRY_ATTEMPTS; retryCount++) 
  {
    if (sckSendCommand(F("exit"), false, "EXIT")) 
    {
      return true;
    }
  }
  return false;
}

boolean sckConnect()
{
  if (!sckReady())
  {
    if (sckReadintEEPROM(EE_ADDR_NUMBER_NETS)<1) return false;
    if(sckEnterCommandMode())
    {    
      sckSendCommand(F("set wlan join 1")); // Disable AP mode
      sckSendCommand(F("set ip dhcp 1")); // Enable DHCP server
      sckSendCommand(F("set ip proto 10")); //Modo TCP y modo HTML
      sckSendCommand(F("set ftp address 198.175.253.161")); //ftp server update
      sckSendCommand(F("set ftp mode 1"));
      char* auth;
      char* ssid;
      char* pass;
      char* antenna;
      for (uint16_t nets = 0  ; nets < sckReadintEEPROM(EE_ADDR_NUMBER_NETS); nets++) {
        auth = sckReadData(DEFAULT_ADDR_AUTH, nets, 0);
        sckSendCommand(F("set wlan auth "), true);
        sckSendCommand(auth);
        boolean mode = true;
        if ((auth==WEP)||(auth==WEP64)) mode=false;
#if debuggEnabled
<<<<<<< HEAD
       if (!wait) Serial.print(auth);
=======
      if (!wait) Serial.println(F("SCK Connected!!"));
#endif
#if autoUpdateWiFly
      sckCheckWiFly();
>>>>>>> a8e5615ef2e4f0c572845413fe738ec18471c284
#endif
        ssid = sckReadData(DEFAULT_ADDR_SSID, nets, 0);
        sckSendCommand(F("set wlan ssid "), true);
        sckSendCommand(ssid);
#if debuggEnabled
        if (!wait) 
        {
          Serial.print(F(" "));
          Serial.print(ssid);
        }
#endif
        pass = sckReadData(DEFAULT_ADDR_PASS, nets, 0);
        if (mode) sckSendCommand(F("set wlan phrase "), true);  // WPA1, WPA2, OPEN
        else sckSendCommand(F("set wlan key "), true);
        sckSendCommand(pass);
#if debuggEnabled
<<<<<<< HEAD
        if (!wait)
        {
          Serial.print(F(" "));
          Serial.print(pass);
        }
=======
        if (!wait) Serial.println(F("Updating RTC..."));
>>>>>>> a8e5615ef2e4f0c572845413fe738ec18471c284
#endif
        antenna = sckReadData(DEFAULT_ADDR_ANTENNA, nets, 0);
        sckSendCommand(F("set wlan ext_antenna "), true);
        sckSendCommand(antenna);
#if debuggEnabled
      if (!wait)
      {
        Serial.print(F(" "));
        Serial.println(antenna);
      }
#endif
        sckSendCommand(F("save"), false, "Storing in config"); // Store settings
        sckSendCommand(F("reboot"), false, "*READY*");
        if (sckReady()) return true;
        sckEnterCommandMode();
      }     
    } 
    return false;
  }
  else return true;  
}  

void sckAPmode(char* ssid)
{
  if (sckEnterCommandMode())
  {    
    sckSendCommand(F("set wlan join 7")); // Enable AP mode
    //sckSendCommand(F("set wlan channel <value> // Specify the channel to create network
    sckSendCommand(F("set wlan ssid "), true); // Set up network broadcast SSID
    sckSendCommand(ssid);

    buffer[6] = 0x00;
    sckSendCommand(F("set opt device_id "), true); // Set up network broadcast SSID
    sckSendCommand(ssid);

    sckSendCommand(F("set ip dhcp 4")); // Enable DHCP server
    sckSendCommand(F("set ip address 1.2.3.4")); // Specify the IP address
    sckSendCommand(F("set ip net 255.255.255.0")); // Specify the subnet mask
    sckSendCommand(F("set ip gateway 1.2.3.4")); // Specify the gateway
    sckSendCommand(F("save"), false, "Storing in config"); // Store settings
    sckSendCommand(F("reboot"), false, "*READY*"); // Reboot the module in AP mode
  }
} 

boolean sckReady()
{
  if(!sckEnterCommandMode())
  {
    sckRepair();
  }
  if (sckEnterCommandMode())
  {
    Serial1.println(F("join"));
    if (sckFindInResponse("Associated!", 8000)) 
    {
      sckSkipRemainderOfResponse(3000);
      sckExitCommandMode();
      return(true);
    }
  } 
  else return(false);
}

boolean sckOpen(const char *addr, int port) {

  if (connected) {
    sckClose();
  } 
  if (sckEnterCommandMode())
  {
    sckSendCommand(F("open "), true);
    sckSendCommand(addr, true);
    Serial1.print(F(" "));
    Serial1.print(port);
    if (sckSendCommand("", false, "*OPEN*")) 
    {
      connected = true;
      return true;
    }
    else return false;
  }
  sckEnterCommandMode();
  return false;
}

boolean sckClose() {
  if (!connected) {
    return true;
  }
  if (sckSendCommand(F("close"), false, "*CLOS*")) {
    connected = false;
    return true;
  }
  connected = false;
  return false;
}

#define MAC_ADDRESS_BUFFER_SIZE 18 // "FF:FF:FF:FF:FF:FF\0"

char* sckMAC() {
  if (sckEnterCommandMode()) 
  {
<<<<<<< HEAD
    if (sckSendCommand(F("get mac"), false, "Mac Addr="))
    {
      char newChar;
      byte offset = 0;

      while (offset < MAC_ADDRESS_BUFFER_SIZE) {
        if (Serial1.available())
        {
          newChar = Serial1.read();
          //Serial.println(newChar);
          if ((newChar == '\n')||(newChar < '0')) {
            buffer[offset] = '\x00';
            break;
          } 
          else if (newChar != -1) {
            buffer[offset] = newChar;
            offset++;
          }
        }
      }
      buffer[MAC_ADDRESS_BUFFER_SIZE-1] = '\x00';
      sckExitCommandMode();
    }        
  }

  return buffer;
}

char* sckid() {
  char* temp = sckMAC();  
  byte len = strlen(temp);
  byte j = 4;
  buffer[0] = 'S';
  buffer[1] = 'C';
  buffer[2] = 'K';
  buffer[3] = '_';
  for(byte i=12; i<len; i++)
  {
    if (temp[i] == ':') buffer[j] = '-';
    else buffer[j] = temp[i];
    j++;
  }
  buffer[j] = 0x00;
  return buffer;
}


#define SCAN_BUFFER_SIZE 4 

char* sckScan() {
  if (sckEnterCommandMode()) 
  {
    if (sckSendCommand(F("scan"), false, "Found "))
    {
      char newChar;
      byte offset = 0;

      while (offset < SCAN_BUFFER_SIZE) {
        if (Serial1.available())
        {
          newChar = Serial1.read();
          if ((newChar == '\r')||(newChar < '0')) {
            buffer[offset] = '\x00';
            break;
          } 
          else if (newChar != -1) {
            buffer[offset] = newChar;
            offset++;
          }
        }
      }
      buffer[SCAN_BUFFER_SIZE-1] = '\x00';
      sckFindInResponse("END:\r\n", 2000);
      sckExitCommandMode();
    }        
  }
  return buffer;
} 

char* itoa(int32_t number)
{
  byte count = 0;
  uint32_t temp;
  if (number < 0) {
    temp = number*(-1); 
    count++;
  } 
  while ((temp/10)!=0) 
  {
    temp = temp/10;
    count++;
  }
  int i;
  if (number < 0) {
    temp = number*(-1);
  } 
  else temp = number;
  for (i = count; i>=0; i--) 
  { 
    buffer[i] = temp%10 + '0'; 
    temp = temp/10; 
  }
  if (number < 0) {
    buffer[0] = '-';
  } 
  buffer[count + 1] = 0x00;
  return buffer;   
}

#if autoUpdateWiFly

void sckCheckWiFly() {
  int ver = getWiFlyVersion();
  if (ver > 0)
  {
    if (ver < WIFLY_LATEST_VERSION)
     {
      #if debuggEnabled
          if (!wait) Serial.println(F("WiFly old firm. Updating..."));
      #endif
          if(sckUpdate()) {
            #if USBEnabled
                if (!wait) Serial.println(F("Wifly Updated."));
            #endif
          } 
          else {
            #if debuggEnabled
                if (!wait) Serial.println(F("Update Fail."));
            #endif
      } 
      sckReset();
    }   
    else {
      #if debuggEnabled
         if (!wait) Serial.println(F("WiFly up to date."));
      #endif
    }
  }
  else if (!wait) Serial.println(F("Error reading the wifi version."));
}

int getWiFlyVersion() {
  if (sckEnterCommandMode()) 
  {
    if (sckSendCommand(F("ver"), false, "wifly-GSX Ver"))
    {
      char newChar;
      byte offset = 0;
      boolean prevWasNumber = false;
      while (offset < 3) {
        if (Serial1.available())
        {
          newChar = Serial1.read();
          if ((newChar != -1 && isdigit(newChar)) || newChar == '.') {
            if (newChar != '.') {
              buffer[offset] = newChar;
              offset++;
            }
            prevWasNumber = true;
          } 
          else {
            if (prevWasNumber){
              break;
            }
            prevWasNumber = false; 
          }
        }
      }
      sckExitCommandMode();
      buffer[offset] = 0x00;
      return atoi(buffer);
    } 
    return 0;
  }
  return 0;
}


boolean sckUpdate() {
  if (sckEnterCommandMode())
  {
    sckSendCommand(F(DEFAULT_WIFLY_FTP_UPDATE));
    sckSendCommand(F(DEFAULT_WIFLY_FIRMWARE));
    delay(1000);
    if (sckFindInResponse("FTP OK.", 60000))
    {
      return true;
    }
  }
  else return false;
}

=======
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
>>>>>>> a8e5615ef2e4f0c572845413fe738ec18471c284
#endif


uint32_t baud[7]={
  2400, 4800, 9600, 19200, 38400, 57600, 115200};

void sckRepair()
{
  if(!sckEnterCommandMode())
  {
    boolean repair = true;
    for (int i=6; ((i>=0)&&repair); i--)
    {
      Serial1.begin(baud[i]);
      Serial.println(baud[i]);
      if(sckEnterCommandMode()) 
      {
        sckReset();
        repair = false;
      }
      Serial1.begin(9600);
    }
  }
}

















