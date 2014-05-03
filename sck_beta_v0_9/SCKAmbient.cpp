/*

  SCKAmbient.cpp
  Supports the sensor reading and calibration functions.

  - Sensors supported (sensors use on board custom peripherials):

    - TEMP / HUM (DHT22 and HPP828E031)
    - NOISE
    - LIGHT (LDR and BH1730FVC)
    - CO (MICS5525 and MICS4514)
    - NO2 (MiCS2710 and MICS4514)
    - ADXL345 (Only on some models)

*/


#include "Constants.h"
#include "SCKAmbient.h"
#include "SCKBase.h"
#include "SCKServer.h"
#include <Wire.h>
#include <EEPROM.h>

/* 

SENSOR Contants and Defaults

*/

#if ((decouplerComp)&&(F_CPU > 8000000 ))
  #include "TemperatureDecoupler.h"
  TemperatureDecoupler decoupler; // Compensate the bat .charger generated heat affecting temp values
#endif


#define USBEnabled      true 
#define autoUpdateWiFly true
#define debugAmbient    false

SCKBase base_;
SCKServer server_;
SCKAmbient ambient_;
  
  
  long value[SENSORS];
  char time[TIME_BUFFER_SIZE];
  boolean wait_moment;
  boolean debugON = true;   
#if F_CPU == 8000000 
  float    Vcc = 3300.; //mV 
#else
  float    Vcc = 5000.; //mV 
#endif

// MICS (Gas Sensors) Ro Default Value (Ohm)
float RoCO  = 750000;
float RoNO2 = 2200;


// MICS (Gas Sensors) RS Value (Ohm)                    
float RsCO = 0;
float RsNO2 = 0;


#if F_CPU == 8000000 
  uint32_t lastHumidity;
  uint32_t lastTemperature;
  int accel_x=0;
  int accel_y=0;
  int accel_z=0;
#else
  int lastHumidity;
  int lastTemperature;
#endif

void SCKAmbient::begin() {
  base_.begin();
  #if ((decouplerComp)&&(F_CPU > 8000000 ))
    decoupler.setup();
  #endif
  base_.config();
  #if F_CPU == 8000000 
    base_.writeCharge(350);
  #endif
  #if F_CPU == 8000000   
    writeVH(MICS_5525, 2700);    // MICS5525_START
    digitalWrite(IO0, HIGH);     // MICS5525
  
    writeVH(MICS_2710, 1700);    // MICS2710_START
    digitalWrite(IO1, HIGH);     // MICS2710_HEATHER
    digitalWrite(IO2, LOW);      // MICS2710_HIGH_IMPEDANCE
  
    pinMode(IO3, OUTPUT);
    digitalWrite(IO3, HIGH);     // MICS POWER LINE 
    writeADXL(0x2D, 0x08);
    //  WriteADXL(0x31, 0x00); //2g
    //  WriteADXL(0x31, 0x01); //4g
    writeADXL(0x31, 0x02); //8g
    //  WriteADXL(0x31, 0x03); //16g
  #else
    writeVH(MICS_5525, 2400);    // MICS5525_START
    digitalWrite(IO0, HIGH);     // MICS5525
  
    writeVH(MICS_2710, 1700);    // MICS2710_START
    digitalWrite(IO1, HIGH);     // MICS2710
    digitalWrite(IO2, LOW);      // MICS2710_HIGH_IMPEDANCE
  #endif
  
    writeRL(MICS_5525, 100000);  // START LOADING MICS5525
    writeRL(MICS_2710, 100000);  // START LOADING MICS2710

}

/* 

GLOBAL SETUP

*/

boolean terminal_mode = false;
boolean usb_mode      = false;
byte      sensor_mode    = NORMAL;
uint32_t  TimeUpdate   = 0;  // Sensor Readings time interval in sec.
uint32_t  NumUpdates   = 0;  // Min. number of sensor readings before publishing
uint32_t  nets           = 0;
boolean sleep         = true; 
uint32_t timetransmit = 0; 
uint32_t timeMICS = 0; 

