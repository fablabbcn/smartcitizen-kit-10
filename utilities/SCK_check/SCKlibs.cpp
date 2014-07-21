#include "SCKlibs.h"
#include <Wire.h>
#include <EEPROM.h>


#define WIFLY_LATEST_VERSION 441
#define DEFAULT_WIFLY_FIRMWARE "ftp update wifly3-441.img"
#define DEFAULT_WIFLY_FTP_UPDATE "set ftp address 198.175.253.161"

#define AWAKE  4 //Despertar WIFI
#define PANEL A8 //Entrada panel
#define BAT   A7 //Entrada bateria

#define IO0 5  //MICS5525_HEATHER
#define IO1 13 //MICS2710_HEATHER
#define IO2 9  //MICS2710_ALTAIMPEDANCIA
#define IO3 10  //MICS2710_ALTAIMPEDANCIA
#define FACTORY 7  //factory RESET/AP RN131
#define CONTROL 12  //Control Mode

#define S0 A4 //MICS_5525
#define S1 A5 //MICS_2710
#define S2 A2 //SENS_5525
#define S3 A3 //SENS_2710
#define S4 A0 //MICRO
#define S5 A1 //LDR

#define OPEN  "0"
#define WEP   "1"
#define WPA1  "2"
#define WPA2  "4"
#define WEP64 "8"

#define INT_ANT "0" //EXT_ANT
#define EXT_ANT "1" //EXT_ANT

char* mySSID = "Red";
char* myPassword = "Pass";
char* wifiEncript = WPA2;
char* antenna  = INT_ANT; //EXT_ANT           

#define buffer_length        32
static char buffer[buffer_length];

boolean sck::checkEEPROM() {
  boolean ok = true;
  for (int i=0; i<32000; i++)
    {
      writeEEPROM(i, 'x');
      Serial.print(".");
      if ((i%100) == 0) 
        {
          Serial.println();
          Serial.print(i/100);
        }
      if (readEEPROM(i)!= 'x') 
        {
          ok = false;
          break;
        }
    }
  Serial.println();
  return ok;
}  
#define TWI_FREQ 400000L //Frecuencia bus I2C

void sck::begin() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Wire.begin();
  TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;  
  if (EEPROM.read(0)>=2) EEPROM.write(0, 0);
  pinMode(IO0, OUTPUT); //VH_MICS5525
  pinMode(IO1, OUTPUT); //VH_MICS2710
  pinMode(IO2, OUTPUT); //MICS2710_ALTAIMPEDANCIA
  pinMode(AWAKE, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(FACTORY, OUTPUT);
  pinMode(CONTROL, INPUT);
  digitalWrite(AWAKE, HIGH); 
//  digitalWrite(FACTORY, LOW); 
  digitalWrite(FACTORY, HIGH); 
}

 boolean sck::findInResponse(const char *toMatch,
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

void sck::recovery()
{
  if (EEPROM.read(0) == 0)
    {
      Serial.println(F("Reseting...")); 
      digitalWrite(FACTORY, HIGH);
      delay(1000);
      digitalWrite(FACTORY, LOW);
      delay(1000);
      digitalWrite(FACTORY, HIGH);
      delay(1000);
      digitalWrite(FACTORY, LOW);
      delay(1000);
      digitalWrite(FACTORY, HIGH);
      delay(1000);
      digitalWrite(FACTORY, LOW);
      delay(1000);
      digitalWrite(FACTORY, HIGH);
      delay(1000);
      digitalWrite(FACTORY, LOW);
      delay(1000);
      digitalWrite(FACTORY, HIGH);
      delay(1000);
      digitalWrite(FACTORY, LOW);
      delay(1000);
      Serial.println("Please, turn off the board.");
      EEPROM.write(0,1);
      while(true);
    }
  else EEPROM.write(0,2);
}

void sck::skipRemainderOfResponse(unsigned int timeOut) {
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

boolean sck::sendCommand(const __FlashStringHelper *command,
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
    if (!findInResponse(expectedResponse, 3000)) {
      return false;
    }
  }
  return true;
}

boolean sck::sendCommand(const char *command,
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
    if (!findInResponse(expectedResponse, 3000)) {
      return false;
    }
    //findInResponse(expectedResponse);
  }
  return true;
}

