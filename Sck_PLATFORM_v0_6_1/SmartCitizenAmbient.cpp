#ifndef SMARTCITIZENAMBIENT_cpp
#define SMARTCITIZENAMBIENT_cpp

#include "SmartCitizenAmbient.h"

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
                  "\",\"co\":\"", 
                  "\",\"no2\":\"", 
                  "\",\"light\":\"", 
                  "\",\"noise\":\"", 
                  "\",\"bat\":\"", 
                  "\",\"panel\":\"", 
                  "\",\"nets\":\"", 
                  "\",\"timestamp\":\"", 
                  "\"}"
                  };
                  
float Rs0 = 0;
float Rs1 = 0;

float k= (RES*(float)R1/100)/1000;  //Constante de conversion a tensiÃ³n de los reguladores 
float kr= ((float)P1*1000)/RES;     //Constante de conversion a resistencia de potenciometros 

#define buffer_length 64
static char buffer[buffer_length];

void SmartCitizen::begin() {
  Wire.begin();
  TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;  
  DHT22(DHT22_PIN);
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

boolean SmartCitizen::check_text(byte inByte, char* text, byte *check)
  {
    if (inByte == text[*check]) 
      {
        *check = *check + 1;
        if (*check == strlen(text)) 
        {
          *check = 0;
          return true;
        }
      }
    else *check = 0;
    return false;
  }

void SmartCitizen::check_data()
  {  
    uint16_t check_measures = readintEEPROM(EE_ADDR_NUMBER_MEASURES);
    uint16_t post = readintEEPROM(EE_ADDR_POST_MEASURES);
    if (check_measures > 0) {check_measures = check_measures%10;}
    uint16_t address = readintEEPROM(EE_ADDR_FREE_ADDR_MEASURES);
    byte count=0;
    if (check_measures > 0)
      {
        while ((count<=check_measures)&&(address>(EE_ADDR_MEASURES-2))) 
          {
            if (readEEPROM(address) == 0x00) count++;
            address--;
          }
          address = address + 2;
      }
//    Serial.println(address);
      writeintEEPROM(EE_ADDR_NUMBER_MEASURES, readintEEPROM(EE_ADDR_NUMBER_MEASURES) - check_measures);
      writeintEEPROM(EE_ADDR_FREE_ADDR_MEASURES, address); 
  }
    
    
void SmartCitizen::writeMCP(byte deviceaddress, byte address, int data ) {
  if (data>RES) data=RES;
  Wire.beginTransmission(deviceaddress);
  address=(address<<4)|bitRead(data, 8) ;
  Wire.write(address);
  Wire.write(lowByte(data));
  Wire.endTransmission();
  delay(4);
}

#define DIRECT_READ(base, mask)		(((*(base)) & (mask)) ? 1 : 0)
#define DIRECT_MODE_INPUT(base, mask)	((*(base+1)) &= ~(mask))
#define DIRECT_MODE_OUTPUT(base, mask)	((*(base+1)) |= (mask))
#define DIRECT_WRITE_LOW(base, mask)	((*(base+2)) &= ~(mask))

// This should be 40, but the sensor is adding an extra bit at the start
#define DHT22_DATA_BIT_COUNT 41

void SmartCitizen::DHT22(uint8_t pin)
{
    _bitmask = digitalPinToBitMask(pin);
    _baseReg = portInputRegister(digitalPinToPort(pin));
    _lastReadTime = millis();
    _lastHumidity = DHT22_ERROR_VALUE;
    _lastTemperature = DHT22_ERROR_VALUE;
}

boolean SmartCitizen::dhtRead()
{
  uint8_t bitmask = _bitmask;
  volatile uint8_t *reg asm("r30") = _baseReg;
  uint8_t retryCount;
  uint8_t bitTimes[DHT22_DATA_BIT_COUNT];
  int currentHumidity;
  int currentTemperature;
  uint8_t checkSum, csPart1, csPart2, csPart3, csPart4;
  unsigned long currentTime;
  int i;

  currentHumidity = 0;
  currentTemperature = 0;
  checkSum = 0;
  currentTime = millis();
  for(i = 0; i < DHT22_DATA_BIT_COUNT; i++)
  {
    bitTimes[i] = 0;
  }

  if(currentTime - _lastReadTime < 2000)
  {
    // Caller needs to wait 2 seconds between each call to readData
    return false;
  }
  _lastReadTime = currentTime;

  // Pin needs to start HIGH, wait until it is HIGH with a timeout
  cli();
  DIRECT_MODE_INPUT(reg, bitmask);
  sei();
  retryCount = 0;
  do
  {
    if (retryCount > 125)
    {
      return false;
    }
    retryCount++;
    delayMicroseconds(2);
  } while(!DIRECT_READ(reg, bitmask));
  // Send the activate pulse
  cli();
  DIRECT_WRITE_LOW(reg, bitmask);
  DIRECT_MODE_OUTPUT(reg, bitmask); // Output Low
  sei();
  delayMicroseconds(1100); // 1.1 ms
  cli();
  DIRECT_MODE_INPUT(reg, bitmask);	// Switch back to input so pin can float
  sei();
  // Find the start of the ACK Pulse
  retryCount = 0;
  do
  {
    if (retryCount > 25) //(Spec is 20 to 40 us, 25*2 == 50 us)
    {
      return false;
    }
    retryCount++;
    delayMicroseconds(2);
  } while(!DIRECT_READ(reg, bitmask));
  // Find the end of the ACK Pulse
  retryCount = 0;
  do
  {
    if (retryCount > 50) //(Spec is 80 us, 50*2 == 100 us)
    {
      return false;
    }
    retryCount++;
    delayMicroseconds(2);
  } while(DIRECT_READ(reg, bitmask));
  // Read the 40 bit data stream
  for(i = 0; i < DHT22_DATA_BIT_COUNT; i++)
  {
    // Find the start of the sync pulse
    retryCount = 0;
    do
    {
      if (retryCount > 35) //(Spec is 50 us, 35*2 == 70 us)
      {
        return false;
      }
      retryCount++;
      delayMicroseconds(2);
    } while(!DIRECT_READ(reg, bitmask));
    // Measure the width of the data pulse
    retryCount = 0;
    do
    {
      if (retryCount > 50) //(Spec is 80 us, 50*2 == 100 us)
      {
        return false;
      }
      retryCount++;
      delayMicroseconds(2);
    } while(DIRECT_READ(reg, bitmask));
    bitTimes[i] = retryCount;
  }
  for(i = 0; i < 16; i++)
  {
    if(bitTimes[i + 1] > 11)
    {
      currentHumidity |= (1 << (15 - i));
    }
  }
  for(i = 0; i < 16; i++)
  {
    if(bitTimes[i + 17] > 11)
    {
      currentTemperature |= (1 << (15 - i));
    }
  }
  for(i = 0; i < 8; i++)
  {
    if(bitTimes[i + 33] > 11)
    {
      checkSum |= (1 << (7 - i));
    }
  }

  _lastHumidity = currentHumidity & 0x7FFF;
  if(currentTemperature & 0x8000)
  {
    // Below zero, non standard way of encoding negative numbers!
    // Convert to native negative format.
    _lastTemperature = -currentTemperature & 0x7FFF;
  }
  else
  {
    _lastTemperature = currentTemperature;
  }

  csPart1 = currentHumidity >> 8;
  csPart2 = currentHumidity & 0xFF;
  csPart3 = currentTemperature >> 8;
  csPart4 = currentTemperature & 0xFF;
  if(checkSum == ((csPart1 + csPart2 + csPart3 + csPart4) & 0xFF))
  {
    return true;
  }
  return false;
}

  
int SmartCitizen::readMCP(int deviceaddress, byte address ) {
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
  
  
void SmartCitizen::VH_MICS(byte device, long voltage ) {
  int data=0;
  int temp = (int)(((voltage/1.2)-1000)*k);
  if (temp>RES) data = RES;
  else if (temp<0) data=0;
  else data = temp;
  writeMCP(MCP2, device, data);
}
  
float SmartCitizen::readVH(byte device) {
  int data;
  data=readMCP(MCP2, device);
  float voltage = (data/k + 1000)*1.2;
  return(voltage);
}
  
void SmartCitizen::RL_MICS(byte device, long resistor) {
  int data=0x00;
  data = (int)(resistor/kr);
  writeMCP(MCP1, device, data);
}
  
  
void SmartCitizen::writeEEPROM(uint16_t eeaddress, byte data ) {
  Wire.beginTransmission(E2PROM);
  Wire.write((byte)(eeaddress >> 8));   // MSB
  Wire.write((byte)(eeaddress & 0xFF)); // LSB
  Wire.write(data);
  Wire.endTransmission();
  delay(8);
}

void SmartCitizen::writeintEEPROM(uint16_t eeaddress, uint16_t data )
{
  writeEEPROM(eeaddress , highByte(data));
  writeEEPROM(eeaddress  + 1, lowByte(data)); ;
}

byte SmartCitizen::readEEPROM(uint16_t eeaddress ) {
  byte rdata = 0xFF;
  Wire.beginTransmission(E2PROM);
  Wire.write((byte)(eeaddress >> 8));   // MSB
  Wire.write((byte)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(E2PROM,1);
  while (!Wire.available()); rdata = Wire.read();
  //if (Wire.available()); rdata = Wire.read();
  return rdata;
}

uint16_t SmartCitizen::readintEEPROM(uint16_t eeaddress)
{
  return (readEEPROM(eeaddress)<<8)+ readEEPROM(eeaddress + 1); 
}

char* SmartCitizen::readCommand(uint16_t eeaddress, uint16_t *pointer )
{
  uint16_t i;
  for (i = eeaddress; (readEEPROM(i)!= 0x00&&((i - eeaddress)<buffer_length)); i++) buffer[i - eeaddress] = readEEPROM(i);
  buffer[i - eeaddress] = 0x00;
  *pointer = i+1;
  delayMicroseconds(10);
  return buffer;
}

void SmartCitizen::writeCommand(uint16_t eeaddressfree, char* text )
{
  uint16_t eeaddress = readintEEPROM(eeaddressfree);
  uint16_t i;
  for (i = eeaddress; text[i - eeaddress]!= 0x00; i++) writeEEPROM(i, text[i - eeaddress]);
  writeEEPROM(i, 0x00);
  i++; 
  writeintEEPROM(eeaddressfree, i);
  if (eeaddressfree == EE_ADDR_FREE_ADDR_MEASURES) writeintEEPROM(EE_ADDR_NUMBER_MEASURES, readintEEPROM(EE_ADDR_NUMBER_MEASURES) + 1);
  else if (eeaddressfree == EE_ADDR_FREE_SSID) writeintEEPROM(EE_ADDR_NUMBER_NETS, readintEEPROM(EE_ADDR_NUMBER_NETS) + 1);
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
      Wire.beginTransmission(DS1307_ADDRESS);
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
    }
    return false;
}

char* SmartCitizen::RTCtime() { 
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write((int)0);	
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 7);
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

void SmartCitizen::getMICS(unsigned long time0, unsigned long time1){          
     
      /*Correccion de la tension del Heather*/
      
      VH_MICS(MICS_5525, 2400); //VH_MICS5525 Inicial
      digitalWrite(IO0, HIGH); //VH_MICS5525
      
      VH_MICS(MICS_2710, 1700); //VH_MICS5525 Inicial
      digitalWrite(IO1, HIGH); //VH_MICS2710
      digitalWrite(IO2, LOW); //RADJ_MICS2710 PIN ALTA IMPEDANCIA

      
      #if debuggSCK
        Serial.println("*******************");
        Serial.println("MICS5525 VH a 2.4V");
        Serial.println("MICS2710 VH a 1.7V");
      #endif
      
      delay(200);
      float Vc0 = (float)average(S2)*Vcc/1023; //mV 
      float current0 = Vc0/Rc; //mA 
      float Rh0 = (readVH(MICS_5525)- Vc0)/current0;
      float Vh0 = (Rh0 + Rc)*32;
      VH_MICS(MICS_5525, Vh0); //VH_MICS5525 Corregido 
      
      float Vc1 = (float)average(S3)*Vcc/1023; //mV 
      float current1 = Vc1/Rc; //mA 
      float Rh1 = (readVH(MICS_2710)- Vc1)/current1; //Ohm
      float Vh1 = (Rh1 + Rc)*26; //mV 
      VH_MICS(MICS_2710, Vh1); //VH_MICS2710 Corregido
      
      #if debuggSCK
        Serial.print("MICS5525 corriente:");
        Serial.print(current0);
        Serial.println("mA");
        Serial.print("MICS2710 corriente:");
        Serial.print(current1);
        Serial.println("mA");
        Serial.print("MICS5525 correccion VH: ");
        Serial.print(Vh0/1000.);
        Serial.println("V");
        Vc0 = (float)average(S2)*Vcc/1023; //mV 
        current0 = Vc0/Rc; //mA 
        Serial.print("MICS5525 corriente corregida:");
        Serial.print(current0);
        Serial.println("mA");
        Serial.println("Heating...");
        Serial.print("MICS2710 correccion VH: ");
        Serial.print(Vh1/1000.);
        Serial.println("V");
        Vc1 = (float)average(S3)*Vcc/1023; //mV 
        current1 = Vc1/Rc; //mA 
        Serial.print("MICS2710 corriente corregida:");
        Serial.print(current1);
        Serial.println("mA");
        Serial.println("Heating...");
      #endif
      
      delay(5000); // Tiempo de heater!
      
      /*Lectura de datos*/
      RL_MICS(MICS_5525, 100000);
      digitalWrite(IO0, LOW);  //VH_MICS5525 OFF para lectura
      
      #if debuggSCK
        Serial.println("MICS5525 VH OFF ");
      #endif
      
      delay(time0); //Tiempo de enfriamiento para lectura
  
      float RL0 = kr*readMCP(MCP1, MICS_5525)/1000; //Kohm
      float VL0 = ((float)average(S0)*Vcc)/1023; //mV
      Rs0 = ((Vcc-VL0)/VL0)*RL0; //Kohm
  
      /*Correccion de impedancia de carga*/
      if (Rs0 < 100)
      {
        delay(100);
        RL_MICS(MICS_5525, Rs0*1000);
        RL0 = kr*readMCP(MCP1, MICS_5525)/1000; //Kohm
        VL0 = ((float)average(S0)*Vcc)/1023; //mV
        Rs0 = ((Vcc-VL0)/VL0)*RL0; //Kohm
      }
      
      Rs0 = Rs0*1000; //ohm
      
      #if debuggSCK
        Serial.print("MICS5525 Rs: ");
        Serial.print(Rs0);
        Serial.println("K");
      #endif
      
      RL_MICS(MICS_2710, 10000);
      
      delay(time1 - time0); 
      
      float RL1 = kr*readMCP(MCP1, MICS_2710)/1000; //Kohm
      float VL1 = ((float)average(S1)*Vcc)/1023; //mV
      Rs1 = ((2500-VL1)/VL1)*RL1; //Kohm
  
      /*Correccion de impedancia de carga*/
      if (Rs1 > 100) RL_MICS(MICS_2710, 100000);
      else RL_MICS(MICS_2710, Rs1*1000);
      delay(100);
      RL1 = kr*readMCP(MCP1, MICS_2710)/1000; //Kohm
      VL1 = ((float)average(S1)*Vcc)/1023; //mV
      
      Rs1 = ((2500-VL1)/VL1)*RL1*1000; //ohm
      
      #if debuggSCK
        Serial.print("MICS2710 Rs: ");
        Serial.print(Rs1);
        Serial.println("K");
      #endif
  
      digitalWrite(IO1, LOW); //VH MICS2710 OFF
      #if debuggSCK
        Serial.println("MICS2710 VH OFF ");
        Serial.println("*******************");
      #endif
}
  
  int SmartCitizen::getTemperatureC(){
      return _lastTemperature;
  }
  
  int SmartCitizen::getHumidity(){
      return _lastHumidity;
  }
  
  uint16_t SmartCitizen::getPanel(){
    int value = map(average(PANEL), 409, 819, 0, 1000); 
    if (value>0) return value;
    else return 0;
  }
  
  uint16_t SmartCitizen::getBattery() {
    uint16_t temp = average(BAT);
    uint16_t MAX = readintEEPROM(EE_ADDR_MAX_BATTERY);
    if (temp > MAX)
      {
        writeintEEPROM(EE_ADDR_MAX_BATTERY, temp);
        MAX = temp;
      }
    temp = map(temp, 613, MAX, 0, 1000);
    if (temp>1000) temp=1000;
    if (temp<0) temp=0;
    return temp; 
  }
  
  uint16_t SmartCitizen::getLight(){
    int temp = map(average(S5), 0, 1023, 0, 1000);
    if (temp>1000) temp=1000;
    if (temp<0) temp=0;
    return temp;
  }
  
  unsigned int SmartCitizen::getNoise() {
    unsigned long temp = 0;
    int n = 200;
    
    digitalWrite(IO1, HIGH); //VH_MICS2710
    delay(500); // LE DAMOS TIEMPO A LA FUENTE Y QUE DESAPAREZCA EL TRANSITORIO
  //  
    for (int i=0; i<n; i++)
    {
      delay(1);
      //temp_actual = analogRead(S4);
      float mVRaw = (float)((analogRead(S4))/1023.)*Vcc;
      
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
    digitalWrite(IO1, LOW); //VH_MICS2710
    
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
 
 
 void SmartCitizen::updateSensors(byte mode) { 
   
  if (mode == 2) 
    {
      writeintEEPROM(EE_ADDR_POST_MEASURES, 0x0000);
      writeintEEPROM(EE_ADDR_FREE_ADDR_MEASURES, FREE_ADDR_MEASURES);
    }
  else check_data();
  
  if (dhtRead())
  {
    writeCommand(EE_ADDR_FREE_ADDR_MEASURES, itoa(getTemperatureC())); // C
    writeCommand(EE_ADDR_FREE_ADDR_MEASURES, itoa(getHumidity())); // %
  }
  else 
  {
    writeCommand(EE_ADDR_FREE_ADDR_MEASURES, "0"); // C
    writeCommand(EE_ADDR_FREE_ADDR_MEASURES, "0"); // %
  }

  if (mode == 3) 
  {
    writeCommand(EE_ADDR_FREE_ADDR_MEASURES, "0"); //ppm
    writeCommand(EE_ADDR_FREE_ADDR_MEASURES, "0"); //ppm
  }
  else
  {
    getMICS(4000, 30000);
    writeCommand(EE_ADDR_FREE_ADDR_MEASURES, itoa(getCO())); //ppm
    writeCommand(EE_ADDR_FREE_ADDR_MEASURES, itoa(getNO2())); //ppm
  }
  
  writeCommand(EE_ADDR_FREE_ADDR_MEASURES, itoa(getLight())); //mV
  writeCommand(EE_ADDR_FREE_ADDR_MEASURES, itoa(getNoise())); //mV
  writeCommand(EE_ADDR_FREE_ADDR_MEASURES, itoa(getBattery())); //%
  writeCommand(EE_ADDR_FREE_ADDR_MEASURES, itoa(getPanel()));  // %
      
  if (mode != 1)
       {
         writeCommand(EE_ADDR_FREE_ADDR_MEASURES, "0");  //Wifi Nets
         writeCommand(EE_ADDR_FREE_ADDR_MEASURES, RTCtime());
       } 
  writeintEEPROM(EE_ADDR_POST_MEASURES, readintEEPROM(EE_ADDR_POST_MEASURES) + 1);
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
    if (!ready()&&(readintEEPROM(EE_ADDR_NUMBER_NETS)>0))
    {
      if (enterCommandMode())
      {    
          sendCommand(F("set wlan join 1")); // Enable AP mode
          sendCommand(F("set ip dhcp 1")); // Enable DHCP server
          sendCommand(F("set ip proto 10")); //Modo TCP y modo HTML
      }
      char* auth;
      char* ssid;
      char* pass;
      uint16_t pointer = EE_ADDR_AUTH;
      uint16_t pointer0 = EE_ADDR_SSID;
      uint16_t pointer1 = EE_ADDR_PASS;
      for (uint16_t nets = readintEEPROM(EE_ADDR_NUMBER_NETS) ; nets > 0; nets--) {
          auth = readCommand(pointer, &pointer);
          sendCommand(F("set wlan auth "), true);
          sendCommand(auth);
          boolean mode = true;
          if ((auth==WEP)||(auth==WEP64)) mode=false;
          Serial.print(auth);
          ssid = readCommand(pointer0, &pointer0);
          sendCommand(F("set wlan ssid "), true);
          sendCommand(ssid);
          Serial.print(" ");
          Serial.print(ssid);
          pass = readCommand(pointer1, &pointer1);
          if (mode) sendCommand(F("set wlan phrase "), true);  // WPA1, WPA2, OPEN
          else sendCommand(F("set wlan key "), true);
          sendCommand(pass);
          Serial.print(" ");
          Serial.println(pass);
          sendCommand(F("save"), false, "Storing in config"); // Store settings
          sendCommand(F("reboot"), false, "*READY*");
          if (ready()) return true;
          enterCommandMode();
      }
      return false;     
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
    writeCommand(EE_ADDR_FREE_ADDR_MEASURES, scan());  //Wifi Nets
    //writeCommand(EE_ADDR_FREE_ADDR_MEASURES, RTCtime());
    writeCommand(EE_ADDR_FREE_ADDR_MEASURES, WIFItime());
    char* temp = mac();
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
    Serial1.print(temp);
    for (byte i = 5; i<7; i++) Serial1.print(WEB[i]);
    return ok; 
  }

void SmartCitizen::json_updtate()
  {   
    uint16_t pointer0 = EE_ADDR_MEASURES; 
    uint16_t updates = readintEEPROM(EE_ADDR_POST_MEASURES); 
    if (updates > 0)
      {
        Serial.print(F("updates = "));
        Serial.println(updates);
        delay(100);
        Serial1.print(F("["));  
        for (byte pending = 0; pending < updates; pending++)
         { 
           byte i;
           for (i = 0; i<10; i++)
            {
              Serial1.print(SERVER[i]);
              Serial1.print(readCommand(pointer0, &pointer0));
            }  
           Serial1.print(SERVER[i]);
           if ((updates > 1)&&(pending < (updates-1))) Serial1.print(F(","));
         }
        Serial1.println(F("]"));
        Serial1.println();
        writeintEEPROM(EE_ADDR_POST_MEASURES, 0x0000);
        writeintEEPROM(EE_ADDR_NUMBER_MEASURES, 0x0000);
        writeintEEPROM(EE_ADDR_FREE_ADDR_MEASURES, FREE_ADDR_MEASURES);
      }
  }  
  
#endif

