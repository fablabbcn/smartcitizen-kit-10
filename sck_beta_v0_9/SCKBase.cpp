/*

  SCKBase.cpp
  Supports core and data management functions (Power, WiFi, SD storage, RTClock and EEPROM storage)

  - Modules supported:

    - WIFI (Microchip RN131 (WiFly))
    - RTC (DS1339U and DS1307Z)
    - EEPROM (24LC256)
    - POWER MANAGEMENT IC's

*/

#include "Constants.h"
#include "SCKBase.h"
#include <Wire.h>
#include <EEPROM.h>

#define debugBASE false



void SCKBase::begin() {
  Wire.begin();
  TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;  
  Serial.begin(115200);
  Serial1.begin(9600);
  pinMode(IO0, OUTPUT); //VH_MICS5525
  pinMode(IO1, OUTPUT); //VH_MICS2710
  pinMode(IO2, OUTPUT); //MICS2710_HIGH_IMPEDANCE
  pinMode(AWAKE, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(FACTORY, OUTPUT);
  pinMode(CONTROL, INPUT);
  digitalWrite(AWAKE, LOW); 
  digitalWrite(FACTORY, LOW); 
}

void SCKBase::config(){
  if (!compareData(__TIME__, readData(EE_ADDR_TIME_VERSION, 0, INTERNAL)))
  {
    digitalWrite(AWAKE, HIGH); 
    for(uint16_t i=0; i<(DEFAULT_ADDR_ANTENNA + 160); i++) EEPROM.write(i, 0x00);  // Memory erasing
    writeData(EE_ADDR_TIME_VERSION, 0, __TIME__, INTERNAL);
    writeData(EE_ADDR_SENSOR_MODE, DEFAULT_MODE_SENSOR, INTERNAL);
    writeData(EE_ADDR_TIME_UPDATE, DEFAULT_TIME_UPDATE, INTERNAL);
    writeData(EE_ADDR_NUMBER_UPDATES, DEFAULT_MIN_UPDATES, INTERNAL);
    writeData(EE_ADDR_MAC, 0, MAC(), INTERNAL);

    #if (networks > 0)
        for(byte i=0; i<networks; i++)
        {
          writeData(DEFAULT_ADDR_SSID, i, mySSID[i], INTERNAL);
          writeData(DEFAULT_ADDR_PASS, i, myPassword[i], INTERNAL);
          writeData(DEFAULT_ADDR_AUTH, i, wifiEncript[i], INTERNAL);
          writeData(DEFAULT_ADDR_ANTENNA, i, antennaExt[i], INTERNAL);
        }
        writeData(EE_ADDR_NUMBER_NETS, networks, INTERNAL);
    #endif
    reset();
  }
  timer1Initialize();
}

float SCKBase::average(int anaPin) {
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

boolean SCKBase::checkText(char* text, char *text1)
{
  byte check = 0;
  byte limit = strlen(text);
  int i = 0;
  for (i = 0; ((i< strlen(text1))&&(check<limit)); i++)
    {
        if (text[check]==text1[i]) check++;
        else check = 0;
    }
  if (check == limit) 
    {
      limit = strlen(text1);
      int j = 0;
      for (j = 0; i<=limit; j++) 
        {
          if (text1[i]=='\r') text1[j]=0x00;
          else text1[j] = text1[i];
          i++;
        }
      return true;
    }
  else return false;
}

boolean SCKBase::compareData(char* text, char* text1)
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

float kr= ((float)P1*1000)/RES;     //  Resistance conversion Constant for the digital pot.

void SCKBase::writeMCP(byte deviceaddress, byte address, int data ) {
  if (data>RES) data=RES;
  Wire.beginTransmission(deviceaddress);
  address=(address<<4)|bitRead(data, 8) ;
  Wire.write(address);
  Wire.write(lowByte(data));
  Wire.endTransmission();
  delay(4);
}

int SCKBase::readMCP(int deviceaddress, uint16_t address ) {
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
  #define MCP3               0x2D    // Direction of the mcp3 Ajust the battary charge
float SCKBase::readCharge() {
  float resistor = kr*readMCP(MCP3, 0x00)/1000;    
  float current = 1000./(2+((resistor * 10)/(resistor + 10)));
  #if debugBASE
    Serial.print("Resistor : ");
    Serial.print(resistor);
    Serial.print(" kOhm, ");  
    Serial.print("Current : ");
    Serial.print(current);
    Serial.println(" mA");  
  #endif
  return(current);
}

void SCKBase::writeCharge(int current) {
  if (current < 100) current = 100;
  else if (current > 500) current = 500;
  float Rp = (1000./current)-2;
  float resistor = Rp*10/(10-Rp);
  writeMCP(MCP3, 0x00, (uint8_t)(resistor*1000/kr));    
#if debugBASE
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

void SCKBase::writeEEPROM(uint16_t eeaddress, uint8_t data) {
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

byte SCKBase::readEEPROM(uint16_t eeaddress) {
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

void SCKBase::writeData(uint32_t eeaddress, long data, uint8_t location)
{
    for (int i =0; i<4; i++) 
      {
        if (location == EXTERNAL) writeEEPROM(eeaddress + (3 -i) , data>>(i*8));
        else EEPROM.write(eeaddress + (3 -i), data>>(i*8));
      }

}

void SCKBase::writeData(uint32_t eeaddress, uint16_t pos, char* text, uint8_t location)
{
  uint16_t eeaddressfree = eeaddress + buffer_length * pos;
  if (location == EXTERNAL)
    {
      for (uint16_t i = eeaddressfree; i< (eeaddressfree + buffer_length); i++) writeEEPROM(i, 0x00);
      for (uint16_t i = eeaddressfree; text[i - eeaddressfree]!= 0x00; i++) writeEEPROM(i, text[i - eeaddressfree]);
    }
  else
    {
      
      for (uint16_t i = eeaddressfree; i< (eeaddressfree + buffer_length); i++) EEPROM.write(i, 0x00);
      for (uint16_t i = eeaddressfree; text[i - eeaddressfree]!= 0x00; i++) 
        {
          if (eeaddressfree>=DEFAULT_ADDR_SSID) if (text[i - eeaddressfree]==' ') text[i - eeaddressfree]='$';
          EEPROM.write(i, text[i - eeaddressfree]); 
        }
    }
}

uint32_t SCKBase::readData(uint16_t eeaddress, uint8_t location)
{
  uint32_t data = 0;
  for (int i =0; i<4; i++)
    {
      if (location == EXTERNAL)  data = data + (uint32_t)((uint32_t)readEEPROM(eeaddress + i)<<((3-i)*8));
      else data = data + (uint32_t)((uint32_t)EEPROM.read(eeaddress + i)<<((3-i)*8));
    }
  return data;
}

char* SCKBase::readData(uint16_t eeaddress, uint16_t pos, uint8_t location)
{
  eeaddress = eeaddress + buffer_length * pos;
  uint16_t i;
  if (location == EXTERNAL)
    {
      uint8_t temp = readEEPROM(eeaddress);
      for ( i = eeaddress; ((temp!= 0x00)&&(temp<0x7E)&&(temp>0x1F)&&((i - eeaddress)<buffer_length)); i++) 
      {
        buffer[i - eeaddress] = readEEPROM(i);
        temp = readEEPROM(i + 1);
      }
    }
  else
    {
      uint8_t temp = EEPROM.read(eeaddress);
      for ( i = eeaddress; ((temp!= 0x00)&&(temp<0x7E)&&(temp>0x1F)&&((i - eeaddress)<buffer_length)); i++) 
      {
        buffer[i - eeaddress] = EEPROM.read(i);
        temp = EEPROM.read(i + 1);
      }
    }
  buffer[i - eeaddress] = 0x00; 
  return buffer;
}

boolean SCKBase::checkRTC() {
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

boolean SCKBase::RTCadjust(char *time) {    
  byte rtc[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
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
  }
  return false;  
}

boolean SCKBase::RTCtime(char *time) {
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
  time[0] = '2';
  time[1] = '0';
  time[2] = (year>>4) + '0';
  time[3] = (year&0x0F) + '0';
  time[4] = '-';
  time[5] = (month>>4) + '0';
  time[6] = (month&0x0F) + '0';
  time[7] = '-';
  time[8] = (day>>4) + '0';
  time[9] = (day&0x0F) + '0';
  time[10] = ' ';
  time[11] = (hours>>4) + '0';
  time[12] = (hours&0x0F) + '0';
  time[13] = ':';
  time[14] = (minutes>>4) + '0';
  time[15] = (minutes&0x0F) + '0';
  time[16] = ':';
  time[17] = (seconds>>4) + '0';
  time[18] = (seconds&0x0F) + '0';
  time[19] = 0x00;
  return true;
}


uint16_t SCKBase::getPanel(float Vref){
#if F_CPU == 8000000 
  uint16_t value = 11*average(PANEL)*Vref/1023.;
  if (value > 500) value = value + 120; //Voltage protection diode
  else value = 0;
#else
  uint16_t value = 3*average(PANEL)*Vref/1023.;
  if (value > 500) value = value + 750; //Voltage protection diode
  else value = 0;
#endif
#if debugBASE
  Serial.print("Panel = ");
  Serial.print(value);
  Serial.println(" mV");
#endif
  return value;
}

uint16_t SCKBase::getBattery(float Vref) {
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
#if debugBASE
  Serial.print("Vbat: ");
  Serial.print(voltage);
  Serial.print(" mV, ");
  Serial.print("Battery level: ");
  Serial.print(temp/10.);
  Serial.println(" %");
#endif
  return temp; 
}

boolean SCKBase::findInResponse(const char *toMatch,
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

void SCKBase::skipRemainderOfResponse(unsigned int timeOut) {
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

boolean SCKBase::sendCommand(const __FlashStringHelper *command,
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

boolean SCKBase::sendCommand(const char *command,
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

boolean SCKBase::enterCommandMode() {
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


boolean SCKBase::sleep() {
  enterCommandMode();
  sendCommand(F("sleep"));
}

boolean SCKBase::reset() {
  enterCommandMode();
  sendCommand(F("factory R"), false, "Set Factory Defaults"); // Store settings
  sendCommand(F("save"), false, "Storing in config"); // Store settings
  sendCommand(F("reboot"), false, "*READY*");
}

boolean SCKBase::exitCommandMode() {
  for (int retryCount = 0; retryCount < COMMAND_MODE_ENTER_RETRY_ATTEMPTS; retryCount++) 
  {
    if (sendCommand(F("exit"), false, "EXIT")) 
    {
      return true;
    }
  }
  return false;
}

boolean SCKBase::connect()
{
  if (!ready())
  {
    if (readData(EE_ADDR_NUMBER_NETS, INTERNAL)<1) return false;
    if(enterCommandMode())
    {    
      sendCommand(F("set wlan join 1")); // Disable AP mode
      sendCommand(F("set ip dhcp 1")); // Enable DHCP server
      sendCommand(F("set ip proto 10")); //TCP mode and HTML mode
      sendCommand(F(DEFAULT_WIFLY_FTP_UPDATE)); //ftp server update
      sendCommand(F("set ftp mode 1"));
      char* auth;
      char* ssid;
      char* pass;
      char* antenna;
      for (uint16_t nets = 0  ; nets < readData(EE_ADDR_NUMBER_NETS, INTERNAL); nets++) {
        auth = readData(DEFAULT_ADDR_AUTH, nets, INTERNAL);
        sendCommand(F("set wlan auth "), true);
        sendCommand(auth);
        boolean mode = true;
        if ((auth==WEP)||(auth==WEP64)) mode=false;
        ssid = readData(DEFAULT_ADDR_SSID, nets, INTERNAL);
        sendCommand(F("set wlan ssid "), true);
        sendCommand(ssid);
        pass = readData(DEFAULT_ADDR_PASS, nets, INTERNAL);
        if (mode) sendCommand(F("set wlan phrase "), true);  // WPA1, WPA2, OPEN
        else sendCommand(F("set wlan key "), true);
        sendCommand(pass);
        antenna = readData(DEFAULT_ADDR_ANTENNA, nets, INTERNAL);
        sendCommand(F("set wlan ext_antenna "), true);
        sendCommand(antenna);
        sendCommand(F("save"), false, "Storing in config"); // Store settings
        sendCommand(F("reboot"), false, "*READY*");
        if (ready()) return true;
        enterCommandMode();
      }    
    } 
    return false;
  }
  else return true;  
}  

void SCKBase::APmode(char* ssid)
{
  if (enterCommandMode())
  {    
    sendCommand(F("set wlan join 7")); // Enable AP mode
    //sendCommand(F("set wlan channel <value> // Specify the channel to create network
    sendCommand(F("set wlan ssid "), true); // Set up network broadcast SSID
    sendCommand(ssid);

    buffer[6] = 0x00;
    sendCommand(F("set opt device_id "), true); // Set up network broadcast SSID
    sendCommand(ssid);

    sendCommand(F("set ip dhcp 4")); // Enable DHCP server
    sendCommand(F("set ip address 1.2.3.4")); // Specify the IP address
    sendCommand(F("set ip net 255.255.255.0")); // Specify the subnet mask
    sendCommand(F("set ip gateway 1.2.3.4")); // Specify the gateway
    sendCommand(F("save"), false, "Storing in config"); // Store settings
    sendCommand(F("reboot"), false, "*READY*"); // Reboot the module in AP mode
  }
} 

boolean SCKBase::ready()
{
  if(!enterCommandMode())
  {
    repair();
    return(false);
  }
  else
  {
    Serial1.println(F("join"));
    if (findInResponse("Associated!", 8000)) 
    {
      skipRemainderOfResponse(3000);
      exitCommandMode();
      return(true);
    }
  } 
 
}

boolean connected = false;

boolean SCKBase::open(const char *addr, int port) {

  if (connected) {
    close();
  } 
  if (enterCommandMode())
  {
    sendCommand(F("open "), true);
    sendCommand(addr, true);
    Serial1.print(F(" "));
    Serial1.print(port);
    if (sendCommand("", false, "*OPEN*")) 
    {
      connected = true;
      return true;
    }
    else return false;
  }
  enterCommandMode();
  return false;
}

boolean SCKBase::close() {
  if (!connected) {
    return true;
  }
  if (sendCommand(F("close"), false, "*CLOS*")) {
    connected = false;
    return true;
  }
  connected = false;
  return false;
}

#define MAC_ADDRESS_BUFFER_SIZE 18 // "FF:FF:FF:FF:FF:FF\0"

char* SCKBase::MAC() {
  if (enterCommandMode()) 
  {
    if (sendCommand(F("get mac"), false, "Mac Addr="))
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
      exitCommandMode();
    }        
  }

  return buffer;
}

char* SCKBase::id() {
  char* temp = MAC();  
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

uint32_t SCKBase::scan() {
  if (enterCommandMode()) 
  {
    if (sendCommand(F("scan"), false, "Found "))
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
      findInResponse("END:\r\n", 2000);
      exitCommandMode();
    }        
  }
  return atol(buffer);
} 

int SCKBase::checkWiFly() {
  int ver = getWiFlyVersion();
  if (ver > 0)
  {
    if (ver < WIFLY_LATEST_VERSION)
     {
      if(update()) return 1; //Wifly Updated.
      else return 2; //Update Fail.
      reset();
     }   
    else return 0; //WiFly up to date.
  }
  else return -1; //Error reading the wifi version.
}

int SCKBase::getWiFlyVersion() {
  if (enterCommandMode()) 
  {
    if (sendCommand(F("ver"), false, "wifly-GSX Ver"))
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
      exitCommandMode();
      buffer[offset] = 0x00;
     return atoi(buffer);
    } 
    return 0;
  }
  return 0;
}


boolean SCKBase::update() {
  if (enterCommandMode())
  {
    sendCommand(F(DEFAULT_WIFLY_FIRMWARE));
    delay(1000);
    if (findInResponse("FTP OK.", 60000))
    {
      return true;
    }
  }
  else return false;
}

uint32_t baud[7]={
  2400, 4800, 9600, 19200, 38400, 57600, 115200};

void SCKBase::repair()
{
  if(!enterCommandMode())
  {
    boolean repair = true;
    for (int i=6; ((i>=0)&&repair); i--)
    {
      Serial1.begin(baud[i]);
      if(enterCommandMode()) 
      {
        reset();
        repair = false;
      }
      Serial1.begin(9600);
    }
  }
}

/*TIMER*/

#define RESOLUTION 65536    // Timer1 is 16 bit
unsigned int pwmPeriod;
unsigned char clockSelectBits;
char oldSREG;					// To hold Status 

void SCKBase::timer1SetPeriod(long microseconds)		// AR modified for atomic access
{

  long cycles = (F_CPU / 2000000) * microseconds;                                // the counter runs backwards after TOP, interrupt is at BOTTOM so divide microseconds by 2
  if(cycles < RESOLUTION)              clockSelectBits = _BV(CS10);              // no prescale, full xtal
  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS11);              // prescale by /8
  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS11) | _BV(CS10);  // prescale by /64
  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS12);              // prescale by /256
  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS12) | _BV(CS10);  // prescale by /1024
  else        cycles = RESOLUTION - 1, clockSelectBits = _BV(CS12) | _BV(CS10);  // request was out of bounds, set as maximum

  oldSREG = SREG;				
  cli();							// Disable interrupts for 16 bit register access
  ICR1 = pwmPeriod = cycles;                                          // ICR1 is TOP in p & f correct pwm mode
  SREG = oldSREG;

  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
  TCCR1B |= clockSelectBits;                                          // reset clock select register, and starts the clock
}

void SCKBase::timer1Initialize()
{
  TCCR1A = 0;                 // clear control register A 
  TCCR1B = _BV(WGM13);        // set mode 8: phase and frequency correct pwm, stop the timer
  timer1SetPeriod(1500); 
  TIMSK1 = _BV(TOIE1);                                  
}

void SCKBase::timer1Stop()
{
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));          // clears all clock selects bits
  TIMSK1 &= ~(_BV(TOIE1)); 
  
}