#define COMMAND_MODE_ENTER_RETRY_ATTEMPTS 2

#define COMMAND_MODE_GUARD_TIME 250 // in milliseconds

boolean sck::enterCommandMode() {
    for (int retryCount = 0; retryCount < COMMAND_MODE_ENTER_RETRY_ATTEMPTS; retryCount++) 
     {
      delay(COMMAND_MODE_GUARD_TIME);
      Serial1.print(F("$$$"));
      delay(COMMAND_MODE_GUARD_TIME);
      Serial1.println();
      Serial1.println();
      if (findInResponse("\r\n<", 1000))
      {
        return true;
      }
    }
    return false;
}

boolean sck::reset() {
      //enterCommandMode();
      sendCommand(F("factory R"), false, "Set Factory Defaults"); // Store settings
      sendCommand(F("save"), false, "Storing in config"); // Store settings
      delay(1000);
      sendCommand(F("reboot"), false, "*READY*");
}

boolean sck::exitCommandMode() {
    for (int retryCount = 0; retryCount < COMMAND_MODE_ENTER_RETRY_ATTEMPTS; retryCount++) 
     {
      if (sendCommand(F("exit"), false, "EXIT")) 
      {
      return true;
      }
    }
    return false;
}

boolean sck::connect()
  {
    if (!ready())
    {
      if(enterCommandMode())
        {    
            sendCommand(F("set wlan join 1")); // Disable AP mode
            sendCommand(F("set ip dhcp 1")); // Enable DHCP server
            sendCommand(F("set ip proto 10")); //Modo TCP y modo HTML
            sendCommand(F("set wlan auth "), true);
            sendCommand(wifiEncript);
            boolean mode = true;
            if ((wifiEncript==WEP)||(wifiEncript==WEP64)) mode=false;
            sendCommand(F("set wlan ssid "), true);
            sendCommand(mySSID);
            if (mode) sendCommand(F("set wlan phrase "), true);  // WPA1, WPA2, OPEN
            else sendCommand(F("set wlan key "), true);
            sendCommand(myPassword);
            sendCommand(F("set wlan ext_antenna "), true);
            sendCommand(antenna);
            sendCommand(F("save"), false, "Storing in config"); // Store settings
            sendCommand(F("reboot"), false, "*READY*");
            return true;
        }
      else return false;     
    }
   else return true;  
  }  

uint32_t baud[7]={2400, 4800, 9600, 19200, 38400, 57600, 115200};

boolean sck::repair()
{
  boolean repair = false;
  if(!enterCommandMode())
    {
      for (int i=6; ((i>=0)&&(!repair)); i--)
      {
//        Serial1.begin(baud[i]);
//        Serial.println(baud[i]);
        if(enterCommandMode()) 
        {
          reset();
          repair = true;
        }
        Serial1.begin(9600);
      }
    }
  else repair = true;
  return repair;
}

boolean sck::ready()
{
  if(!enterCommandMode())
    {
      repair();
    }
  if (enterCommandMode())
    {
      Serial1.println(F("join"));
      if (findInResponse("Associated!", 8000)) 
      {
        skipRemainderOfResponse(3000);
        exitCommandMode();
        return(true);
      }
   } 
  else return(false);
}
  
char* itoa(int32_t number)
  {
   byte count = 0;
   uint32_t temp;
   if (number < 0) {temp = number*(-1); count++;} 
   while ((temp/10)!=0) 
   {
     temp = temp/10;
     count++;
   }
   int i;
   if (number < 0) {temp = number*(-1);} 
   else temp = number;
   for (i = count; i>=0; i--) 
   { 
     buffer[i] = temp%10 + '0'; 
     temp = temp/10; 
   }
   if (number < 0) {buffer[0] = '-';} 
   buffer[count + 1] = 0x00;
   return buffer;   
  }
  

char* sck::getWiFlyVersion(unsigned long timeOut) {
     Serial1.println();
     Serial1.println();
     char newChar = '<';
     byte offset = 0;
     unsigned long time = millis();
     while (((millis()-time)<timeOut))
      {
        if (Serial1.available())
        {
          newChar = Serial1.read();
          time = millis();
          if (( newChar != '<')&&( newChar != '>'))
          {
                buffer[offset] = newChar;
                offset++;
          }
          else if (newChar=='>') break;
        }
      }
      skipRemainderOfResponse(1000);
      
      if (newChar=='>') 
        {
          buffer[offset] = 0x00;
          return buffer;
        }
      else return "0";   
}

