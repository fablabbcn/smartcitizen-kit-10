#ifndef SMARTCITIZENAMBIENT_cpp
#define SMARTCITIZENAMBIENT_cpp

#include "SmartCitizenAmbient.h"

#define redes 0
#if (redes > 0)
  char* mySSID[redes]      = { "Red1"       , "Red2"        , "Red3"             };
  char* myPassword[redes]  = { "Pass1"      , "Pass2"       , "Pass3"            };
  char* wifiEncript[redes] = { WPA2         , WPA2          , WPA2               };
  char* antennaExt[redes]  = { INT_ANT      , INT_ANT       , INT_ANT            }; //EXT_ANT
#endif

char* WEB[10]={
                  "data.smartcitizen.me",
                  "PUT /add HTTP/1.1 \n", 
                  "Host: data.smartcitizen.me \n", 
                  "User-Agent: SmartCitizen \n", 
                  "X-SmartCitizenMacADDR: ", 
                  " \n", 
                  "X-SmartCitizenData: ",
                  /*Servidor de tiempo*/
                  "GET /datetime HTTP/1.1 \n",
                  "Host: data.smartcitizen.me \n",
                  "User-Agent: SmartCitizen \n\n"  
                  };
                  
char* SERVER[11]={
                  "{\"temp\":\"",
                  "\",\"hum\":\"", 
                  "\",\"light\":\"",
                  "\",\"bat\":\"",
                  "\",\"panel\":\"",
                  "\",\"co\":\"", 
                  "\",\"no2\":\"", 
                  "\",\"noise\":\"", 
                  "\",\"nets\":\"", 
                  "\",\"timestamp\":\"", 
                  "\"}"
                  };
                  
float Rs0 = 0;
float Rs1 = 0;

#define RES 256   //Resolucion de los potenciometros digitales

#if F_CPU == 8000000 
  #define R1  12    //Kohm
#else
  #define R1  82    //Kohm
#endif

#define P1  100   //Kohm 

float k= (RES*(float)R1/100)/1000;  //Constante de conversion a tension de los reguladores 
float kr= ((float)P1*1000)/RES;     //Constante de conversion a resistencia de potenciometrosen ohmios


static char buffer[buffer_length];

void SmartCitizen::begin() {
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
  digitalWrite(IO0, LOW); //VH_MICS5525
  digitalWrite(IO1, LOW); //VH_MICS2710
  digitalWrite(IO2, LOW); //RADJ_MICS2710
  digitalWrite(AWAKE, LOW); 
  
  #if F_CPU == 8000000 
    pinMode(IO4, OUTPUT); //Si7005
    //digitalWrite(IO4, LOW); //Si7005
    digitalWrite(IO4, HIGH); //Si7005
  #endif
}

void SmartCitizen::config(){
  if (!compareDate(__TIME__, readData(EE_ADDR_TIME_VERSION, 0, 0)))
  {
    reset();
    #if debuggEnabled
      Serial.println("Reseteando...");
    #endif
    for(uint16_t i=0; i<DEFAULT_ADDR_MEASURES; i++) writeEEPROM(i, 0x00);  //Borrado de la memoria
    
    writeData(EE_ADDR_TIME_VERSION, 0, __TIME__);
    writeData(EE_ADDR_TIME_UPDATE, 0, DEFAULT_TIME_UPDATE);
    
    #if (redes > 0)
      for(byte i=0; i<redes; i++)
      {
        writeData(DEFAULT_ADDR_SSID, i, mySSID[i]);
        writeData(DEFAULT_ADDR_PASS, i, myPassword[i]);
        writeData(DEFAULT_ADDR_AUTH, i, wifiEncript[i]);
        writeData(DEFAULT_ADDR_ANTENNA, i, antennaExt[i]);
      }
      writeintEEPROM(EE_ADDR_NUMBER_NETS, redes);
    #endif
  }
}

