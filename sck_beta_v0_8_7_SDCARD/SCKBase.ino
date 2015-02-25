boolean connected;                  

#define buffer_length        32
static char buffer[buffer_length];

#define TWI_FREQ 400000L //Frecuencia bus I2C

void sckBegin() {
  Wire.begin();
  TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;  
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
  digitalWrite(AWAKE, HIGH); 
  digitalWrite(FACTORY, LOW); 
#if ((decouplerComp)&&(F_CPU > 8000000 ))
  decoupler.setup();
#endif
#if F_CPU == 8000000 
  sckWriteCharge(350);

  sckWriteVH(MICS_5525, 2700); //VH_MICS5525 Inicial
  digitalWrite(IO0, HIGH); //VH_MICS5525

  sckWriteVH(MICS_2710, 1700); //VH_MICS5525 Inicial
  digitalWrite(IO1, HIGH); //VH_MICS2710
  digitalWrite(IO2, LOW); //RADJ_MICS2710 PIN ALTA IMPEDANCIA

  pinMode(IO3, OUTPUT);
  digitalWrite(IO3, HIGH); //Alimentacion de los MICS
  
  #if ADXLEnabled
    sckWriteADXL(0x2D, 0x08);
    //  sckWriteADXL(0x31, 0x00); //2g
    //  sckWriteADXL(0x31, 0x01); //4g
    sckWriteADXL(0x31, 0x02); //8g
    //  sckWriteADXL(0x31, 0x03); //16g
  #endif

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
    sckRTCadjust(sckDate(__DATE__,__TIME__));
#if debuggEnabled
    Serial.println(F("Resetting..."));
#endif
    for(uint16_t i=0; i<60; i++) sckWriteEEPROM(i, 0x00);  //Borrado de la memoria
    sckWriteData(EE_ADDR_TIME_VERSION, 0, __TIME__);
    sckWriteData(EE_ADDR_TIME_UPDATE, 0, DEFAULT_TIME_UPDATE);
    sckWriteData(EE_ADDR_NUMBER_UPDATES, 0, DEFAULT_MIN_UPDATES);
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
    EEPROM.write(eeaddress, data);
    delay(6);
    retry++;
  }
}

byte sckReadEEPROM(uint16_t eeaddress ) {
  return  EEPROM.read(eeaddress);
}


void sckWriteintEEPROM(uint16_t eeaddress, uint16_t data )
{
  sckWriteEEPROM(eeaddress , highByte(data));
  sckWriteEEPROM(eeaddress  + 1, lowByte(data)); 
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


char* sckDate(const char* date, const char* time){
    int j = 0;
    for  (int i = 7; date[i]!=0x00; i++)
      {
        buffer[j] = date[i];
        j++;
      }
    buffer[j] = '-';
    j++;
    // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec 
    switch (date[0]) {
        case 'J': 
            if (date[1] == 'a') buffer[j] = '1';
            else if (date[2] == 'n') buffer[j] = '6';
            else buffer[j] = '7';
            break; 
        case 'F': 
            buffer[j] = '2'; 
            break;
        case 'A': 
            if (date[1] == 'p') buffer[j] = '4';
            else buffer[j] = '8';
            break;
        case 'M': 
            if (date[2] == 'r') buffer[j] = '3';
            else buffer[j] = '5';
            break;
        case 'S': 
            buffer[j] = '9'; 
            break;
        case 'O': 
            buffer[j] = '1'; 
            buffer[j+1] = '0';
            j++;
            break;
        case 'N': 
            buffer[j] = '1'; 
            buffer[j+1] = '1';
            j++;
            break;
        case 'D': 
            buffer[j] = '1'; 
            buffer[j+1] = '2';
            j++;
            break;
    }
  j++;
  buffer[j] = '-';
  j++;
  for  (int i = 4; date[i]!=' '; i++)
      {
        buffer[j] = date[i];
        j++;
      }
  buffer[j] = ' ';
  j++;
  for  (int i = 0; time[i]!=0x00; i++)
    {
      buffer[j] = time[i];
      j++;
    }
  buffer[j]=0x00;   
  return buffer;
}
  
boolean sckRTCadjust(char *time) {    
  byte rtc[6] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00               
    };
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

#define COMMAND_MODE_GUARD_TIME 250 // in milliseconds

boolean sckSleep() {
  delay(COMMAND_MODE_GUARD_TIME);
  Serial1.print(F("$$$"));
  delay(COMMAND_MODE_GUARD_TIME);
  Serial1.println();
  Serial1.println();
  Serial1.println(F("sleep"));
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

