int sck::checkWiFlyVersion(char *text) {
   if (text[0]=='0')
     {
       Serial.println(F("Error reading version."));
       return -1;
     }
   else if (text[0]<'4') Serial.println(F("Old version, please update."));
   else if ((text[0]=='4')&&(text[2]=='0')) Serial.println(F("Warning version, please update."));
   else if ((text[0]=='4')&&(text[2]=='4')) Serial.println(F("WiFly up to date.")); 
   else if (text[0]=='W') 
     {
       Serial.println(F("Started as web_app"));
       return 0;
     }
   return 1;
}

boolean sck::webAppRepair() {
  if (enterCommandMode())
    {
      Serial1.println(F("boot image 2"));
      if (findInResponse("= OK", 8000)) 
      {
        skipRemainderOfResponse(3000);
        Serial1.println(F("save"));
        skipRemainderOfResponse(3000);
        Serial1.println(F("reboot"));
        skipRemainderOfResponse(3000);
        enterCommandMode();
        Serial.print("Firmware version: "); 
        char *Version = getWiFlyVersion(1000);
        Serial.println(Version);
        int state = checkWiFlyVersion(Version);
        if (state==1) Serial.print("Wifi device is ok :)");
        return(true);
      }
   } 
  else return(false);
}

#define E2PROM               0x50    // Direcion de la EEPROM

void sck::writeEEPROM(uint16_t eeaddress, uint8_t data) {
  uint8_t retry = 0;
  while ((readEEPROM(eeaddress)!=data)&&(retry<10))
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

byte sck::readEEPROM(uint16_t eeaddress) {
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

#define RTC_ADDRESS          0x68    // Direcion de la RTC

boolean sck::checkRTC() {
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

#if F_CPU == 8000000 
  #define  VAL_MAX_BATTERY                             4200
  #define  VAL_MIN_BATTERY                             3000
  #define  Vref                                        3300
#else
  #define  VAL_MAX_BATTERY                             4050
  #define  VAL_MIN_BATTERY                             3000
  #define  Vref                                        5000
#endif


uint16_t sck::getPanel(){
#if F_CPU == 8000000 
  uint16_t value = 11*average(PANEL)*Vref/1023.;
  if (value > 500) value = value + 120; //Tension del diodo de proteccion
  else value = 0;
#else
  uint16_t value = 3*average(PANEL)*Vref/1023.;
  if (value > 500) value = value + 750; //Tension del diodo de proteccion
  else value = 0;
#endif
  return value;
}

float sck::getBattery() {
  uint16_t temp = average(BAT);
#if F_CPU == 8000000 
  float voltage = Vref*temp/1023.;
  voltage = voltage + (voltage/180)*100;
#else
  float voltage = Vref*temp/1023.;
#endif
  temp = map(voltage, VAL_MIN_BATTERY, VAL_MAX_BATTERY, 0, 1000);
  if (temp>1000) temp=1000;
  if (temp<0) temp=0;
  return temp/10.; 
}

#define RES 256   //Resolucion de los potenciometros digitales

#if F_CPU == 8000000 
  #define R1  12    //Kohm
#else
  #define R1  82    //Kohm
#endif

#define P1  100   //Kohm 

float kr= ((float)P1*1000)/RES;     //Constante de conversion a resistencia de potenciometrosen ohmios

void writeMCP(byte deviceaddress, byte address, int data ) {
  if (data>RES) data=RES;
  Wire.beginTransmission(deviceaddress);
  address=(address<<4)|bitRead(data, 8) ;
  Wire.write(address);
  Wire.write(lowByte(data));
  Wire.endTransmission();
  delay(4);
}

int readMCP(int deviceaddress, uint16_t address ) {
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
  #define MCP3               0x2D    // Direcion del mcp3 Ajuste carga bateria
float sck::readCharge() {
  float resistor = kr*readMCP(MCP3, 0x00)/1000;    
  float current = 1000./(2+((resistor * 10)/(resistor + 10)));
  return(current);
}
#else
float SCKBase::readCharge() {
  return(500);
}
#endif

