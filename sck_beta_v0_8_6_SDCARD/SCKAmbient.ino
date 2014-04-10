
//Valores por defecto de la resistencia en vacio de los MICS
float RoCO  = 750000;
float RoNO2 = 2200;

#if ((decouplerComp)&&(F_CPU > 8000000 ))
  #include "TemperatureDecoupler.h"
  TemperatureDecoupler decoupler; //use this object to compensate for charger generated heat affecting temp values
#endif
                   
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

#if F_CPU == 8000000 
  uint16_t lastHumidity;
  uint16_t lastTemperature;
  int accel_x=0;
  int accel_y=0;
  int accel_z=0;
#else
  int lastHumidity;
  int lastTemperature;
#endif



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

  void sckGetVcc()
  {
    float temp = average(S3);
    analogReference(INTERNAL);
    delay(100);
    Vcc = (float)(average(S3)/temp)*reference;
    analogReference(DEFAULT);
    delay(100);
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
   uint16_t sckReadSHT21(uint8_t type){
      uint16_t DATA = 0;
      Wire.beginTransmission(Temperature);
      Wire.write(type);
      Wire.endTransmission();
      Wire.requestFrom(Temperature,2);
      unsigned long time = millis();
      while (!Wire.available()) if ((millis() - time)>500) return 0x00;
      DATA = Wire.read()<<8; 
      while (!Wire.available()); 
      DATA = (DATA|Wire.read()); 
      DATA &= ~0x0003; 
      return DATA;
  }
  
   void sckGetSHT21()
   {
      #if DataRaw
        lastTemperature = sckReadSHT21(0xE3); // Datos en RAW para conversion por plataforma
        lastHumidity    = sckReadSHT21(0xE5); // Datos en RAW para conversion por plataforma
      #else
        lastTemperature = (-46.85 + 175.72 / 65536.0 * (float)(sckReadSHT21(0xE3)))*10;  // formula original
        lastHumidity    = (-6.0 + 125.0 / 65536.0 * (float)(sckReadSHT21(0xE5)))*10;     // formula orginal      
      #endif
      
      #if debuggSCK
        Serial.print("SHT21:  ");
        Serial.print("Temperatura: ");
        Serial.print(lastTemperature/10.);
        Serial.print(" C, Humedad: ");
        Serial.print(lastHumidity/10.);
        Serial.println(" %");    
      #endif
    }
    
    void sckWriteADXL(byte address, byte val) {
       Wire.beginTransmission(ADXL); //start transmission to device 
       Wire.write(address);        // write register address
       Wire.write(val);        // write value to write
       Wire.endTransmission(); //end transmission
    }
    
    //reads num bytes starting from address register on device in to buff array
    void sckrReadADXL(byte address, int num, byte buff[]) {
      Wire.beginTransmission(ADXL); //start transmission to device 
      Wire.write(address);        //writes address to read from
      Wire.endTransmission(); //end transmission
      
      Wire.beginTransmission(ADXL); //start transmission to device
      Wire.requestFrom(ADXL, num);    // request 6 bytes from device
      
      int i = 0;
      unsigned long time = millis();
      while (!Wire.available()) 
      {
        if ((millis() - time)>500) 
        {
          for(int i=0; i<num; i++) buff[i]=0x00;
          break;
        }
      }
      while(Wire.available())    //device may write less than requested (abnormal)
      { 
        buff[i] = Wire.read(); // read a byte
        i++;
      }
      Wire.endTransmission(); //end transmission
    }
    
    void sckAverageADXL()
    {
      #define lim 512
      int temp_x=0;
      int temp_y=0;
      int temp_z=0;
      int lecturas=10;
      byte buffADXL[6] ;    //6 bytes buffer for saving data read from the device
      accel_x=0;
      accel_y=0;
      accel_z=0;
      
      for(int i=0; i<lecturas; i++)
      {
        sckrReadADXL(0x32, 6, buffADXL); //read the acceleration data from the ADXL345
        temp_x = (((int)buffADXL[1]) << 8) | buffADXL[0]; 
        temp_x = map(temp_x,-lim,lim,0,1023);  
        temp_y = (((int)buffADXL[3])<< 8) | buffADXL[2];
        temp_y = map(temp_y,-lim,lim,0,1023); 
        temp_z = (((int)buffADXL[5]) << 8) | buffADXL[4];
        temp_z = map(temp_z,-lim,lim,0,1023); 
        accel_x = (int)(temp_x + accel_x);
        accel_y = (int)(temp_y + accel_y);
        accel_z = (int)(temp_z + accel_z);
      }
      accel_x = (int)(accel_x / lecturas);
      accel_y = (int)(accel_y / lecturas);
      accel_z = (int)(accel_z / lecturas);
      
      #if debuggSCK
        Serial.print("eje_x= ");
        Serial.print(accel_x);
        Serial.print(", ");
        Serial.print("eje_y= ");
        Serial.print(accel_y);
        Serial.print(", ");
        Serial.print("eje_z= ");
        Serial.println(accel_z);  
      #endif
    }
 #else
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
 #endif
  
  uint16_t sckGetLight(){
    #if F_CPU == 8000000 
      uint8_t TIME0  = 0xDA;
      uint8_t GAIN0 = 0x00;
      uint8_t DATA [8] = {0x03, TIME0, 0x00 ,0x00, 0x00, 0xFF, 0xFF ,GAIN0} ;
      
      uint16_t DATA0 = 0;
      uint16_t DATA1 = 0;
      
      Wire.beginTransmission(bh1730);
      Wire.write(0x80|0x00);
      for(int i= 0; i<8; i++) Wire.write(DATA[i]);
      Wire.endTransmission();
      delay(100); 
      Wire.beginTransmission(bh1730);
      Wire.write(0x94);	
      Wire.endTransmission();
      Wire.requestFrom(bh1730, 4);
      DATA0 = Wire.read();
      DATA0=DATA0|(Wire.read()<<8);
      DATA1 = Wire.read();
      DATA1=DATA1|(Wire.read()<<8);
        
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
    
    #if F_CPU == 8000000 
     #define GAIN 10000
     sckWriteGAIN(GAIN);
     delay(100);
    #endif
    
    float mVRaw = (float)((average(S4))/1023.)*Vcc;
    float dB = 0;
    
    #if F_CPU == 8000000 
      #if DataRaw==false
        dB = 0.0222*mVRaw + 58.006; 
      #endif
    #else
      #if DataRaw==false
       dB = 9.7*log( (mVRaw*200)/1000. ) + 40;  // calibracion para ruido rosa // energia constante por octava
       if (dB<50) dB = 50; // minimo con la resolucion actual!
      #endif
    #endif
    
    #if debuggSCK
      Serial.print("nOISE = ");
      Serial.print(mVRaw);
      #if DataRaw==false
        Serial.print(" mV nOISE = ");
        Serial.print(dB);
        Serial.print(" dB, GAIN = ");
      #else 
        Serial.print(" mV GAIN = ");
      #endif
      Serial.println(GAIN);
    #endif
 
    #if DataRaw
      return mVRaw; 
    #else
      return dB*100;
    #endif   
  }
  
  unsigned long sckGetCO()
  {
    return RsCO;
  }  
  
  unsigned long sckGetNO2()
  {
    return RsNO2;
  } 