void SCKAmbient::ini()
  {
    debugON = false;
    /*init WiFly*/
    digitalWrite(AWAKE, HIGH); 
    sensor_mode = sensor_mode = base_.readData(EE_ADDR_SENSOR_MODE, INTERNAL);  //Normal mode
    TimeUpdate = base_.readData(EE_ADDR_TIME_UPDATE, INTERNAL);    //Time between transmissions in sec.
    NumUpdates = base_.readData(EE_ADDR_NUMBER_UPDATES, INTERNAL); //Number of readings before batch update
    nets = base_.readData(EE_ADDR_NUMBER_NETS, INTERNAL);
    if (TimeUpdate*NumUpdates < 60) sleep = false;
    else sleep = true; 
    if (nets==0)
    {
      sleep = false;  
      sensor_mode = OFFLINE;    //Offline mode, no networks un memory
      base_.repair();           //Repairs wifi if corruption
      base_.APmode(base_.id()); //Starts as acces point
    #if debugEnabled
        if (!debugON) Serial.println(F("AP initialized!"));
    #endif 
    }
    else
    {
      if (base_.connect())
      {
        #if debugEnabled
          if (!debugON) Serial.println(F("SCK Connected!!"));
        #endif
        #if autoUpdateWiFly
          int report = base_.checkWiFly();
          #if debugEnabled
            if (!debugON)
              {
                  if (report == 1) Serial.println(F("Wifly Updated."));
                  else if (report == 2) Serial.println(F("Update Fail."));
                  else if (report == 0) Serial.println(F("WiFly up to date."));
                  else if (report == -1) Serial.println(F("Error reading the wifi version."));
              }
          #endif
        #endif
        byte retry = 0;
        if (base_.checkRTC())
        {
          if (server_.time(time))
          {
            while (!base_.RTCadjust(time)&&(retry<5)) retry++;
            #if debugEnabled
                    if (!debugON) Serial.println(F("Updating RTC..."));
            #endif
          }
          #if debugEnabled
            else if (!debugON) Serial.println(F("Fail updating RTC!!"));
          #endif
        }
      }
    }  
    if(sleep)
    {
      base_.sleep();
      #if debugEnabled
          if (!debugON) Serial.println(F("SCK Sleeping...")); 
      #endif
      digitalWrite(AWAKE, LOW); 
    }   
    timetransmit = millis();
    wait_moment = false;
    timeMICS = millis();
  }  
  
  float k= (RES*(float)R1/100)/1000; //Voltatge Constant for the Voltage reg.
 
  
  /* 

    SENSOR Functions

  */     
  void SCKAmbient::writeVH(byte device, long voltage ) {
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
      base_.writeMCP(MCP1, device, data);
    #else
      base_.writeMCP(MCP2, device, data);
    #endif
  }

  
  float SCKAmbient::readVH(byte device) {
    int data;
    #if F_CPU == 8000000 
      data=base_.readMCP(MCP1, device);
      float voltage = (data/k + 1000)*0.41;
    #else
      data=base_.readMCP(MCP2, device);
      float voltage = (data/k + 1000)*1.2;
    #endif
    
    return(voltage);
  }
  
  float kr1= ((float)P1*1000)/RES;    //  Resistance conversion Constant for the digital pot.
  
  void SCKAmbient::writeRL(byte device, long resistor) {
    int data=0x00;
    data = (int)(resistor/kr1);
    #if F_CPU == 8000000 
      base_.writeMCP(MCP1, device + 6, data);
    #else
      base_.writeMCP(MCP1, device, data);
    #endif
  }

  float SCKAmbient::readRL(byte device)
  {
    #if F_CPU == 8000000 
      return (kr1*base_.readMCP(MCP1, device + 6)); // Returns Resistance (Ohms)
    #else
      return (kr1*base_.readMCP(MCP1, device));     // Returns Resistance (Ohms)
    #endif 
  }

  void SCKAmbient::writeRGAIN(byte device, long resistor) {
    int data=0x00;
    data = (int)(resistor/kr1);
    base_.writeMCP(MCP2, device, data);
  }
  
  float SCKAmbient::readRGAIN(byte device)
  {
      return (kr1*base_.readMCP(MCP2, device));    // Returns Resistance (Ohms)
  }

  void SCKAmbient::writeGAIN(long value)
  {
    if (value == 100)
    {
      writeRGAIN(0x00, 10000);
      writeRGAIN(0x01, 10000);
    }
    else if (value == 1000)
    {
      writeRGAIN(0x00, 10000);
      writeRGAIN(0x01, 100000);
    }
    else if (value == 10000)
          {
             writeRGAIN(0x00, 100000);
             writeRGAIN(0x01, 100000);
          }
    delay(100);
  }

  float SCKAmbient::readGAIN()
  {
    return (readRGAIN(0x00)/1000)*(readRGAIN(0x01)/1000);
  }    

  void SCKAmbient::getVcc()
  {
    float temp = base_.average(S3);
    analogReference(INTERNAL);
    delay(100);
    Vcc = (float)(base_.average(S3)/temp)*reference;
    analogReference(DEFAULT);
    delay(100);
  }
  
  void SCKAmbient::heat(byte device, int current)
  {
    float Rc=Rc0;
    byte Sensor = S2;
    if (device == MICS_2710) { Rc=Rc1; Sensor = S3;}

    float Vc = (float)base_.average(Sensor)*Vcc/1023; //mV 
    float current_measure = Vc/Rc; //mA 
    float Rh = (readVH(device)- Vc)/current_measure;
    float Vh = (Rh + Rc)*current;

    writeVH(device, Vh);
      #if debugAmbient
        if (device == MICS_2710) Serial.print("MICS2710 current: ");
        else Serial.print("MICS5525 current: ");
        Serial.print(current_measure);
        Serial.println(" mA");
        if (device == MICS_2710) Serial.print("MICS2710 correction VH: ");
        else  Serial.print("MICS5525 correction VH: ");
        Serial.print(readVH(device));
        Serial.println(" mV");
        Vc = (float)average(Sensor)*Vcc/1023; //mV 
        current_measure = Vc/Rc; //mA 
        if (device == MICS_2710) Serial.print("MICS2710 current adjusted: ");
        else Serial.print("MICS5525 current adjusted: ");
        Serial.print(current_measure);
        Serial.println(" mA");
        Serial.println("Heating...");
      #endif
    
  }

   float SCKAmbient::readRs(byte device)
   {
     byte Sensor = S0;
     float VMICS = VMIC0;
     if (device == MICS_2710) {Sensor = S1; VMICS = VMIC1;}
     float RL = readRL(device); //Ohm
     float VL = ((float)base_.average(Sensor)*Vcc)/1023; //mV
     if (VL > VMICS) VL = VMICS;
     float Rs = ((VMICS-VL)/VL)*RL; //Ohm
     #if debugAmbient
        if (device == MICS_5525) Serial.print("MICS5525 Rs: ");
        else Serial.print("MICS2710 Rs: ");
        Serial.print(VL);
        Serial.print(" mV, ");
        Serial.print(Rs);
        Serial.println(" Ohm");
      #endif
     return Rs;
   }
   
  float SCKAmbient::readMICS(byte device)
  {
      float Rs = readRs(device);
      float RL = readRL(device); //Ohm
      
      // Charging impedance correction
      if ((Rs <= (RL - 1000))||(Rs >= (RL + 1000)))
      {
        if (Rs < 2000) writeRL(device, 2000);
        else writeRL(device, Rs);
        delay(100);
        Rs = readRs(device);
      }
       return Rs;
  }
  
  void SCKAmbient::GasSensor(boolean active)
  {
    if (active)
      {
        #if F_CPU == 8000000   
          digitalWrite(IO0, HIGH);     // MICS5525
          digitalWrite(IO1, HIGH);     // MICS2710_HEATHER
          digitalWrite(IO2, LOW);      // MICS2710_HIGH_IMPEDANCE
          digitalWrite(IO3, HIGH);     // MICS POWER LINE 
        #else
          digitalWrite(IO0, HIGH);     // MICS5525
          digitalWrite(IO1, HIGH);     // MICS2710
          digitalWrite(IO2, LOW);      // MICS2710_HIGH_IMPEDANCE
        #endif
      }
    else 
      {
        #if F_CPU == 8000000   
          digitalWrite(IO0, LOW);     // MICS5525
          digitalWrite(IO1, LOW);     // MICS2710_HEATHER
          digitalWrite(IO2, LOW);      // MICS2710_HIGH_IMPEDANCE
          digitalWrite(IO3, LOW);     // MICS POWER LINE 
        #else
          digitalWrite(IO0, LOW);     // MICS5525
          digitalWrite(IO1, HIGH);     // MICS2710
          digitalWrite(IO2, LOW);      // MICS2710_HIGH_IMPEDANCE
        #endif
      }
  }
  
  void SCKAmbient::getMICS(){          
       
        // Charging tension heaters
        heat(MICS_5525, 32); //Corriente en mA
        heat(MICS_2710, 26); //Corriente en mA
        
        RsCO = readMICS(MICS_5525);
        RsNO2 = readMICS(MICS_2710);
         
  }

 #if F_CPU == 8000000
   uint16_t SCKAmbient::readSHT21(uint8_t type){
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
  
   void SCKAmbient::getSHT21()
   {
        lastTemperature = readSHT21(0xE3);  // RAW DATA for calibration in platform
        lastHumidity    = readSHT21(0xE5);  // RAW DATA for calibration in platform
      #if debugAmbient
        Serial.print("SHT21:  ");
        Serial.print("Temperature: ");
        Serial.print(lastTemperature/10.);
        Serial.print(" C, Humidity: ");
        Serial.print(lastHumidity/10.);
        Serial.println(" %");    
      #endif
    }
    
    void SCKAmbient::writeADXL(byte address, byte val) {
       Wire.beginTransmission(ADXL);    // Start transmission to device 
       Wire.write(address);             // Write register address
       Wire.write(val);                 // Write value to write
       Wire.endTransmission();          // End transmission
    }
    
    //reads num bytes starting from address register on device in to buff array
    void SCKAmbient::readADXL(byte address, int num, byte buff[]) {
      Wire.beginTransmission(ADXL);     //start transmission to device 
      Wire.write(address);              //writes address to read from
      Wire.endTransmission();           //end transmission
      
      Wire.beginTransmission(ADXL);     //start transmission to device
      Wire.requestFrom(ADXL, num);      // request 6 bytes from device
      
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
      while(Wire.available())            //device may write less than requested (abnormal)
      { 
        buff[i] = Wire.read();           // read a byte
        i++;
      }
      Wire.endTransmission();            //end transmission
    }
    
    void SCKAmbient::averageADXL()
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
        readADXL(0x32, 6, buffADXL); //read the acceleration data from the ADXL345
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
      
      #if debugAmbient
        Serial.print("x_axis= ");
        Serial.print(accel_x);
        Serial.print(", ");
        Serial.print("y_axis= ");
        Serial.print(accel_y);
        Serial.print(", ");
        Serial.print("z_axis= ");
        Serial.println(accel_z); 
      #endif
    }
 #else
    uint8_t bits[5];  // buffer to receive data
    
    #define TIMEOUT 10000
    
    boolean SCKAmbient::getDHT22()
    {
            // Read Values
            int rv = DhtRead(IO3);
            if (rv != true)
            {
                  lastHumidity    = DHTLIB_INVALID_VALUE;  // invalid value, or is NaN prefered?
                  lastTemperature = DHTLIB_INVALID_VALUE;  // invalid value
                  return rv;
            }
    
            // Convert and Store
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
    
            // Test Checksum
            uint8_t sum = bits[0] + bits[1] + bits[2] + bits[3];
            if (bits[4] != sum) return false;
            if ((lastTemperature == 0)&&(lastHumidity == 0))return false;
            return true;
    }
    
    boolean SCKAmbient::DhtRead(uint8_t pin)
    {
            // init Buffer to receive data
            uint8_t cnt = 7;
            uint8_t idx = 0;
    
            // empty the buffer
            for (int i=0; i< 5; i++) bits[i] = 0;
    
            // request the sensor
            pinMode(pin, OUTPUT);
            digitalWrite(pin, LOW);
            delay(20);
            digitalWrite(pin, HIGH);
            delayMicroseconds(40);
            pinMode(pin, INPUT);
    
            // get ACK or timeout
            unsigned int loopCnt = TIMEOUT;
            while(digitalRead(pin) == LOW)
                    if (loopCnt-- == 0) return false;
    
            loopCnt = TIMEOUT;
            while(digitalRead(pin) == HIGH)
                    if (loopCnt-- == 0) return false;
    
            // read Ouput - 40 bits => 5 bytes
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
  
  uint16_t SCKAmbient::getLight(){
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
      
       #if debugAmbient
        Serial.print("BH1730: ");
        Serial.print(Lx);
        Serial.println(" Lx");
      #endif
     return Lx*10;
    #else
      int temp = map(base_.average(S5), 0, 1023, 0, 1000);
      if (temp>1000) temp=1000;
      if (temp<0) temp=0;
      return temp;
    #endif
  }
 
  
  unsigned int SCKAmbient::getNoise() {  
    
    #if F_CPU == 8000000 
     #define GAIN 10000
     writeGAIN(GAIN);
     delay(100);
    #endif
    float mVRaw = (float)((base_.average(S4))/1023.)*Vcc;
    float dB = 0;
    return mVRaw;  
  }
  
  unsigned long SCKAmbient::getCO()
  {
    return RsCO;
  }  
  
  unsigned long SCKAmbient::getNO2()
  {
    return RsNO2;
  } 
  
  #if F_CPU == 8000000 
    uint32_t SCKAmbient::getTemperature()
    {
      return lastTemperature;
    }  
    
    uint32_t SCKAmbient::getHumidity()
    {
      return lastHumidity;
    } 
  #else
    int SCKAmbient::getTemperature()
    {
      return lastTemperature;
    }  
    
    int SCKAmbient::getHumidity()
    {
      return lastHumidity;
    } 
  #endif
    
  void SCKAmbient::updateSensors(byte mode) 
   {   
      boolean ok_read = false; 
      byte    retry   = 0;
      
      #if F_CPU == 8000000 
        getSHT21();
        ok_read = true;
      #else
        base_.timer1Stop();
        while ((!ok_read)&&(retry<5))
        {
          ok_read = getDHT22();
          retry++; 
          if (!ok_read)delay(3000);
        }
        base_.timer1Initialize(); 
      #endif
        if (((millis()-timeMICS)<=6*minute)||(mode!=ECONOMIC))  //6 minutes
        {  
          #if F_CPU == 8000000 
            getVcc();
          #endif
          getMICS();
          value[5] = getCO(); //ppm
          value[6] = getNO2(); //ppm
        }
        else if((millis()-timeMICS)>=60*minute) 
              {
                GasSensor(true);
                timeMICS = millis();
              }
        else
        {
          GasSensor(false);
        }
        
        if (ok_read )  
        {
          #if ((decouplerComp)&&(F_CPU > 8000000 ))
            uint16_t battery = base_.getBattery(Vcc);
            decoupler.update(battery);
            value[0] = getTemperature() - (int) decoupler.getCompensation();
          #else
            value[0] = getTemperature();
          #endif
           value[1] = getHumidity();
        }
        else 
        {
          value[0] = 0; // ºC
          value[1] = 0; // %
        }  
        value[2] = getLight(); //mV
        value[3] = base_.getBattery(Vcc); //%
        value[4] = base_.getPanel(Vcc);  // %
        value[7] = getNoise(); //mV     
        if (mode == NOWIFI)
             {
               value[8] = 0;  //Wifi Nets
               base_.RTCtime(time);
             } 
        else if (mode == OFFLINE)
          {
            value[8] = base_.scan();  //Wifi Nets
            base_.RTCtime(time);
          }
   }
  
boolean SCKAmbient::debug_state()
  {
    return debugON;
  }

void SCKAmbient::execute()
  {
    if (terminal_mode)  // Telnet  (#data + *OPEN* detectado )
    {
      sleep = false;
      digitalWrite(AWAKE, HIGH);
      server_.json_update(0, value, time, true);
      usb_mode = false;
      terminal_mode = false;
    }
    if ((millis()-timetransmit) >= (unsigned long)TimeUpdate*second)
    {  
      timetransmit = millis();
      TimeUpdate = base_.readData(EE_ADDR_TIME_UPDATE, INTERNAL);    // Time between transmissions in sec.
      NumUpdates = base_.readData(EE_ADDR_NUMBER_UPDATES, INTERNAL); // Number of readings before batch update
      updateSensors(sensor_mode); 
      if (!debugON)                                                  // CMD Mode False
      {
        if ((sensor_mode)>NOWIFI) server_.send(sleep, &wait_moment, value, time);
        #if USBEnabled
              txDebug();
        #endif
      }
    }
  }       
           
void SCKAmbient::txDebug() {
  if (debugON== false) {
    float dec = 0;
    for(int i=0; i<9; i++) 
    {
      #if F_CPU == 8000000 
        if (i<2) dec = 1;
        else if (i<4) dec = 10;
        else if (i<5) dec = 1;
        else if (i<7) dec = 1000;
        else if (i<8) dec = 1;
      #else
        if (i<4) dec = 10;
        else if (i<5) dec = 1;
        else if (i<7) dec = 1000;
        else if (i<8) dec = 1;
      #endif
        else dec = 1;
      Serial.print(SENSOR[i]); 
      if (dec>1) Serial.print((float)(value[i]/dec)); 
      else Serial.print((int)(value[i]/dec)); 
      Serial.println(UNITS[i]);
    }
    Serial.print(SENSOR[9]);
    Serial.println(time);
    Serial.println(F("*******************"));    
  } 
}



static char buffer_int[buffer_length];
byte count_char = 0;

boolean SCKAmbient::addData(byte inByte)
  {
    if (inByte == '\r')  
      {
         buffer_int[count_char] = inByte;
         buffer_int[count_char + 1] = 0x00;
         count_char = 0;
         return true;
      }
    else if((inByte != '\n')&&(inByte != '#')&&(inByte != '$'))
      {
        buffer_int[count_char] = inByte;
        count_char = count_char + 1;
        return false;
      }
    else if ((inByte == '#')||(inByte == '$'))
      {
        buffer_int[count_char] = inByte;
        count_char = count_char + 1;
        if (count_char == 3) 
          {
            buffer_int[count_char] = 0x00;
            count_char = 0;
            return true;
          }
      } 
    return false;
  }

    
boolean SCKAmbient::printNetWorks(unsigned int address_eeprom)
    {
      int nets_temp = base_.readData(EE_ADDR_NUMBER_NETS, INTERNAL);
      if (nets_temp>0){
        for (int i = 0; i<nets_temp; i++)
          {
            Serial.print(base_.readData(address_eeprom, i, INTERNAL));
            if (i<(nets_temp - 1)) Serial.print(' ');
          }
        Serial.println();}
    }  

void SCKAmbient::addNetWork(unsigned int address_eeprom, char* text)
    {
      int pos = 0;
      int nets_temp = base_.readData(EE_ADDR_NUMBER_NETS, INTERNAL);
      if (address_eeprom < DEFAULT_ADDR_PASS)
        {
          nets_temp = nets_temp + 1;
          if (nets_temp<=5) base_.writeData(EE_ADDR_NUMBER_NETS, nets_temp , INTERNAL); 
        }
      if (nets_temp<=5)
        {
          if (nets_temp == 0) pos = 0;
          else pos = nets_temp - 1;
          base_.writeData(address_eeprom, pos, text, INTERNAL);
        }
    } 
    
    
boolean serial_bridge = false;
boolean eeprom_write_ok      = false;
boolean text_write           = true;
unsigned int address_eeprom  = 0;
int temp_mode = NORMAL;

void SCKAmbient::serialRequests()
  {
    sei();
    base_.timer1Stop();
    #if F_CPU == 8000000 
      if (!digitalRead(CONTROL))
      {
        digitalWrite(AWAKE, HIGH);
        digitalWrite(FACTORY, HIGH);
        sleep = false;  
        sensor_mode = OFFLINE; //Offline mode or acces point mode
      }
      else digitalWrite(AWAKE, LOW);
    #endif
      if (Serial.available())
      {
        byte inByte = Serial.read();
        if (addData(inByte)) 
          {
            if (base_.checkText("###", buffer_int)) { debugON= true; temp_mode = sensor_mode; sensor_mode = OFFLINE; } //Serial.println(F("AOK"));} //Terminal SCK ON
            else if (base_.checkText("exit", buffer_int)) {
              Serial.println(F("EXIT"));
              serial_bridge = false;
              sensor_mode = temp_mode;
              debugON= false;
            }
            else if (base_.checkText("$$$", buffer_int))  //Terminal WIFI ON
            {
              digitalWrite(AWAKE, HIGH); 
              delayMicroseconds(100);
              digitalWrite(AWAKE, LOW);
              temp_mode = sensor_mode;
              sensor_mode = NOWIFI;
              if (!wait_moment) serial_bridge = true;
              else Serial.println(F("Please, wait wifly sleep"));
              debugON= true;
            }
            /*Reading commands*/
            else if (base_.checkText("get sck info\r", buffer_int))           Serial.println(FirmWare);
            else if (base_.checkText("get wifi info\r", buffer_int))          Serial.println(base_.getWiFlyVersion());
            else if (base_.checkText("get mac\r", buffer_int))                Serial.println(base_.readData(EE_ADDR_MAC, 0, INTERNAL));
            else if (base_.checkText("get wlan ssid\r", buffer_int))          printNetWorks(DEFAULT_ADDR_SSID);
            else if (base_.checkText("get wlan phrase\r", buffer_int))        printNetWorks(DEFAULT_ADDR_PASS);
            else if (base_.checkText("get wlan auth\r", buffer_int))          printNetWorks(DEFAULT_ADDR_AUTH);
            else if (base_.checkText("get wlan ext_antenna\r", buffer_int))   printNetWorks(DEFAULT_ADDR_ANTENNA);
            else if (base_.checkText("get mode sensor\r", buffer_int))        Serial.println(base_.readData(EE_ADDR_SENSOR_MODE, INTERNAL));
            else if (base_.checkText("get time update\r", buffer_int))        Serial.println(base_.readData(EE_ADDR_TIME_UPDATE, INTERNAL));
            else if (base_.checkText("get number updates\r", buffer_int))     Serial.println(base_.readData(EE_ADDR_NUMBER_UPDATES, INTERNAL));
            else if (base_.checkText("get apikey\r", buffer_int))             Serial.println(base_.readData(EE_ADDR_APIKEY, 0, INTERNAL));
            /*Write commands*/
            else if (base_.checkText("set wlan ssid ", buffer_int))
            {
                addNetWork(DEFAULT_ADDR_SSID, buffer_int);
                sensor_mode = base_.readData(EE_ADDR_SENSOR_MODE, INTERNAL); 
                if (TimeUpdate < 60) sleep = false;
                else sleep = true; 
            }
            else if (base_.checkText("set wlan phrase ", buffer_int)) addNetWork(DEFAULT_ADDR_PASS, buffer_int);
            else if (base_.checkText("set wlan key ", buffer_int)) addNetWork(EE_ADDR_NUMBER_NETS, buffer_int);
            else if (base_.checkText("set wlan ext_antenna ", buffer_int))  addNetWork(DEFAULT_ADDR_ANTENNA, buffer_int);
            else if (base_.checkText("set wlan auth ", buffer_int)) addNetWork(DEFAULT_ADDR_AUTH, buffer_int);
            else if (base_.checkText("clear nets\r", buffer_int)) base_.writeData(EE_ADDR_NUMBER_NETS, 0x0000, INTERNAL);
            else if (base_.checkText("set mode sensor ", buffer_int)) base_.writeData(EE_ADDR_SENSOR_MODE, atol(buffer_int), INTERNAL);
            else if (base_.checkText("set time update ", buffer_int)) base_.writeData(EE_ADDR_TIME_UPDATE, atol(buffer_int), INTERNAL);
            else if (base_.checkText("set number updates ", buffer_int)) base_.writeData(EE_ADDR_NUMBER_UPDATES, atol(buffer_int), INTERNAL);
            else if (base_.checkText("set apikey ", buffer_int)){
              eeprom_write_ok = true;
              address_eeprom = EE_ADDR_APIKEY;
            } 
          }
        if (serial_bridge) Serial1.write(inByte); 
      }
      else if (serial_bridge)
      {
        if (Serial1.available()) 
        {
          byte inByte = Serial1.read();
          Serial.write(inByte);
        }
       }
      base_.timer1Initialize(); // set a timer of length 1000000 microseconds (or 1 sec - or 1Hz)  
  }
  
ISR(TIMER1_OVF_vect)
{
  ambient_.serialRequests();
}

