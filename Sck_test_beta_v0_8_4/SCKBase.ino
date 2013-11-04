#define redes 0
#if (redes > 0)
char* mySSID[redes]      = { "Red1"        , "Red2"        , "Red3"             };
char* myPassword[redes]  = { "Pass1"      , "Pass2"       , "Pass3"            };
char* wifiEncript[redes] = { WPA2         , WPA2          , WPA2               };
char* antennaExt[redes]  = { INT_ANT      , INT_ANT       , INT_ANT            }; //EXT_ANT
#endif


boolean connected;                  

static char buffer[buffer_length];

void sckBegin() {
  I2c.begin();
  I2c.setSpeed(fast);
  I2c.timeOut(100);
  Serial.begin(115200);
  Serial1.begin(9600);
  pinMode(IO0, OUTPUT); //VH_MICS5525
  pinMode(IO1, OUTPUT); //VH_MICS2710
  pinMode(IO2, OUTPUT); //MICS2710_ALTAIMPEDANCIA
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
    pinMode(IO4, OUTPUT); //Si7005
    digitalWrite(IO4, HIGH); //Si7005
    
    sckWriteVH(MICS_5525, 2700); //VH_MICS5525 Inicial
    digitalWrite(IO0, HIGH); //VH_MICS5525
        
    sckWriteVH(MICS_2710, 1700); //VH_MICS5525 Inicial
    digitalWrite(IO1, HIGH); //VH_MICS2710
    digitalWrite(IO2, LOW); //RADJ_MICS2710 PIN ALTA IMPEDANCIA
    
    pinMode(IO3, OUTPUT);
    digitalWrite(IO3, HIGH); //Alimentacion de los MICS
  #else
    sckWriteVH(MICS_5525, 2400); //VH_MICS5525 Inicial
    digitalWrite(IO0, HIGH); //VH_MICS5525
    
    sckWriteVH(MICS_2710, 1700); //VH_MICS5525 Inicial
    digitalWrite(IO1, HIGH); //VH_MICS2710
    digitalWrite(IO2, LOW); //RADJ_MICS2710 PIN ALTA IMPEDANCIA
  #endif
  
  sckWriteRL(MICS_5525, 100000); //Inicializacion de la carga del MICS5525
  sckWriteRL(MICS_2710, 100000); //Inicializacion de la carga del MICS2710
}

void sckConfig(){
  if (!sckCompareDate(__TIME__, sckReadData(EE_ADDR_TIME_VERSION, 0, 0)))
  {
    sckReset();
    #if debuggEnabled
      Serial.println(F("Resetting..."));
    #endif
    for(uint16_t i=0; i<DEFAULT_ADDR_MEASURES; i++) sckWriteEEPROM(i, 0x00);  //Borrado de la memoria
    
    sckWriteData(EE_ADDR_TIME_VERSION, 0, __TIME__);
    sckWriteData(EE_ADDR_TIME_UPDATE, 0, DEFAULT_TIME_UPDATE);
    
    #if (redes > 0)
      for(byte i=0; i<redes; i++)
      {
        sckWriteData(DEFAULT_ADDR_SSID, i, mySSID[i]);
        sckWriteData(DEFAULT_ADDR_PASS, i, myPassword[i]);
        sckWriteData(DEFAULT_ADDR_AUTH, i, wifiEncript[i]);
        sckWriteData(DEFAULT_ADDR_ANTENNA, i, antennaExt[i]);
      }
      sckWriteintEEPROM(EE_ADDR_NUMBER_NETS, redes);
    #endif
  }
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
  address = (address<<4)|bitRead(data, 8) ;
  I2c.write(deviceaddress,address,lowByte(data),false); //configura el dispositivo para medir temperatura
}
  
