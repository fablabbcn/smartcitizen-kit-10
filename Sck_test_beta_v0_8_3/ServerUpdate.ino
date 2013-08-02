#if wiflyEnabled

void txWiFly() {  
  
  uint16_t updates = (sck.readintEEPROM(EE_ADDR_NUMBER_MEASURES) + 3)/10; 
  boolean ok_sleep = false;
  
  if ((sleep)&&(updates>=MIN_UPDATES)){
    #if debuggEnabled
      Serial.println(F("SCK Waking up..."));
    #endif
    ok_sleep = true;
    digitalWrite(AWAKE, HIGH);
  }
  if ((sck.connect())&&(updates>=MIN_UPDATES))
   { 
      #if debuggEnabled
        Serial.println(F("SCK Connected!!")); 
      #endif   
      if (sck.server_connect())
      {
        uint16_t initial = 0; 
        #if debuggEnabled
          Serial.print(F("updates = "));
          Serial.println(updates-initial);
        #endif
        sck.json_update(initial);
        initial = initial + POST_MAX;
        #if debuggEnabled
          Serial.println(F("Posted to Server!")); 
        #endif
        while (updates > initial){
          sck.server_reconnect();
          #if debuggEnabled
            Serial.print(F("updates = "));
            Serial.println(updates-initial);
          #endif
          sck.json_update(initial);
          initial = initial + POST_MAX;
          #if debuggEnabled
            Serial.println(F("Posted to Server!")); 
          #endif
        }
       sck.writeintEEPROM(EE_ADDR_NUMBER_MEASURES, 0x0000);
      }
      else {
        #if debuggEnabled
          Serial.println(F("Error posting on Server..!"));
        #endif
      }
      if (sck.isConnected()) {
        #if debuggEnabled
          Serial.println(F("Old connection active. Closing..."));
        #endif
        sck.close();
      }
    }
   else 
    {
     uint16_t pos = sck.readintEEPROM(EE_ADDR_NUMBER_MEASURES);
     
     if (ok_sleep) sck.writeData(DEFAULT_ADDR_MEASURES, pos + 1, sck.scan());  //Wifi Nets
     else sck.writeData(DEFAULT_ADDR_MEASURES, pos + 1, "0");  //Wifi Nets
     
     if (sck.checkRTC()) 
       {
         sck.writeData(DEFAULT_ADDR_MEASURES, pos + 2, sck.RTCtime());
       }
     sck.checkData();//No hace falta que se guarda en memoria si falla la RTC
     #if debuggEnabled
       Serial.print(F("updates = "));
       Serial.println((sck.readintEEPROM(EE_ADDR_NUMBER_MEASURES)+1)/10);
       if (((sck.readintEEPROM(EE_ADDR_NUMBER_MEASURES) + 3)/10)>=MIN_UPDATES) Serial.println(F("Error in connectionn!!"));
       else Serial.println(F("Saved in memory!!"));
     #endif
    }
  
  
  if ((sleep)&&(ok_sleep))
  {
    sck.sleep();
    #if debuggEnabled
      Serial.println(F("SCK Sleeping")); 
      Serial.println(F("*******************"));
    #endif
    digitalWrite(AWAKE, LOW); 
  }
}

#endif

#if SDEnabled
void txSD() {
  myFile = SD.open("post.txt", FILE_WRITE);
  // if the file opened okay, write to it:
  if (myFile) {
    #if debuggEnabled
      Serial.println(F("Writing...")); 
    #endif 
    for (int i=0; i<8; i++)
      {
        myFile.print(i);
        myFile.print(", ");
        myFile.print(SENSORvalue[i]);
        myFile.print(' ');
      }
    myFile.print(sck.RTCtime());
    myFile.println();
    // close the file:
    myFile.close();
    #if debuggEnabled
      Serial.println(F("Closing...")); 
    #endif 
  }
}
#endif

#if USBEnabled

char* SENSOR[10]={
                  "Temperature: ",
                  "Humidity: ",
                  "Light: ",
                  "Battery: ",
                  "Solar Panel: ",
                  "Carbon Oxide: ",
                  "Nitrogen Dioxide: ",
                  "Noise: ",
                  "Wifi Spots: ",
                  "UTC: " 
             };

#if F_CPU == 8000000 
  char* UNITS[10]={
                    " C",
                    " %",
                    " lx",
                    " %",
                    " V",
                    " kOhm",
                    " kOhm",
                    " dB",
                    "",
                    "" 
               };         
#else    
  char* UNITS[10]={
                    " C",
                    " %",
                    " %",
                    " %",
                    " V",
                    " kOhm",
                    " kOhm",
                    " dB",
                    "",
                    "" 
               };  
#endif             
             
void txDebug() {
  uint8_t dec = 0;
  uint16_t pos = (sck.readintEEPROM(EE_ADDR_NUMBER_MEASURES) + 1)/10;
  if (pos>0)pos = (pos - 1)*10;
    for(int i=0; i<10; i++) 
     {
       if (i<4) dec = 1;
       else if (i<7) dec = 3;
       else if (i<8) dec = 2;
       else dec = 0;
       Serial.print(SENSOR[i]); Serial.print(sck.readData(DEFAULT_ADDR_MEASURES, pos + i, dec)); Serial.println(UNITS[i]);
     }
      Serial.println(F("*******************"));
     
}
#endif

#if SDEnabled
  void updateSensorsSD() {
    if (sck.DHT22(IO3))
    {
      SENSORvalue[0] = sck.getTemperatureC(); // C
      SENSORvalue[1] = sck.getHumidity(); // %
    }
    sck.getMICS(4000, 30000);
    SENSORvalue[2] = sck.getLight(); // %
    SENSORvalue[3] = sck.getBattery(); //%
    SENSORvalue[4] = sck.getPanel();  // %
    SENSORvalue[5] = sck.getCO(); //Ohm
    SENSORvalue[6] = sck.getNO2(); //Ohm
    SENSORvalue[7] = sck.getNoise(); //dB    
  }
  
void txDebugSD() {
  float dec = 0;
    for(int i=0; i<8; i++) 
     {
       if (i<4) dec = 10;
       else if (i<7) dec = 1000;
       else if (i<8) dec = 100;
       else dec = 1;
       Serial.print(SENSOR[i]); Serial.print((SENSORvalue[i])/dec); Serial.println(UNITS[i]);
     }
     Serial.print(SENSOR[9]);
     Serial.println(sck.RTCtime());
     Serial.println(F("*******************"));     
}
#endif


