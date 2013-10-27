#define debuggSCK     false
#define decouplerComp true
#define ppmEnabled    false

//Valores por defecto de la resistencia en vacio de los MICS
float RoCO  = 750000;
float RoNO2 = 2200;

#if ((decouplerComp)&&(F_CPU > 8000000 ))
  #include "TemperatureDecoupler.h"
  TemperatureDecoupler decoupler; //use this object to compensate for charger generated heat affecting temp values
#endif
  

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
                  
float RsCO = 0;
float RsNO2 = 0;

#define RES 256   //Resolucion de los potenciometros digitales

#if F_CPU == 8000000 
  #define R1  12    //Kohm
#else
  #define R1  82    //Kohm
#endif

#define P1  100   //Kohm 

float k= (RES*(float)R1/100)/1000;  //Constante de conversion a tension de los reguladores 
float kr= ((float)P1*1000)/RES;     //Constante de conversion a resistencia de potenciometrosen ohmios

int lastHumidity;
int lastTemperature;
uint8_t bits[5];  // buffer to receive data

#define TIMEOUT 10000

boolean sckDHT22(uint8_t pin)
{
        // READ VALUES
        int rv = sckDhtRead(pin);
        if (rv != true)
        {
              lastHumidity    = DHTLIB_INVALID_VALUE;  // invalid value, or is NaN prefered?
              lastTemperature = DHTLIB_INVALID_VALUE;  // invalid value
              return rv;
        }

        // CONVERT AND STORE
        lastHumidity    = word(bits[0], bits[1]);

        if (bits[2] & 0x80) // negative temperature
        {
            lastTemperature = word(bits[2]&0x7F, bits[3]);
            lastTemperature *= -1.0;
        }
        else
        {
            lastTemperature = word(bits[2], bits[3]);
        }

        // TEST CHECKSUM
        uint8_t sum = bits[0] + bits[1] + bits[2] + bits[3];
        if (bits[4] != sum) return false;
        if ((lastTemperature == 0)&&(lastHumidity == 0))return false;
        return true;
}

boolean sckDhtRead(uint8_t pin)
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

void sckWriteVH(byte device, long voltage ) {
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
    sckWriteMCP(MCP1, device, data);
  #else
    sckWriteMCP(MCP2, device, data);
  #endif
}

  
float sckReadVH(byte device) {
  int data;
  #if F_CPU == 8000000 
    data=sckReadMCP(MCP1, device);
    float voltage = (data/k + 1000)*0.41;
  #else
    data=sckReadMCP(MCP2, device);
    float voltage = (data/k + 1000)*1.2;
  #endif
  
  return(voltage);
}

void sckWriteRL(byte device, long resistor) {
  int data=0x00;
  data = (int)(resistor/kr);
  #if F_CPU == 8000000 
    sckWriteMCP(MCP1, device + 6, data);
  #else
    sckWriteMCP(MCP1, device, data);
  #endif
}

float sckReadRL(byte device)
{
  #if F_CPU == 8000000 
    return (kr*sckReadMCP(MCP1, device + 6)); //Devuelve en Ohms
  #else
    return (kr*sckReadMCP(MCP1, device));  //Devuelve en Ohms
  #endif 
}

void sckWriteRGAIN(byte device, long resistor) {
  int data=0x00;
  data = (int)(resistor/kr);
  sckWriteMCP(MCP2, device, data);
}

float sckReadRGAIN(byte device)
{
    return (kr*sckReadMCP(MCP2, device));  //Devuelve en Ohms
}

void sckWriteGAIN(long value)
{
  if (value == 100)
  {
    sckWriteRGAIN(0x00, 10000);
    sckWriteRGAIN(0x01, 10000);
  }
  else if (value == 1000)
  {
    sckWriteRGAIN(0x00, 10000);
    sckWriteRGAIN(0x01, 100000);
  }
  else if (value == 10000)
        {
           sckWriteRGAIN(0x00, 100000);
           sckWriteRGAIN(0x01, 100000);
        }
  delay(100);
}