float SmartCitizen::average(int anaPin) {
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

boolean SmartCitizen::checkText(byte inByte, char* text, byte *check)
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

boolean SmartCitizen::compareDate(char* text, char* text1)
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
  
void SmartCitizen::checkData()
  {  
     uint32_t check_measures = readintEEPROM(EE_ADDR_NUMBER_MEASURES);
      if (check_measures > 8)
       {
         if (((check_measures + 1)%10) != 0) check_measures = 0;
       }
       else check_measures = 0;
     writeintEEPROM(EE_ADDR_NUMBER_MEASURES, check_measures); 
  }
    


#define TIMEOUT 10000

boolean SmartCitizen::DHT22(uint8_t pin)
{
        // READ VALUES
        int rv = dhtRead(pin);
        if (rv != true)
        {
              _lastHumidity    = DHTLIB_INVALID_VALUE;  // invalid value, or is NaN prefered?
              _lastTemperature = DHTLIB_INVALID_VALUE;  // invalid value
              return rv;
        }

        // CONVERT AND STORE
        _lastHumidity    = word(bits[0], bits[1]);

        if (bits[2] & 0x80) // negative temperature
        {
            _lastTemperature = word(bits[2]&0x7F, bits[3]);
            _lastTemperature *= -1.0;
        }
        else
        {
            _lastTemperature = word(bits[2], bits[3]);
        }

        // TEST CHECKSUM
        uint8_t sum = bits[0] + bits[1] + bits[2] + bits[3];
        if (bits[4] != sum) return false;
        if ((_lastTemperature == 0)&&(_lastHumidity == 0))return false;
        return true;
}

boolean SmartCitizen::dhtRead(uint8_t pin)
{
        // INIT BUFFERVAR TO RECEIVE DATA
        uint8_t cnt = 7;
        uint8_t idx = 0;

        // EMPTY BUFFER
        for (int i=0; i< 5; i++) bits[i] = 0;

        // REQUEST SAMPLE
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
        delay(20);
        digitalWrite(pin, HIGH);
        delayMicroseconds(40);
        pinMode(pin, INPUT);

        // GET ACKNOWLEDGE or TIMEOUT
        unsigned int loopCnt = TIMEOUT;
        while(digitalRead(pin) == LOW)
                if (loopCnt-- == 0) return false;

        loopCnt = TIMEOUT;
        while(digitalRead(pin) == HIGH)
                if (loopCnt-- == 0) return false;

        // READ THE OUTPUT - 40 BITS => 5 BYTES
        for (int i=0; i<40; i++)
        {
                loopCnt = TIMEOUT;
                while(digitalRead(pin) == LOW)
                        if (loopCnt-- == 0) return false;

                unsigned long t = micros();

                loopCnt = TIMEOUT;
                while(digitalRead(pin) == HIGH)
                        if (loopCnt-- == 0) return false;

                if ((micros() - t) > 40) bits[idx] |= (1 << cnt);
                if (cnt == 0)   // next byte?
                {
                        cnt = 7;  
                        idx++;      
                }
                else cnt--;
        }

        return true;
}

int SmartCitizen::getTemperatureC(){
    return _lastTemperature;
}

int SmartCitizen::getHumidity(){
    return _lastHumidity;
}
  
void SmartCitizen::writeMCP(byte deviceaddress, byte address, int data ) {
  if (data>RES) data=RES;
  address = (address<<4)|bitRead(data, 8) ;
  I2c.write(deviceaddress,address,lowByte(data),false); //configura el dispositivo para medir temperatura
}
  
int SmartCitizen::readMCP(int deviceaddress, uint16_t address ) {
  byte rdata = 0xFF;
  int  data = 0x0000;
  
  address=(address<<4)|B00001100;
  I2c.read(deviceaddress, address, 2, false); //read 2 bytes
  data = I2c.receive()<<8; 
  data = data | I2c.receive();
  return data;
}
  
void SmartCitizen::writeVH(byte device, long voltage ) {
  int data=0;
  
  #if F_CPU == 8000000 
    int temp = (int)(((voltage/0.41)-1000)*k);
  #else
    int temp = (int)(((voltage/1.2)-1000)*k);
  #endif

  if (temp>RES) data = RES;
  else if (temp<0) data=0;
  else data = temp;
  #if F_CPU == 8000000 
    writeMCP(MCP1, device, data);
  #else
    writeMCP(MCP2, device, data);
  #endif
}
  
float SmartCitizen::readVH(byte device) {
  int data;
  #if F_CPU == 8000000 
    data=readMCP(MCP1, device);
    float voltage = (data/k + 1000)*0.41;
  #else
    data=readMCP(MCP2, device);
    float voltage = (data/k + 1000)*1.2;
  #endif
  
  return(voltage);
}
  
void SmartCitizen::writeRL(byte device, long resistor) {
  int data=0x00;
  data = (int)(resistor/kr);
  #if F_CPU == 8000000 
    writeMCP(MCP1, device + 6, data);
  #else
    writeMCP(MCP1, device, data);
  #endif
}

float SmartCitizen::readRL(byte device)
{
  #if F_CPU == 8000000 
    return (kr*readMCP(MCP1, device + 6)); //Devuelve en Ohms
  #else
    return (kr*readMCP(MCP1, device));  //Devuelve en Ohms
  #endif 
}

void SmartCitizen::writeRGAIN(byte device, long resistor) {
  int data=0x00;
  data = (int)(resistor/kr);
  writeMCP(MCP2, device, data);
}

float SmartCitizen::readRGAIN(byte device)
{
    return (kr*readMCP(MCP2, device));  //Devuelve en Ohms
}

  
void SmartCitizen::writeEEPROM(uint16_t eeaddress, uint8_t data ) {
  uint8_t retry = 0;
  while ((readEEPROM(eeaddress)!=data)&&(retry<10))
  {  
    I2c.write(E2PROM, eeaddress, data, true);
    delay(4);
    retry++;
  }
}

byte SmartCitizen::readEEPROM(uint16_t eeaddress ) {
  byte data = 0xFF;
  I2c.read(E2PROM,  eeaddress, 1, true); //read 1 byte
  data = I2c.receive();
  return data;
}

void SmartCitizen::writeintEEPROM(uint16_t eeaddress, uint16_t data )
{
  writeEEPROM(eeaddress , highByte(data));
  writeEEPROM(eeaddress  + 1, lowByte(data)); ;
}

uint16_t SmartCitizen::readintEEPROM(uint16_t eeaddress)
{
  return (readEEPROM(eeaddress)<<8)+ readEEPROM(eeaddress + 1); 
}

char* SmartCitizen::readData(uint16_t eeaddress, uint16_t pos, uint8_t dec)
{
  eeaddress = eeaddress + buffer_length * pos;
  uint8_t temp = readEEPROM(eeaddress);
  uint16_t i;
  for ( i = eeaddress; ((temp!= 0x00)&&(temp<0x7E)&&(temp>0x1F)&&((i - eeaddress)<buffer_length)); i++) 
  {
       buffer[i - eeaddress] = readEEPROM(i);
       temp = readEEPROM(i + 1);
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


void SmartCitizen::writeData(uint16_t eeaddress, uint16_t pos, char* text )
{
  uint16_t eeaddressfree = eeaddress + buffer_length * pos;
  for (uint16_t i = eeaddressfree; i< (eeaddressfree + buffer_length); i++) writeEEPROM(i, 0x00);
  for (uint16_t i = eeaddressfree; text[i - eeaddressfree]!= 0x00; i++) writeEEPROM(i, text[i - eeaddressfree]);
  if (eeaddress == DEFAULT_ADDR_MEASURES) writeintEEPROM(EE_ADDR_NUMBER_MEASURES, pos);
}

boolean SmartCitizen::checkRTC() {
  if (I2c.write(RTC_ADDRESS, 0x00) == 0) return true; 
  return false;
}
  
boolean SmartCitizen::RTCadjust(char *time) {    
  
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
          uint8_t DATA [8] = { rtc[5] | 0x80, rtc[4], rtc[3], 0x00 ,rtc[2], rtc[1], rtc[0], 0x00 } ;
      #else
          uint8_t DATA [8] = { rtc[5], rtc[4], rtc[3], 0x00 ,rtc[2], rtc[1], rtc[0], 0x00 } ;
      #endif
      I2c.write(RTC_ADDRESS, 0x00, DATA, 8);   // COMMAND
      return true;
    }
    return false;   
}

char* SmartCitizen::RTCtime() {
      I2c.read(RTC_ADDRESS, (uint16_t)0x00, 7, false); //read 4 bytes
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

void SmartCitizen::heat(byte device, int current)
  {
    float Rc=Rc0;
    byte Sensor = S2;
    if (device == MICS_2710) { Rc=Rc1; Sensor = S3;}
    float Vc = (float)average(Sensor)*Vcc/1023; //mV 
    float current_measure = Vc/Rc; //mA 
    float Rh = (readVH(device)- Vc)/current_measure;
    float Vh = (Rh + Rc)*current;
    writeVH(device, Vh);
      #if debuggSCK
        if (device == MICS_2710) Serial.print("MICS2710 corriente: ");
        else Serial.print("MICS5525 corriente: ");
        Serial.print(current_measure);
        Serial.println(" mA");
        if (device == MICS_2710) Serial.print("MICS2710 correccion VH: ");
        else  Serial.print("MICS5525 correccion VH: ");
        Serial.print(readVH(device));
        Serial.println(" mV");
        Vc = (float)average(Sensor)*Vcc/1023; //mV 
        current_measure = Vc/Rc; //mA 
        if (device == MICS_2710) Serial.print("MICS2710 corriente corregida: ");
        else Serial.print("MICS5525 corriente corregida: ");
        Serial.print(current_measure);
        Serial.println(" mA");
        Serial.println("Heating...");
      #endif
    
  }

  float SmartCitizen::readMICS(byte device, unsigned long time)
  {
      byte Sensor = S0;
      float VMICS = VMIC0;
      if (device == MICS_2710) {Sensor = S1; VMICS = VMIC1;}
      delay(time); //Tiempo de enfriamiento para lectura
      float RL = readRL(device); //Ohm
      float VL = ((float)average(Sensor)*Vcc)/1023; //mV
      float Rs = ((VMICS-VL)/VL)*RL; //Ohm
      /*Correccion de impedancia de carga*/
      if (Rs < 100000)
      {
        writeRL(device, Rs);
        delay(100);
        RL = readRL(device); //Ohm
        VL = ((float)average(Sensor)*Vcc)/1023; //mV
        Rs = ((VMICS-VL)/VL)*RL; //Ohm
      }
      
      #if debuggSCK
        if (device == MICS_5525) Serial.print("MICS5525 Rs: ");
        else Serial.print("MICS2710 Rs: ");
        Serial.print(Rs);
        Serial.println(" Ohm");
      #endif;  
       return Rs;
  }
  
void SmartCitizen::getMICS(unsigned long time0, unsigned long time1){          
     
      /*Correccion de la tension del Heather*/
      
      #if F_CPU == 8000000 
        writeVH(MICS_5525, 2700); //VH_MICS5525 Inicial
        digitalWrite(IO0, HIGH); //VH_MICS5525
        
        writeVH(MICS_2710, 2700); //VH_MICS5525 Inicial
        digitalWrite(IO1, HIGH); //VH_MICS2710
        digitalWrite(IO2, LOW); //RADJ_MICS2710 PIN ALTA IMPEDANCIA
        
        digitalWrite(IO3, HIGH); //Alimentacion de los MICS
        #if debuggSCK
          Serial.println("*******************");
          Serial.println("MICS5525 VH a 2700 mV");
          Serial.println("MICS2710 VH a 2700 mV");
        #endif
      #else
        writeVH(MICS_5525, 2400); //VH_MICS5525 Inicial
        digitalWrite(IO0, HIGH); //VH_MICS5525
        
        writeVH(MICS_2710, 1700); //VH_MICS5525 Inicial
        digitalWrite(IO1, HIGH); //VH_MICS2710
        digitalWrite(IO2, LOW); //RADJ_MICS2710 PIN ALTA IMPEDANCIA
        #if debuggSCK
          Serial.println("*******************");
          Serial.println("MICS5525 VH a 2400 mV");
          Serial.println("MICS2710 VH a 1700 mV");
        #endif
      #endif
      
      

      
      delay(200);  // Tiempo estabilizacion de la alimentacion
      heat(MICS_5525, 32); //Corriente en mA
      heat(MICS_2710, 26); //Corriente en mA
      delay(5000); // Tiempo de heater!
      
      writeRL(MICS_5525, 100000); //Inicializacion de la carga del MICS5525
      writeRL(MICS_2710, 100000); //Inicializacion de la carga del MICS2710
      
//      #if F_CPU == 16000000 
//        /*Lectura de datos*/
//        digitalWrite(IO0, LOW);  //VH_MICS5525 OFF para lectura
//        
//        #if debuggSCK
//          Serial.println("MICS5525 VH OFF ");
//        #endif
//      #endif
      
      Rs0 = readMICS(MICS_5525, time0);
      Rs1 = readMICS(MICS_2710, time1 - time0);
      
//      #if F_CPU == 16000000
//        digitalWrite(IO1, LOW); //VH MICS2710 OFF
//        #if debuggSCK
//          Serial.println("MICS2710 VH OFF ");
//          Serial.println("*******************");
//        #endif
//      #endif    
}
  
  
  uint16_t SmartCitizen::getPanel(){
    uint16_t value = 3*average(PANEL)*Vcc/1023;
    if (value > 500) value = value + 750; //Tension del diodo de proteccion
    return value;
  }
  
  uint16_t SmartCitizen::getBattery() {
    uint16_t temp = average(BAT);
    temp = map(temp, VAL_MIN_BATTERY, VAL_MAX_BATTERY, 0, 1000);
    if (temp>1000) temp=1000;
    if (temp<0) temp=0;
    return temp; 
  }
 
  
 #if F_CPU == 8000000
 
   uint16_t SmartCitizen::readSi7005(uint8_t type){
      uint16_t DATA = 1;
      I2c.write(Si7005,(uint16_t)0x03,type, false); //configura el dispositivo para medir
      delay(15);
      while (DATA&0x0001 == 0x0001)
            {
              I2c.read(Si7005,(uint16_t)0x00,1, false); //read 1 byte
              DATA = I2c.receive();      
            }
      I2c.read(Si7005,(uint16_t)0x01,2, false); //read 2 bytes
      DATA = I2c.receive()<<8;
      if (type == 0x11) DATA = (DATA|I2c.receive())>>2;
      else if (type == 0x01) DATA = (DATA|I2c.receive())>>4;
      return DATA;
  }
  
   void SmartCitizen::getSi7005(){
      digitalWrite(IO4, LOW); //Si7005
      delay(15);
            
      _lastTemperature = (((float)readSi7005(0x11)/32)-50)*10;
      _lastHumidity    = (((float)readSi7005(0x01)/16)-24)*10;  

      #if debuggSCK
        Serial.print("Temperatura: ");
        Serial.print(_lastTemperature/10.);
        Serial.print(" C, Humedad: ");
        Serial.print(_lastHumidity/10.);
        Serial.println(" %");   
      #endif
      
      digitalWrite(IO4, HIGH); //Si7005
  }
 #endif
  
  uint16_t SmartCitizen::getLight(){
    #if F_CPU == 8000000 
      uint8_t TIME0  = 0xDA;
      uint8_t GAIN0 = 0x00;
      uint8_t DATA [8] = {0x03, TIME0, 0x00 ,0x00, 0x00, 0xFF, 0xFF ,GAIN0} ;
      
      uint16_t DATA0 = 0;
      uint16_t DATA1 = 0;
      
      I2c.write(bh1730,0x80|0x00, DATA, 8);   // COMMAND
          
      I2c.read(bh1730, (uint16_t)0x94, 4, false); //read 4 bytes
      DATA0 = I2c.receive();
      DATA0=DATA0|(I2c.receive()<<8);
      DATA1 = I2c.receive();
      DATA1=DATA1|(I2c.receive()<<8);   
      
      uint8_t Gain = 0x00; 
      if (GAIN0 == 0x00) Gain = 1;
      else if (GAIN0 == 0x01) Gain = 2;
      else if (GAIN0 == 0x02) Gain = 64;
      else if (GAIN0 == 0x03) Gain = 128;
      
      float ITIME =  (256- TIME0)*2.7;
      
      float Lx = 0;
      float cons = (Gain * 100) / ITIME;
      float comp = (float)DATA1/DATA0;
      if (comp<0.26) Lx = ( 1.290*DATA0 - 2.733*DATA1 ) / cons;
      else if (comp < 0.55) Lx = ( 0.795*DATA0 - 0.859*DATA1 ) / cons;
      else if (comp < 1.09) Lx = ( 0.510*DATA0 - 0.345*DATA1 ) / cons;
      else if (comp < 2.13) Lx = ( 0.276*DATA0 - 0.130*DATA1 ) / cons;
      else Lx=0;
      
       #if debuggSCK
        Serial.print(DATA0);
        Serial.print(' ');
        Serial.print(DATA1);
        Serial.print(' ');
        Serial.print(comp);
        Serial.print(' ');
        Serial.print(Gain);
        Serial.print(' ');
        Serial.print(ITIME);
        Serial.print(' ');
        Serial.print(cons);
        Serial.print(' ');
        Serial.println(Lx);
      #endif
     return Lx*10;
    #else
      int temp = map(average(S5), 0, 1023, 0, 1000);
      if (temp>1000) temp=1000;
      if (temp<0) temp=0;
      return temp;
    #endif
  }
  
  unsigned int SmartCitizen::getNoise() {
    unsigned long temp = 0;
    int n = 200;
    
    #if F_CPU == 8000000 
     writeRGAIN(0x00, 100000);
     writeRGAIN(0x01, 100000);
      #if debuggSCK
        Serial.print("R0 = ");
        Serial.print(readRGAIN(0x00));
        Serial.print(" R1 = ");
        Serial.println(readRGAIN(0x01));
        delay(200);
      #endif
    #else
     digitalWrite(IO1, HIGH); //VH_MICS2710
     delay(500); // LE DAMOS TIEMPO A LA FUENTE Y QUE DESAPAREZCA EL TRANSITORIO
    #endif
    

  // 
    float mVRaw = 0; 
    for (int i=0; i<n; i++)
    {
      delay(1);
      //temp_actual = analogRead(S4);
      mVRaw = (float)((analogRead(S4))/1023.)*Vcc;
      
      //dB_temp = 16.801*log( (mVRaw*200)/1000. ) + 9.872;
      float dB_temp = 9.7*log( (mVRaw*200)/1000. ) + 40;  // calibracion para ruido rosa // energia constante por octava
  
      temp = temp + dB_temp;
  
    }
  //  
    float dB = (float)temp/n;
    if(dB < 50) dB = 50; // minimo con la resolucion actual!
    
    //mVRaw = (float)((float)(average(S4))/1023)*Vcc;
    #if debuggSCK
      Serial.print("nOISE mV = ");
      Serial.print(mVRaw);
      Serial.print("  -  nOISE dB = ");
      Serial.println(dB);
    #endif

    #if F_CPU > 8000000 
      digitalWrite(IO1, LOW); //VH_MICS2710
    #endif
    
    
    //return max_mVRaw;
    return dB*100;
    
  }
 
  unsigned long SmartCitizen::getCO()
  {
    return Rs0;
  }  
  
  unsigned long SmartCitizen::getNO2()
  {
    return Rs1;
  } 
 
 
 void SmartCitizen::updateSensors(byte mode) 
 {   
  checkData();
  uint16_t pos = readintEEPROM(EE_ADDR_NUMBER_MEASURES);
  uint16_t MAX = 800;
  if ((mode == 2)||(mode == 4)||(pos >= MAX)) 
    {
      writeintEEPROM(EE_ADDR_NUMBER_MEASURES, 0x0000);
      pos = 0;
    }  
  boolean ok_read = false; 
  byte    retry   = 0;
  
  if (pos > 0) pos = pos + 1;
  
  #if F_CPU == 8000000 
    getSi7005();
    ok_read = true;
  #else
    Timer1.stop();
    while ((!ok_read)&&(retry<5))
    {
      ok_read = DHT22(IO3);
      retry++; 
      if (!ok_read){ /*Serial.println("FAIL!");*/ delay(3000);}
      //else Serial.println("OK!");
    }
    Timer1.initialize(500); // set a timer of length 1000000 microseconds (or 1 sec - or 1Hz)
  #endif
    if (ok_read )  
    {
      writeData(DEFAULT_ADDR_MEASURES, pos + 0, itoa(getTemperatureC())); // C
      writeData(DEFAULT_ADDR_MEASURES, pos + 1, itoa(getHumidity())); // %   
    }
    else 
    {
      writeData(DEFAULT_ADDR_MEASURES, pos + 0, readData(DEFAULT_ADDR_MEASURES, 0, 0)); // C
      writeData(DEFAULT_ADDR_MEASURES, pos + 1, readData(DEFAULT_ADDR_MEASURES, 1, 0)); // %
    }  
  writeData(DEFAULT_ADDR_MEASURES, pos + 2, itoa(getLight())); //mV
  writeData(DEFAULT_ADDR_MEASURES, pos + 3, itoa(getBattery())); //%
  writeData(DEFAULT_ADDR_MEASURES, pos + 4, itoa(getPanel()));  // %
  
  if ((mode == 3)||(mode == 4))
    { 
       writeData(DEFAULT_ADDR_MEASURES, pos + 5, "0"); //ppm
       writeData(DEFAULT_ADDR_MEASURES, pos + 6, "0"); //ppm
    }
  else
    {
      getMICS(4000, 30000);
      writeData(DEFAULT_ADDR_MEASURES, pos + 5, itoa(getCO())); //ppm
      writeData(DEFAULT_ADDR_MEASURES, pos + 6, itoa(getNO2())); //ppm
    }
    
  writeData(DEFAULT_ADDR_MEASURES, pos + 7, itoa(getNoise())); //mV
      
  if ((mode == 0)||(mode == 2)||(mode == 4))
       {
         writeData(DEFAULT_ADDR_MEASURES, pos + 8, "0");  //Wifi Nets
         writeData(DEFAULT_ADDR_MEASURES, pos + 9, RTCtime());
       } 
}
  
  
 boolean SmartCitizen::findInResponse(const char *toMatch,
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

void SmartCitizen::skipRemainderOfResponse(unsigned int timeOut) {
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

boolean SmartCitizen::sendCommand(const __FlashStringHelper *command,
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

boolean SmartCitizen::sendCommand(const char *command,
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

boolean SmartCitizen::enterCommandMode() {
    for (int retryCount = 0; retryCount < COMMAND_MODE_ENTER_RETRY_ATTEMPTS; retryCount++) 
     {
      delay(COMMAND_MODE_GUARD_TIME);
      Serial1.print("$$$");
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


boolean SmartCitizen::sleep() {
      enterCommandMode();
      sendCommand(F("sleep"));
}

boolean SmartCitizen::reset() {
      enterCommandMode();
      sendCommand(F("factory R"), false, "Set Factory Defaults"); // Store settings
      sendCommand(F("save"), false, "Storing in config"); // Store settings
      sendCommand(F("reboot"), false, "*READY*");
}


boolean SmartCitizen::exitCommandMode() {
    for (int retryCount = 0; retryCount < COMMAND_MODE_ENTER_RETRY_ATTEMPTS; retryCount++) 
     {
      if (sendCommand(F("exit"), false, "EXIT")) 
      {
      return true;
      }
    }
    return false;
}

boolean SmartCitizen::connect()
  {
    if (!ready())
    {
      if (readintEEPROM(EE_ADDR_NUMBER_NETS)<1) return false;
      if(enterCommandMode())
        {    
            sendCommand(F("set wlan join 1")); // Disable AP mode
            sendCommand(F("set ip dhcp 1")); // Enable DHCP server
            sendCommand(F("set ip proto 10")); //Modo TCP y modo HTML
            char* auth;
            char* ssid;
            char* pass;
            char* antenna;
            for (uint16_t nets = 0  ; nets < readintEEPROM(EE_ADDR_NUMBER_NETS); nets++) {
                auth = readData(DEFAULT_ADDR_AUTH, nets, 0);
                sendCommand(F("set wlan auth "), true);
                sendCommand(auth);
                boolean mode = true;
                if ((auth==WEP)||(auth==WEP64)) mode=false;
                Serial.print(auth);
                ssid = readData(DEFAULT_ADDR_SSID, nets, 0);
                sendCommand(F("set wlan ssid "), true);
                sendCommand(ssid);
                Serial.print(" ");
                Serial.print(ssid);
                pass = readData(DEFAULT_ADDR_PASS, nets, 0);
                if (mode) sendCommand(F("set wlan phrase "), true);  // WPA1, WPA2, OPEN
                else sendCommand(F("set wlan key "), true);
                sendCommand(pass);
                Serial.print(" ");
                Serial.print(pass);
                antenna = readData(DEFAULT_ADDR_ANTENNA, nets, 0);
                sendCommand(F("set wlan ext_antenna "), true);
                sendCommand(antenna);
                Serial.print(" ");
                Serial.println(antenna);
                sendCommand(F("save"), false, "Storing in config"); // Store settings
                sendCommand(F("reboot"), false, "*READY*");
                if (ready()) return true;
                enterCommandMode();
             }
          return false;     
        } 
    }
   else return true;  
  }  

void SmartCitizen::APmode(char* ssid)
  {
    if (enterCommandMode())
    {    
          sendCommand(F("set wlan join 7")); // Enable AP mode
          //sendCommand(F("set wlan channel <value> // Specify the channel to create network
          sendCommand(F("set wlan ssid "), true); // Set up network broadcast SSID
          sendCommand(ssid);
          sendCommand(F("set ip dhcp 4")); // Enable DHCP server
          sendCommand(F("set ip address 192.168.0.1")); // Specify the IP address
          sendCommand(F("set ip net 255.255.255.0")); // Specify the subnet mask
          sendCommand(F("set ip gateway 192.168.0.1")); // Specify the gateway
          sendCommand(F("save"), false, "Storing in config"); // Store settings
          sendCommand(F("reboot"), false, "*READY*"); // Reboot the module in AP mode
    }
  } 
  
boolean SmartCitizen::ready()
{
  if(!enterCommandMode())
    {
      Serial1.begin(115200);
      if(enterCommandMode()) reset();
      Serial1.begin(9600);
    }
  if (enterCommandMode())
    {
      Serial1.println("join");
      if (findInResponse("Associated!", 8000)) 
      {
        skipRemainderOfResponse(3000);
        exitCommandMode();
        return(true);
      }
   } 
  else return(false);
}

boolean SmartCitizen::open(const char *addr, int port) {
  
  if (connected) {
	close();
    } 
  if (enterCommandMode())
    {
      sendCommand(F("open "), true);
      sendCommand(addr, true);
      Serial1.print(" ");
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


boolean SmartCitizen::isConnected()
{
    return connected;
}


boolean SmartCitizen::close() {
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

char* SmartCitizen::mac() {
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

char* SmartCitizen::id() {
  char* temp = mac();  
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

#define TIME_BUFFER_SIZE 20 

char* SmartCitizen::WIFItime() {
  boolean ok=false;
  uint8_t count = 0;
  if (enterCommandMode()) 
  {
    byte retry=0;
    while ((!open(WEB[0], 80))&&(retry<5)) 
      {
        retry++; //Serial.println("Retry!!");
      }
    if(retry<5)
    {
      for(byte i = 7; i<10; i++) Serial1.print(WEB[i]); //Peticiones al servidor de tiempo
      if (findInResponse("UTC:", 2000)) 
      {
        char newChar;
        byte offset = 0;
  
        while (offset < TIME_BUFFER_SIZE) {
          if (Serial1.available())
          {
            newChar = Serial1.read();
            if (newChar == '#') {
              ok = true;
              buffer[offset] = '\x00';
              break;
            } 
            else if (newChar != -1) {
              if (newChar==',') 
                {
                  if (count<2) buffer[offset]='-';
                  else if (count>2) buffer[offset]=':';
                  else buffer[offset]=' ';
                  count++;
                }
              else buffer[offset] = newChar;
              offset++;
            }
          }
        }
      }
    }
    if (isConnected()) {
        close();
      }
    exitCommandMode();
  } 
  if (!ok)
    {
      buffer[0] = '#';
      buffer[1] = 0x00;
      //Serial.println("Fail!!");
    }
  return buffer;
} 


#define SCAN_BUFFER_SIZE 4 

char* SmartCitizen::scan() {
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
  return buffer;
} 
  
char* SmartCitizen::itoa(uint32_t number)
  {
   byte count = 0;
   uint32_t temp = number;
   while ((temp/10)!=0) 
   {
     temp = temp/10;
     count++;
   }
   int i;
   for (i = count; i>=0; i--) 
   { 
     buffer[i] = number%10 + '0'; 
     number = number/10; 
   }
   buffer[count + 1] = 0x00;
   return buffer;   
  }
  
#define numbers_retry 5

boolean SmartCitizen::server_connect()
  {
    uint16_t pos = readintEEPROM(EE_ADDR_NUMBER_MEASURES);
    writeData(DEFAULT_ADDR_MEASURES, pos + 1, scan());  //Wifi Nets
    writeData(DEFAULT_ADDR_MEASURES, pos + 2, WIFItime());
    checkData(); //Volvemos a verificar si datos correctos
    return server_reconnect(); 
  }

boolean SmartCitizen::server_reconnect()
  {
    char* mac_Address = mac();
    int retry = 0;
    boolean ok = false;   
    while ((!ok)&&(retry<numbers_retry)){
        if (open(WEB[0], 80)) ok = true;
        else 
          {
            retry++;
            if (retry >= numbers_retry) return ok;
          }
      }    
    for (byte i = 1; i<5; i++) Serial1.print(WEB[i]);
    Serial1.print(mac_Address);
    for (byte i = 5; i<7; i++) Serial1.print(WEB[i]);
    return ok; 
  }

void SmartCitizen::json_update(uint16_t initial)
  {  
    uint16_t updates = ((readintEEPROM(EE_ADDR_NUMBER_MEASURES) + 1)/10); 
    if ((initial + POST_MAX) <= updates) updates = initial + POST_MAX;    
    if (updates > 0)
      {
        Serial1.print(F("["));  
        for (uint16_t pending = initial; pending < updates; pending++)
         { 
           byte i;
           for (i = 0; i<10; i++)
            {
              Serial1.print(SERVER[i]);
              Serial1.print(readData(DEFAULT_ADDR_MEASURES, i + pending*10, 0));
            }  
           Serial1.print(SERVER[i]);
           if ((updates > 1)&&(pending < (updates-1))) Serial1.print(F(","));
         }
        Serial1.println(F("]"));
        Serial1.println();
      }
    #if debuggSCK  
      if (updates > 0)
        {
          Serial.print(F("["));  
          for (uint16_t pending = initial; pending < updates; pending++)
           { 
             byte i;
             for (i = 0; i<10; i++)
              {
                Serial.print(SERVER[i]);
                Serial.print(readData(DEFAULT_ADDR_MEASURES, i + pending*10, 0));
              }  
             Serial.print(SERVER[i]);
             if ((updates > 1)&&(pending < (updates-1))) Serial.print(F(","));
           }
          Serial.println(F("]"));
        }
     #endif 
  }  






/*TIMER*/

TimerOne Timer1;              // preinstatiate

ISR(TIMER1_OVF_vect)          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
  Timer1.isrCallback();
}

void TimerOne::initialize(long microseconds)
{
  TCCR1A = 0;                 // clear control register A 
  TCCR1B = _BV(WGM13);        // set mode 8: phase and frequency correct pwm, stop the timer
  setPeriod(microseconds);
}

#define RESOLUTION 65536    // Timer1 is 16 bit

void TimerOne::setPeriod(long microseconds)		// AR modified for atomic access
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

void TimerOne::attachInterrupt(void (*isr)(), long microseconds)
{
  if(microseconds > 0) setPeriod(microseconds);
  isrCallback = isr;                                       // register the user's callback with the real ISR
  TIMSK1 = _BV(TOIE1);                                     // sets the timer overflow interrupt enable bit
	// AR - remove sei() - might be running with interrupts disabled (eg inside an ISR), so leave unchanged
//  sei();                                                   // ensures that interrupts are globally enabled
  resume();
}

void TimerOne::resume()				// AR suggested
{ 
  TCCR1B |= clockSelectBits;
}

void TimerOne::start()	// AR addition, renamed by Lex to reflect it's actual role
{
  unsigned int tcnt1;
  
  TIMSK1 &= ~_BV(TOIE1);        // AR added 
  GTCCR |= _BV(PSRSYNC);   		// AR added - reset prescaler (NB: shared with all 16 bit timers);

  oldSREG = SREG;				// AR - save status register
  cli();						// AR - Disable interrupts
  TCNT1 = 0;                	
  SREG = oldSREG;          		// AR - Restore status register

  do {	// Nothing -- wait until timer moved on from zero - otherwise get a phantom interrupt
	oldSREG = SREG;
	cli();
	tcnt1 = TCNT1;
	SREG = oldSREG;
  } while (tcnt1==0); 
 
//  TIFR1 = 0xff;              		// AR - Clear interrupt flags
//  TIMSK1 = _BV(TOIE1);              // sets the timer overflow interrupt enable bit
}

void TimerOne::stop()
{
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));          // clears all clock selects bits
}



uint8_t I2C::bytesAvailable = 0;
uint8_t I2C::bufferIndex = 0;
uint8_t I2C::totalBytes = 0;
uint16_t I2C::timeOutDelay = 0;

I2C::I2C()
{
}


/*I2C*/

void I2C::begin()
{
  #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega328P__)
    // activate internal pull-ups for twi
    // as per note from atmega8 manual pg167
    sbi(PORTC, 4);
    sbi(PORTC, 5);
  #else
    // activate internal pull-ups for twi
    // as per note from atmega128 manual pg204
    sbi(PORTD, 0);
    sbi(PORTD, 1);
  #endif
  // initialize twi prescaler and bit rate
  cbi(TWSR, TWPS0);
  cbi(TWSR, TWPS1);
  TWBR = ((F_CPU / 100000) - 16) / 2;
  // enable twi module and acks
  TWCR = _BV(TWEN) | _BV(TWEA); 
}

void I2C::end()
{
  TWCR = 0;
}

void I2C::timeOut(uint16_t _timeOut)
{
  timeOutDelay = _timeOut;
}

void I2C::setSpeed(uint8_t _fast)
{
  if(!_fast)
  {
    TWBR = ((F_CPU / 100000) - 16) / 2;
  }
  else
  {
    TWBR = ((F_CPU / 400000) - 16) / 2;
  }
}
 
uint8_t I2C::available()
{
  return(bytesAvailable);
}

uint8_t I2C::receive()
{
  bufferIndex = totalBytes - bytesAvailable;
  if(!bytesAvailable)
  {
    bufferIndex = 0;
    return(0);
  }
  bytesAvailable--;
  return(data[bufferIndex]);
}

  
/*return values for new functions that use the timeOut feature 
  will now return at what point in the transmission the timeout
  occurred. Looking at a full communication sequence between a 
  master and slave (transmit data and then readback data) there
  a total of 7 points in the sequence where a timeout can occur.
  These are listed below and correspond to the returned value:
  1 - Waiting for successful completion of a Start bit
  2 - Waiting for ACK/NACK while addressing slave in transmit mode (MT)
  3 - Waiting for ACK/NACK while sending data to the slave
  4 - Waiting for successful completion of a Repeated Start
  5 - Waiting for ACK/NACK while addressing slave in receiver mode (MR)
  6 - Waiting for ACK/NACK while receiving data from the slave
  7 - Waiting for successful completion of the Stop bit

  All possible return values:
  0           Function executed with no errors
  1 - 7       Timeout occurred, see above list
  8 - 0xFF    See datasheet for exact meaning */ 


/////////////////////////////////////////////////////

uint8_t I2C::write(uint8_t address, uint8_t registerAddress)
{
  returnStatus = 0;
  returnStatus = start();
  if(returnStatus){return(returnStatus);}
  returnStatus = sendAddress(SLA_W(address));
  if(returnStatus)
  {
    if(returnStatus == 1){return(2);}
    return(returnStatus);
  }
  returnStatus = sendByte(registerAddress);
  if(returnStatus)
  {
    if(returnStatus == 1){return(3);}
    return(returnStatus);
  }
  returnStatus = stop();
  if(returnStatus)
  {
    if(returnStatus == 1){return(7);}
    return(returnStatus);
  }
  return(returnStatus);
}

uint8_t I2C::write(int address, int registerAddress)
{
  return(write((uint8_t) address, (uint8_t) registerAddress));
}

uint8_t I2C::write(uint8_t address, uint16_t registerAddress, uint8_t data, boolean mode)
{
  returnStatus = 0;
  returnStatus = start(); 
  if(returnStatus){return(returnStatus);}
  returnStatus = sendAddress(SLA_W(address));
  if(returnStatus)
  {
    if(returnStatus == 1){return(2);}
    return(returnStatus);
  }
  if (!mode) returnStatus = sendByte(registerAddress);
  else 
    {
      returnStatus = sendByte((registerAddress >> 8) & 0x00FF);
      returnStatus = sendByte(registerAddress & 0x00FF);
    }
  if(returnStatus)
  {
    if(returnStatus == 1){return(3);}
    return(returnStatus);
  }
  returnStatus = sendByte(data);
  if(returnStatus)
  {
    if(returnStatus == 1){return(3);}
    return(returnStatus);
  }
  returnStatus = stop();
  if(returnStatus)
  {
    if(returnStatus == 1){return(7);}
    return(returnStatus);
  }
  return(returnStatus);
}

uint8_t I2C::write(int address, int registerAddress, int data)
{
  return(write((uint8_t) address, (uint8_t) registerAddress, (uint8_t) data));
}

uint8_t I2C::write(uint8_t address, uint8_t registerAddress, char *data)
{
  uint8_t bufferLength = strlen(data);
  returnStatus = 0;
  returnStatus = write(address, registerAddress, (uint8_t*)data, bufferLength);
  return(returnStatus);
}

uint8_t I2C::write(uint8_t address, uint8_t registerAddress, uint8_t *data, uint8_t numberBytes)
{
  returnStatus = 0;
  returnStatus = start();
  if(returnStatus){return(returnStatus);}
  returnStatus = sendAddress(SLA_W(address));
  if(returnStatus)
  {
    if(returnStatus == 1){return(2);}
    return(returnStatus);
  }
  returnStatus = sendByte(registerAddress);
  if(returnStatus)
  {
    if(returnStatus == 1){return(3);}
    return(returnStatus);
  }
  for (uint8_t i = 0; i < numberBytes; i++)
  {
    returnStatus = sendByte(data[i]);
    if(returnStatus)
      {
        if(returnStatus == 1){return(3);}
        return(returnStatus);
      }
  }
  returnStatus = stop();
  if(returnStatus)
  {
    if(returnStatus == 1){return(7);}
    return(returnStatus);
  }
  return(returnStatus);
}

uint8_t I2C::read(int address, int numberBytes)
{
  return(read((uint8_t) address, (uint8_t) numberBytes));
}

uint8_t I2C::read(uint8_t address, uint8_t numberBytes)
{
  bytesAvailable = 0;
  bufferIndex = 0;
  if(numberBytes == 0){numberBytes++;}
  nack = numberBytes - 1;
  returnStatus = 0;
  returnStatus = start();
  if(returnStatus){return(returnStatus);}
  returnStatus = sendAddress(SLA_R(address));
  if(returnStatus)
  {
    if(returnStatus == 1){return(5);}
    return(returnStatus);
  }
  for(uint8_t i = 0; i < numberBytes; i++)
  {
    if( i == nack )
    {
      returnStatus = receiveByte(0);
      if(returnStatus == 1){return(6);}

      if(returnStatus != MR_DATA_NACK){return(returnStatus);}
    }
    else
    {
      returnStatus = receiveByte(1);
      if(returnStatus == 1){return(6);}
      if(returnStatus != MR_DATA_ACK){return(returnStatus);}
    }
    data[i] = TWDR;
    bytesAvailable = i+1;
    totalBytes = i+1;
  }
  returnStatus = stop();
  if(returnStatus)
  {
    if(returnStatus == 1){return(7);}
    return(returnStatus);
  }
  return(returnStatus);
}

uint8_t I2C::read(int address, int registerAddress, int numberBytes)
{
  return(read((uint8_t) address, (uint8_t) registerAddress, (uint8_t) numberBytes));
}

uint8_t I2C::read(uint8_t address, uint16_t registerAddress, uint8_t numberBytes, boolean mode)
{
  bytesAvailable = 0;
  bufferIndex = 0;
  if(numberBytes == 0){numberBytes++;}
  nack = numberBytes - 1;
  returnStatus = 0;
  returnStatus = start();
  if(returnStatus){return(returnStatus);}
  returnStatus = sendAddress(SLA_W(address));
  if(returnStatus)
  {
    if(returnStatus == 1){return(2);}
    return(returnStatus);
  }
  if (!mode) returnStatus = sendByte(registerAddress);
  else 
    {
      returnStatus = sendByte((registerAddress >> 8) & 0x00FF);
      returnStatus = sendByte(registerAddress & 0x00FF);
    }
  
  if(returnStatus)
  {
    if(returnStatus == 1){return(3);}
    return(returnStatus);
  }
  returnStatus = start();
  if(returnStatus)
  {
    if(returnStatus == 1){return(4);}
    return(returnStatus);
  }
  returnStatus = sendAddress(SLA_R(address));
  if(returnStatus)
  {
    if(returnStatus == 1){return(5);}
    return(returnStatus);
  }
  for(uint8_t i = 0; i < numberBytes; i++)
  {
    if( i == nack )
    {
      returnStatus = receiveByte(0);
      if(returnStatus == 1){return(6);}
      if(returnStatus != MR_DATA_NACK){return(returnStatus);}
    }
    else
    {
      returnStatus = receiveByte(1);
      if(returnStatus == 1){return(6);}
      if(returnStatus != MR_DATA_ACK){return(returnStatus);}
    }
    data[i] = TWDR;
    bytesAvailable = i+1;
    totalBytes = i+1;
  }
  returnStatus = stop();
  if(returnStatus)
  {
    if(returnStatus == 1){return(7);}
    return(returnStatus);
  }
  return(returnStatus);
}

uint8_t I2C::read(uint8_t address, uint8_t numberBytes, uint8_t *dataBuffer)
{
  bytesAvailable = 0;
  bufferIndex = 0;
  if(numberBytes == 0){numberBytes++;}
  nack = numberBytes - 1;
  returnStatus = 0;
  returnStatus = start();
  if(returnStatus){return(returnStatus);}
  returnStatus = sendAddress(SLA_R(address));
  if(returnStatus)
  {
    if(returnStatus == 1){return(5);}
    return(returnStatus);
  }
  for(uint8_t i = 0; i < numberBytes; i++)
  {
    if( i == nack )
    {
      returnStatus = receiveByte(0);
      if(returnStatus == 1){return(6);}
      if(returnStatus != MR_DATA_NACK){return(returnStatus);}
    }
    else
    {
      returnStatus = receiveByte(1);
      if(returnStatus == 1){return(6);}
      if(returnStatus != MR_DATA_ACK){return(returnStatus);}
    }
    dataBuffer[i] = TWDR;
    bytesAvailable = i+1;
    totalBytes = i+1;
  }
  returnStatus = stop();
  if(returnStatus)
  {
    if(returnStatus == 1){return(7);}
    return(returnStatus);
  }
  return(returnStatus);
}

uint8_t I2C::read(uint8_t address, uint8_t registerAddress, uint8_t numberBytes, uint8_t *dataBuffer)
{
  bytesAvailable = 0;
  bufferIndex = 0;
  if(numberBytes == 0){numberBytes++;}
  nack = numberBytes - 1;
  returnStatus = 0;
  returnStatus = start();
  if(returnStatus){return(returnStatus);}
  returnStatus = sendAddress(SLA_W(address));
  if(returnStatus)
  {
    if(returnStatus == 1){return(2);}
    return(returnStatus);
  }
  returnStatus = sendByte(registerAddress);
  if(returnStatus)
  {
    if(returnStatus == 1){return(3);}
    return(returnStatus);
  }
  returnStatus = start();
  if(returnStatus)
  {
    if(returnStatus == 1){return(4);}
    return(returnStatus);
  }
  returnStatus = sendAddress(SLA_R(address));
  if(returnStatus)
  {
    if(returnStatus == 1){return(5);}
    return(returnStatus);
  }
  for(uint8_t i = 0; i < numberBytes; i++)
  {
    if( i == nack )
    {
      returnStatus = receiveByte(0);
      if(returnStatus == 1){return(6);}
      if(returnStatus != MR_DATA_NACK){return(returnStatus);}
    }
    else
    {
      returnStatus = receiveByte(1);
      if(returnStatus == 1){return(6);}
      if(returnStatus != MR_DATA_ACK){return(returnStatus);}
    }
    dataBuffer[i] = TWDR;
    bytesAvailable = i+1;
    totalBytes = i+1;
  }
  returnStatus = stop();
  if(returnStatus)
  {
    if(returnStatus == 1){return(7);}
    return(returnStatus);
  }
  return(returnStatus);
}


/////////////// Private Methods ////////////////////////////////////////


uint8_t I2C::start()
{
  unsigned long startingTime = millis();
  TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
  while (!(TWCR & (1<<TWINT)))
  {
    if(!timeOutDelay){continue;}
    if((millis() - startingTime) >= timeOutDelay)
    {
      lockUp();
      return(1);
    }
       
  }
  if ((TWI_STATUS == START) || (TWI_STATUS == REPEATED_START))
  {
    return(0);
  }
  if (TWI_STATUS == LOST_ARBTRTN)
  {
    uint8_t bufferedStatus = TWI_STATUS;
    lockUp();
    return(bufferedStatus);
  }
  return(TWI_STATUS);
}

uint8_t I2C::sendAddress(uint8_t i2cAddress)
{
  TWDR = i2cAddress;
  unsigned long startingTime = millis();
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)))
  {
    if(!timeOutDelay){continue;}
    if((millis() - startingTime) >= timeOutDelay)
    {
      lockUp();
      return(1);
    }
       
  }
  if ((TWI_STATUS == MT_SLA_ACK) || (TWI_STATUS == MR_SLA_ACK))
  {
    return(0);
  }
  uint8_t bufferedStatus = TWI_STATUS;
  if ((TWI_STATUS == MT_SLA_NACK) || (TWI_STATUS == MR_SLA_NACK))
  {
    stop();
    return(bufferedStatus);
  }
  else
  {
    lockUp();
    return(bufferedStatus);
  } 
}

uint8_t I2C::sendByte(uint8_t i2cData)
{
  TWDR = i2cData;
  unsigned long startingTime = millis();
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)))
  {
    if(!timeOutDelay){continue;}
    if((millis() - startingTime) >= timeOutDelay)
    {
      lockUp();
      return(1);
    }
       
  }
  if (TWI_STATUS == MT_DATA_ACK)
  {
    return(0);
  }
  uint8_t bufferedStatus = TWI_STATUS;
  if (TWI_STATUS == MT_DATA_NACK)
  {
    stop();
    return(bufferedStatus);
  }
  else
  {
    lockUp();
    return(bufferedStatus);
  } 
}

uint8_t I2C::receiveByte(uint8_t ack)
{
  unsigned long startingTime = millis();
  if(ack)
  {
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);

  }
  else
  {
    TWCR = (1<<TWINT) | (1<<TWEN);
  }
  while (!(TWCR & (1<<TWINT)))
  {
    if(!timeOutDelay){continue;}
    if((millis() - startingTime) >= timeOutDelay)
    {
      lockUp();
      return(1);
    }
  }
  if (TWI_STATUS == LOST_ARBTRTN)
  {
    uint8_t bufferedStatus = TWI_STATUS;
    lockUp();
    return(bufferedStatus);
  }
  return(TWI_STATUS); 
}

uint8_t I2C::stop()
{
  unsigned long startingTime = millis();
  TWCR = (1<<TWINT)|(1<<TWEN)| (1<<TWSTO);
  while ((TWCR & (1<<TWSTO)))
  {
    if(!timeOutDelay){continue;}
    if((millis() - startingTime) >= timeOutDelay)
    {
      lockUp();
      return(1);
    }
       
  }
  return(0);
}

void I2C::lockUp()
{
  TWCR = 0; //releases SDA and SCL lines to high impedance
  TWCR = _BV(TWEN) | _BV(TWEA); //reinitialize TWI 
}

I2C I2c = I2C();

#endif