int sckReadMCP(int deviceaddress, uint16_t address ) {
  byte rdata = 0xFF;
  int  data = 0x0000;
  
  address=(address<<4)|B00001100;
  I2c.read(deviceaddress, address, 2, false); //read 2 bytes
  data = I2c.receive()<<8; 
  data = data | I2c.receive();
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
    I2c.write(E2PROM, eeaddress, data, true);
    delay(4);
    retry++;
  }
}

byte sckReadEEPROM(uint16_t eeaddress ) {
  byte data = 0xFF;
  I2c.read(E2PROM,  eeaddress, 1, true); //read 1 byte
  data = I2c.receive();
  return data;
}

void sckWriteintEEPROM(uint16_t eeaddress, uint16_t data )
{
  sckWriteEEPROM(eeaddress , highByte(data));
  sckWriteEEPROM(eeaddress  + 1, lowByte(data)); ;
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
  if (I2c.write(RTC_ADDRESS, 0x00) == 0) return true; 
  return false;
}
  
boolean sckRTCadjust(char *time) {    
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
        uint8_t DATA [7] = { rtc[5], rtc[4], rtc[3], 0x00 ,rtc[2], rtc[1], rtc[0]} ;
        I2c.write(RTC_ADDRESS, 0x00, DATA, 7); 
        I2c.write(RTC_ADDRESS, (uint16_t)0x0E, 0x00, false);   // COMMAND 
      #else
        uint8_t DATA [8] = { rtc[5], rtc[4], rtc[3], 0x00 ,rtc[2], rtc[1], rtc[0], 0x00 } ;
        I2c.write(RTC_ADDRESS, 0x00, DATA, 8);   // COMMAND
      #endif
      return true;
    }
    return false;  
}

char* sckRTCtime() {
      I2c.read(RTC_ADDRESS, (uint16_t)0x00, 7, false); //read 7 bytes
      uint8_t seconds = (I2c.receive() & 0x7F);
      uint8_t minutes = I2c.receive();
      uint8_t hours = I2c.receive();
      I2c.receive();
      uint8_t day = I2c.receive();
      uint8_t month = I2c.receive();
      uint8_t year = I2c.receive(); 
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
                Serial.print(auth);
                ssid = sckReadData(DEFAULT_ADDR_SSID, nets, 0);
                sckSendCommand(F("set wlan ssid "), true);
                sckSendCommand(ssid);
                Serial.print(F(" "));
                Serial.print(ssid);
                pass = sckReadData(DEFAULT_ADDR_PASS, nets, 0);
                if (mode) sckSendCommand(F("set wlan phrase "), true);  // WPA1, WPA2, OPEN
                else sckSendCommand(F("set wlan key "), true);
                sckSendCommand(pass);
                Serial.print(F(" "));
                Serial.print(pass);
                antenna = sckReadData(DEFAULT_ADDR_ANTENNA, nets, 0);
                sckSendCommand(F("set wlan ext_antenna "), true);
                sckSendCommand(antenna);
                Serial.print(F(" "));
                Serial.println(antenna);
                sckSendCommand(F("save"), false, "Storing in config"); // Store settings
                sckSendCommand(F("reboot"), false, "*READY*");
                if (sckReady()) return true;
                sckEnterCommandMode();
             }
          return false;     
        } 
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
          sckSendCommand(F("set ip dhcp 4")); // Enable DHCP server
          sckSendCommand(F("set ip address 192.168.0.1")); // Specify the IP address
          sckSendCommand(F("set ip net 255.255.255.0")); // Specify the subnet mask
          sckSendCommand(F("set ip gateway 192.168.0.1")); // Specify the gateway
          sckSendCommand(F("save"), false, "Storing in config"); // Store settings
          sckSendCommand(F("reboot"), false, "*READY*"); // Reboot the module in AP mode
    }
  } 
  
boolean sckReady()
{
  if(!sckEnterCommandMode())
    {
      Serial1.begin(115200);
      if(sckEnterCommandMode()) sckReset();
      Serial1.begin(9600);
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
    if (temp[i] != ':') 
      {
        buffer[j] = temp[i];
        j++;
      }
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
  