float sckReadGAIN()
{
  return (sckReadRGAIN(0x00)/1000)*(sckReadRGAIN(0x01)/1000);
}    

void sckHeat(byte device, int current)
  {
    float Rc=Rc0;
    byte Sensor = S2;
    if (device == MICS_2710) { Rc=Rc1; Sensor = S3;}
    float Vc = (float)average(Sensor)*Vcc/1023; //mV 
    float current_measure = Vc/Rc; //mA 
    float Rh = (sckReadVH(device)- Vc)/current_measure;
    float Vh = (Rh + Rc)*current;
    sckWriteVH(device, Vh);
      #if debuggSCK
        if (device == MICS_2710) Serial.print("MICS2710 corriente: ");
        else Serial.print("MICS5525 corriente: ");
        Serial.print(current_measure);
        Serial.println(" mA");
        if (device == MICS_2710) Serial.print("MICS2710 correccion VH: ");
        else  Serial.print("MICS5525 correccion VH: ");
        Serial.print(sckReadVH(device));
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

   float sckReadRs(byte device)
   {
     byte Sensor = S0;
     float VMICS = VMIC0;
     if (device == MICS_2710) {Sensor = S1; VMICS = VMIC1;}
     float RL = sckReadRL(device); //Ohm
     float VL = ((float)average(Sensor)*Vcc)/1023; //mV
     if (VL > VMICS) VL = VMICS;
     float Rs = ((VMICS-VL)/VL)*RL; //Ohm
     #if debuggSCK
        if (device == MICS_5525) Serial.print("MICS5525 Rs: ");
        else Serial.print("MICS2710 Rs: ");
        Serial.print(VL);
        Serial.print(" mV, ");
        Serial.print(Rs);
        Serial.println(" Ohm");
      #endif;  
     return Rs;
   }
   
  float sckReadMICS(byte device)
  {
      float Rs = sckReadRs(device);
      float RL = sckReadRL(device); //Ohm
      
      /*Correccion de impedancia de carga*/
      if ((Rs <= (RL - 1000))||(Rs >= (RL + 1000)))
      {
        if (Rs < 2000) sckWriteRL(device, 2000);
        else sckWriteRL(device, Rs);
        delay(100);
        Rs = sckReadRs(device);
      }
       return Rs;
  }
  
void sckGetMICS(){          
     
      /*Correccion de la tension del Heather*/
      sckHeat(MICS_5525, 32); //Corriente en mA
      sckHeat(MICS_2710, 26); //Corriente en mA
      
      RsCO = sckReadMICS(MICS_5525);
      RsNO2 = sckReadMICS(MICS_2710);
       
}

 #if F_CPU == 8000000
 
   uint16_t sckReadSi7005(uint8_t type){
      uint16_t DATA = 1;
      I2c.write(Temperature,(uint16_t)0x03,type, false); //configura el dispositivo para medir
      delay(15);
      while (DATA&0x0001 == 0x0001)
            {
              I2c.read(Temperature,(uint16_t)0x00,1, false); //read 1 byte
              DATA = I2c.receive();      
            }
      I2c.read(Temperature,(uint16_t)0x01,2, false); //read 2 bytes
      DATA = I2c.receive()<<8;
      if (type == 0x11) DATA = (DATA|I2c.receive())>>2;
      else if (type == 0x01) DATA = (DATA|I2c.receive())>>4;
      return DATA;
  }
  
   void sckGetSi7005(){
      digitalWrite(IO4, LOW); //Si7005
      delay(15);
            
      lastTemperature = (((float)sckReadSi7005(0x11)/32)-50)*10;
      lastHumidity    = (((float)sckReadSi7005(0x01)/16)-24)*10;  

      #if debuggSCK
        Serial.print("Si7005: ");
        Serial.print("Temperatura: ");
        Serial.print(lastTemperature/10.);
        Serial.print(" C, Humedad: ");
        Serial.print(lastHumidity/10.);
        Serial.println(" %");   
      #endif
      
      digitalWrite(IO4, HIGH); //Si7005
  }
  
  uint16_t sckReadSHT21(uint8_t type){
      uint16_t DATA = 0;
      I2c.write((uint8_t)Temperature, type); //configura el dispositivo para medir
      delay(200);
      I2c.read(Temperature, 3); //read 2 bytes      
      DATA = I2c.receive()<<8;
      DATA = (DATA|I2c.receive());
      DATA &= ~0x0003;   
      return DATA;
  }
  
   void sckGetSHT21()
   {
      digitalWrite(IO4, HIGH); //Si7005
      lastTemperature = (-53 + 175.72 / 65536.0 * (float)(sckReadSHT21(0xE3)))*10;   // formula con factor de correccion
      lastHumidity    = (7 + 125.0 / 65536.0 * (float)(sckReadSHT21(0xE5)))*10;      // formula con factor de correccion
      //lastTemperature = (-46.85 + 175.72 / 65536.0 * (float)(sckReadSHT21(0xE3)))*10;  // formula original
      //lastHumidity    = (-6.0 + 125.0 / 65536.0 * (float)(sckReadSHT21(0xE5)))*10;     // formula orginal
      
      //lastTemperature = sckReadSHT21(0xE3); // Datos en RAW para conversion por plataforma
      //lastHumidity    = sckReadSHT21(0xE5); // Datos en RAW para conversion por plataforma
      
      #if debuggSCK
        Serial.print("SHT21:  ");
        Serial.print("Temperatura: ");
        Serial.print(lastTemperature/10.);
        Serial.print(" C, Humedad: ");
        Serial.print(lastHumidity/10.);
        Serial.println(" %");    
      #endif
  }
  
 #endif
  
  uint16_t sckGetLight(){
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
        Serial.print("BH1730: ");
        Serial.print(Lx);
        Serial.println(" Lx");
      #endif
     return Lx*10;
    #else
      int temp = map(average(S5), 0, 1023, 0, 1000);
      if (temp>1000) temp=1000;
      if (temp<0) temp=0;
      return temp;
    #endif
  }
 
  
  unsigned int sckGetNoise() {
    unsigned long temp = 0;
    int n = 100;
    
    #if F_CPU == 8000000 
     sckWriteGAIN(10000);
     delay(100);
    #endif
    
    float mVRaw = (float)((average(S4))/1023.)*Vcc;
    float dB = 0;
    float GAIN = 100;
    
    #if F_CPU == 8000000 
 /*   
 
 
      if (mVRaw > 1000) 
        {
          sckWriteGAIN(1000);
          delay(100);
          mVRaw = (float)((analogRead(S4))/1023.)*Vcc;
        }
      if (mVRaw > 100) 
      {
        sckWriteGAIN(100);
        delay(100);
        mVRaw = (float)((analogRead(S4))/1023.)*Vcc;
      }
      if (mVRaw >= 1)
      {
        GAIN = sckReadGAIN();
        if (GAIN > 1000)
         {
           dB = 6.0686*log(mVRaw) + 30.43;
         }
        else if (GAIN > 100)
         {
           dB = 18.703*log(mVRaw) - 40.534;
         }
        else if (GAIN <= 100) dB = 9.8903*log(mVRaw) + 29.793; 
      }
      else dB = 30;
 */     
    #else
       dB = 9.7*log( (mVRaw*200)/1000. ) + 40;  // calibracion para ruido rosa // energia constante por octava
       if (dB<50) dB = 50; // minimo con la resolucion actual!
    #endif
    

    
    mVRaw = (float)((float)(average(S4))/1023)*Vcc;
    #if debuggSCK
      Serial.print("nOISE = ");
      Serial.print(mVRaw);
      Serial.print(" mV nOISE = ");
      Serial.print(dB);
      Serial.print(" dB, GAIN = ");
      Serial.println(GAIN);
    #endif
 
    #if F_CPU == 8000000 
       return mVRaw*100;    
    #else
       return dB*100;
    #endif
  }
  
  unsigned long sckGetCO()
  {
    #if ppmEnabled
      return (1002.7*exp(-8.476*(RsCO/RoCO)))*1000;
    #else 
      return RsCO;
    #endif
  }  
  
  unsigned long sckGetNO2()
  {
    #if ppmEnabled
     return (0.0343*pow(RsNO2/RoNO2,0.5442))*1000;
    #else
     return RsNO2;
    #endif
  } 
 
  void sckUpdateSensors(byte mode) 
 {   
  sckCheckData();
  uint16_t pos = sckReadintEEPROM(EE_ADDR_NUMBER_MEASURES);
  uint16_t MAX = 800;
  if ((mode == 2)||(mode == 4)||(pos >= MAX)) 
    {
      sckWriteintEEPROM(EE_ADDR_NUMBER_MEASURES, 0x0000);
      pos = 0;
    }  
  boolean ok_read = false; 
  byte    retry   = 0;
  
  if (pos > 0) pos = pos + 1;
  
  #if F_CPU == 8000000 
    #if SensorModel == 1
      sckGetSHT21();
    #else 
      #if SensorModel == 2
        sckGetSi7005();
      #endif
    #endif
    ok_read = true;
  #else
    #if wiflyEnabled
      timer1Stop();
    #endif
    while ((!ok_read)&&(retry<5))
    {
      ok_read = sckDHT22(IO3);
      retry++; 
      if (!ok_read)delay(3000);
    }
     #if wiflyEnabled
       timer1Initialize(); // set a timer of length 1000000 microseconds (or 1 sec - or 1Hz)
     #endif
  #endif
    if (ok_read )  
    {
      #if ((decouplerComp)&&(F_CPU > 8000000 ))
        uint16_t battery = sckGetBattery();
        decoupler.update(battery);
        sckWriteData(DEFAULT_ADDR_MEASURES, pos + 0, itoa( (int)lastTemperature - (int) decoupler.getCompensation())); // C
      #else
        sckWriteData(DEFAULT_ADDR_MEASURES, pos + 0, itoa(lastTemperature)); // C
      #endif
      sckWriteData(DEFAULT_ADDR_MEASURES, pos + 1, itoa(lastHumidity)); // %   
    }
    else 
    {
      sckWriteData(DEFAULT_ADDR_MEASURES, pos + 0, sckReadData(DEFAULT_ADDR_MEASURES, 0, 0)); // C
      sckWriteData(DEFAULT_ADDR_MEASURES, pos + 1, sckReadData(DEFAULT_ADDR_MEASURES, 1, 0)); // %
    }  
  sckWriteData(DEFAULT_ADDR_MEASURES, pos + 2, itoa(sckGetLight())); //mV
  sckWriteData(DEFAULT_ADDR_MEASURES, pos + 3, itoa(sckGetBattery())); //%
  sckWriteData(DEFAULT_ADDR_MEASURES, pos + 4, itoa(sckGetPanel()));  // %
  
  if ((mode == 3)||(mode == 4))
    { 
       sckWriteData(DEFAULT_ADDR_MEASURES, pos + 5, "0"); //ppm
       sckWriteData(DEFAULT_ADDR_MEASURES, pos + 6, "0"); //ppm
    }
  else
    {
      sckGetMICS();
      sckWriteData(DEFAULT_ADDR_MEASURES, pos + 5, itoa(sckGetCO())); //ppm
      sckWriteData(DEFAULT_ADDR_MEASURES, pos + 6, itoa(sckGetNO2())); //ppm
    }
    
  sckWriteData(DEFAULT_ADDR_MEASURES, pos + 7, itoa(sckGetNoise())); //mV
      
  if ((mode == 0)||(mode == 2)||(mode == 4))
       {
         sckWriteData(DEFAULT_ADDR_MEASURES, pos + 8, "0");  //Wifi Nets
         sckWriteData(DEFAULT_ADDR_MEASURES, pos + 9, sckRTCtime());
       } 
}


