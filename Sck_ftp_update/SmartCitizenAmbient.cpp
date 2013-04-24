#ifndef SMARTCITIZENAMBIENT_cpp
#define SMARTCITIZENAMBIENT_cpp

#include "SmartCitizenAmbient.h"

float Rs0 = 0;
float Rs1 = 0;

float k= (RES*(float)R1/100)/1000;  //Constante de conversion a tensión de los reguladores 
float kr= ((float)P1*1000)/RES;     //Constante de conversion a resistencia de potenciometros 


float RS_RO_MICS_5525[22] = {
    0.72, 0.68, 0.66, 0.64, 0.63, 0.62, 0.61, 0.6, 0.59, 0.58, 0.5, 0.45, 0.41, 0.38, 0.34, 0.31, 0.29, 0.275, 0.26, 0.17, 0.12, 0.088}; //Rs/Ro
float PPM_MICS_5525[22] =   {   
    1,    2,    3,    4,    5,    6,    7,   8,    9,   10,  20,   30,   40,   50,   60,   70,   80,    90,  100,  200,  300,   400}; //ppm 
float RS_RO_MICS_2710[10] = { 
    25,  55,  90, 140, 190, 260, 330, 410, 500, 1000}; //Rs/Ro
float PPM_MICS_2710[10]   = {
    0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9,   1,  1.5}; //ppm

#define DIRECT_READ(base, mask)		(((*(base)) & (mask)) ? 1 : 0)
#define DIRECT_MODE_INPUT(base, mask)	((*(base+1)) &= ~(mask))
#define DIRECT_MODE_OUTPUT(base, mask)	((*(base+1)) |= (mask))
#define DIRECT_WRITE_LOW(base, mask)	((*(base+2)) &= ~(mask))

// This should be 40, but the sensor is adding an extra bit at the start
#define DHT22_DATA_BIT_COUNT 41


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

int SmartCitizen::decimal(float temp)
{
  return((int)((temp-(int)(temp))*100));
}

uint8_t SmartCitizen::bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
uint8_t SmartCitizen::bin2bcd (uint8_t val) { return val + 6 * (val / 10); }

float SmartCitizen::mapfloat(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
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
  while (!Wire.available()); 
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
  
  
void SmartCitizen::writeEEPROM(unsigned int eeaddress, byte data ) {
  Wire.beginTransmission(E2PROM);
  Wire.write((byte)(eeaddress >> 8));   // MSB
  Wire.write((byte)(eeaddress & 0xFF)); // LSB
  Wire.write(data);
  Wire.endTransmission();
  delay(8);
}

byte SmartCitizen::readEEPROM(unsigned int eeaddress ) {
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

char* SmartCitizen::readCommand(unsigned int eeaddress, unsigned int *pointer )
{
  char text[128];
  unsigned int i;
  for (i = eeaddress; readEEPROM(i)!= 0x00; i++) text[i - eeaddress] = readEEPROM(i);
  text[i - eeaddress] = 0x00;
  *pointer = i+1;
  delayMicroseconds(10);
  return text;
}

void SmartCitizen::writeCommand(unsigned int eeaddressfree, char* text )
{
  unsigned int eeaddress = (readEEPROM(eeaddressfree)<<8) + (readEEPROM(eeaddressfree + 1));
  unsigned int i;
  for (i = eeaddress; text[i - eeaddress]!= 0x00; i++) writeEEPROM(i, text[i - eeaddress]);
  writeEEPROM(i, 0x00);
  i++;
  writeEEPROM(eeaddressfree, highByte(i));
  writeEEPROM(eeaddressfree + 1, lowByte(i));
}
  
void SmartCitizen::RTCadjust(ClockTime time) {
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.write((int)0);
    Wire.write(time.seconds);
    Wire.write(time.minutes);
    Wire.write(time.hours);
    Wire.write(0x00);
    Wire.write(time.day);
    Wire.write(time.month);
    Wire.write(time.year);
    Wire.write((int)0);
    Wire.endTransmission();
}

void SmartCitizen::RTCtime() {
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write((int)0);	
  Wire.endTransmission();
  
  Wire.requestFrom(DS1307_ADDRESS, 7);
  uint8_t ss = bcd2bin(Wire.read() & 0x7F);
  uint8_t mm = bcd2bin(Wire.read());
  uint8_t hh = bcd2bin(Wire.read());
  Wire.read();
  uint8_t d = bcd2bin(Wire.read());
  uint8_t m = bcd2bin(Wire.read());
  uint16_t y = bcd2bin(Wire.read()) + 2000;
  Serial.print(y, DEC);
  Serial.print('/');
  Serial.print(m, DEC);
  Serial.print('/');
  Serial.print(d, DEC);
  Serial.print(' ');
  Serial.print(hh, DEC);
  Serial.print(':');
  Serial.print(mm, DEC);
  Serial.print(':');
  Serial.print(ss, DEC);
  Serial.println();
}

float SmartCitizen::getMICS(unsigned long time0, unsigned long time1){          
     
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
  
      //Vc = (float)average(S2)*Vcc/1023; //mV 
      //current = Vc/Rc; //mA 
  
      float RL0 = kr*readMCP(MCP1, MICS_5525)/1000; //Kohm
      float VL0 = ((float)average(S0)*Vcc)/1023; //mV
      Rs0 = ((Vcc-VL0)/VL0)*RL0; //Kohm
      
      #if debuggSCK
        Serial.print("MICS5525 Rs: ");
        Serial.print(Rs0);
        Serial.println("K");
      #endif
  
      /*Correccion de impedancia de carga*/
      if (Rs0 < 100)
      {
        delay(100);
        RL_MICS(MICS_5525, Rs0*1000);
        RL0 = kr*readMCP(MCP1, MICS_5525)/1000; //Kohm
        VL0 = ((float)average(S0)*Vcc)/1023; //mV
        Rs0 = ((Vcc-VL0)/VL0)*RL0; //Kohm
      }
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
      Rs1 = ((2500-VL1)/VL1)*RL1; //Kohm
      
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
  
  float SmartCitizen::getTemperatureC(){
      float temperature = _lastTemperature/10.;
      #if debuggSCK
        Serial.println("*******************");
        Serial.print("Temperature: ");
        Serial.println(temperature);
      #endif
      //return myDHT22.getTemperatureC();
      return temperature;
  }
  
  float SmartCitizen::getHumidity(){
      float humidity = _lastHumidity/10.;
      #if debuggSCK
        Serial.print("Humidity: ");
        Serial.println(humidity);
      #endif
      //return myDHT22.getHumidity();
      return humidity;
  }
  
  float SmartCitizen::getPanel(){
    float value = mapfloat(average(PANEL), 409, 819, 0, 100); 
    if (value>0) {
      return value;  // %
    } 
    else { 
      return 0;  // %
    }
  }
  
  float SmartCitizen::getBattery() {
    float temp = average(BAT);
    int MAX = (readEEPROM(EE_ADDR_MAX_BATTERY )<<8) + (readEEPROM(EE_ADDR_MAX_BATTERY  + 1));
    if (temp > MAX)
      {
        writeEEPROM(EE_ADDR_MAX_BATTERY , highByte((int)temp));
        writeEEPROM(EE_ADDR_MAX_BATTERY  + 1, lowByte((int)temp));
        MAX = (int)temp;
      }
    #if debuggSCK
      Serial.print("Battery mV: ");
      Serial.print(temp);
      Serial.print("  -  Maximo mV: ");
      Serial.println(MAX);
      Serial.println("*******************");
    #endif
    temp = mapfloat(temp, 613, MAX, 0, 100);
    if (temp>100) temp=100;
    if (temp<0) temp=0;
    return temp; 
  }
  
  float SmartCitizen::getLight(){
    //float temp = mapfloat(average(S5), 370, 1021, 0, 100); 
    float temp = mapfloat(average(S5), 0, 1023, 0, 100);
    if (temp>100) temp=100;
    if (temp<0) temp=0;
    #if debuggSCK
      Serial.print("light mV: ");
      Serial.println(((float)((float)average(S5)/1023)*Vcc));
    #endif
    return temp;
    // return ((float)((float)average(S5)/1023)*Vcc);
  }
  
  float SmartCitizen::getNoise() {
    unsigned long temp = 0;
    int temp_actual = 0;
    int max_temp = 0;
    float mVRaw = 0;
    float max_mVRaw = 0;
    float dB_temp = 0;
    float dB = 0;
    int n = 200;
    
    digitalWrite(IO1, HIGH); //VH_MICS2710
    delay(500); // LE DAMOS TIEMPO A LA FUENTE Y QUE DESAPAREZCA EL TRANSITORIO
  //  
    for (int i=0; i<n; i++)
    {
      delay(1);
      //temp_actual = analogRead(S4);
      mVRaw = (float)((analogRead(S4))/1023.)*Vcc;
      
      //dB_temp = 16.801*log( (mVRaw*200)/1000. ) + 9.872;
      dB_temp = 9.7*log( (mVRaw*200)/1000. ) + 40;  // calibracion para ruido rosa // energia constante por octava
  
      temp = temp + dB_temp;
  
    }
  //  
    dB = temp/n;
    if(dB < 60) dB = 60; // minimo con la resolucion actual!
    
    //mVRaw = (float)((float)(average(S4))/1023)*Vcc;
    #if debuggSCK
      Serial.print("nOISE mV = ");
      Serial.print(mVRaw);
      Serial.print("  -  nOISE dB = ");
      Serial.println(dB);
    #endif
    digitalWrite(IO1, LOW); //VH_MICS2710
    
    //return max_mVRaw;
    return dB;
    
  }
 
  float SmartCitizen::getCO()
  {
    return Rs0;
  }  
  
  float SmartCitizen::getNO2()
  {
    return Rs1;
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

void SmartCitizen::skipRemainderOfResponse() {
  while (Serial1.available())
  {
    Serial1.read();
    delay(1);
  }
//    while (!(Serial1.available() && (Serial1.read() == '\n'))) {
//      // Skip remainder of response
//    }
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
        //Serial.println("ok");
        return true;
      }
    }
    //Serial.println("fail");
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

void SmartCitizen::config(char* ssid, char* pass, char* auth, char* antenna)
  {
    if (enterCommandMode())
    {    
        sendCommand(F("set wlan ssid "), true);
        sendCommand(ssid);
        
        sendCommand(F("set wlan auth "), true);
        sendCommand(auth);
        
        if( (auth == "1") || (auth == "8") )
          sendCommand(F("set wlan key "), true);  // WEP, WEP64 -> characters in HEX!!!!!
        else
          sendCommand(F("set wlan phrase "), true);  // WPA1, WPA2, OPEN
        
        sendCommand(pass);
        
        sendCommand(F("set wlan join 1"));
        
        sendCommand(F("set ip dhcp 1"));
        
        //set ftp address 198.175.253.161
        
        sendCommand(F("save"), false, "Storing in config");
        
        sendCommand(F("reboot"), false, "*READY*");
        
        enterCommandMode();
    }
  }  
  
boolean SmartCitizen::wifi_ready()
{
  if (enterCommandMode())
    {
      Serial1.println("join");
      if (findInResponse("Associated!", 5000)) 
      {
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
  else return false;
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
    //skipRemainderOfResponse();
    connected = false;
    //exitCommandMode();
    //uart->println("exit");
    return true;
  }
  return false;
}

#define IP_ADDRESS_BUFFER_SIZE 16 // "255.255.255.255\0"

const char * SmartCitizen::ip() {
  /*

    The return value is intended to be dropped directly
    into calls to 'print' or 'println' style methods.

   */
  static char ip[IP_ADDRESS_BUFFER_SIZE] = "";

  // TODO: Ensure we're not in a connection?

  if (enterCommandMode()) 
  {
      delay(5000);
      // Version 2.19 of the WiFly firmware has a "get ip a" command but
      // we can't use it because we want to work with 2.18 too.
      if (sendCommand(F("get ip"), false, "IP="))
      {
          char newChar;
          byte offset = 0;
    
          // Copy the IP address from the response into our buffer
          while (offset < IP_ADDRESS_BUFFER_SIZE) {
            if (Serial1.available())
            {
               newChar = Serial1.read();
               //Serial.println(newChar);
              if (newChar == ':') {
                ip[offset] = '\x00';
                break;
              } 
              else if (newChar != -1) {
                ip[offset] = newChar;
                offset++;
              }
            }
          }
          ip[IP_ADDRESS_BUFFER_SIZE-1] = '\x00';
          
          // This should skip the remainder of the output.
          // TODO: Handle this better?
          /*waitForResponse("<");
          while (uart->read() != ' ') {
          // Skip the prompt
          }*/
          
          //findInResponse("> ");
          exitCommandMode();
      }        
  }
  
  return ip;
}

ClockTime SmartCitizen::WIFItime() {
  /*

    The return value is intended to be dropped directly
    into calls to 'print' or 'println' style methods.

   */
  //static char time[TIME_BUFFER_SIZE] = "";
  ClockTime time;
  // TODO: Ensure we're not in a connection?

  if (enterCommandMode()) 
  {
      sendCommand(F("time"));
      if (sendCommand(F("show t"), false, "Time="))
      {
          char newChar;
          byte offset = 0;
    
          // Copy the IP address from the response into our buffer
          while (offset < 9) {
            if (Serial1.available())
            {
               newChar = Serial1.read();
               if ((newChar >= '0')&& (newChar <= '9'))
               {
                 byte hex = newChar - '0';
                 if (offset == 0) time.hours = hex<<4;
                 if (offset == 1) time.hours = time.hours|hex;
                 if (offset == 3) time.minutes = hex<<4;
                 if (offset == 4) time.minutes = time.minutes|hex;
                 if (offset == 6) time.seconds = hex<<4;
                 if (offset == 7) time.seconds = time.seconds|hex;
                 //Serial.println(hex);
               }
              if (newChar == '\n') {
                //time[offset] = '\x00';
                break;
              } 
              else if (newChar != -1) {
                //time[offset] = newChar;
                offset++;
              }
            }
          }
          time.year = 0x13;
          time.month = 0x03;
          time.day = 0x20;
          //time[TIME_BUFFER_SIZE-1] = '\x00';
          exitCommandMode();
      }        
  }
  
  return time;
} 

#endif
