void txSD() {
  myFile = SD.open("post.csv", FILE_WRITE);
  // if the file opened okay, write to it:
  if (myFile) {
  #if debuggEnabled
      Serial.println(F("Writing...")); 
  #endif 
    float dec = 0;
    for (int i=0; i<8; i++)
    {
      if (i<4) dec = 10;
      else if (i<7) dec = 1000;
      else if (i<8) dec = 100;
      else dec = 1;

      myFile.print(i);
      myFile.print(" ");
      myFile.print(SENSORvalue[i]/dec);
      myFile.print(" ");
    }
    myFile.print(sckRTCtime());
    myFile.println();
    // close the file:
    myFile.close();
#if debuggEnabled
    Serial.println(F("Closing...")); 
#endif 
  }
}

char* SENSOR[10]={
  "Temperature: ",
  "Humidity: ",
  "Light: ",
  "Battery: ",
  "Solar Panel: ",
  "Carbon Monxide: ",
  "Nitrogen Dioxide: ",
  "Noise: ",
  "Wifi Spots: ",
  "UTC: " 
};

char* UNITS[10]={
#if F_CPU == 8000000 
#if DataRaw
  " C RAW",
  " % RAW",
#else
  " C",
  " %",
#endif
#else
  " C",
  " %",
#endif
#if F_CPU == 8000000 
  " lx",
#else
  " %",
#endif
  " %",
  " V",
  " kOhm",
  " kOhm",
#if DataRaw
  " mV",
#else
  " dB",
#endif
  "",
  "" 
};            

void updateSensorsSD() {
  #if F_CPU == 8000000 
    sckGetVcc();
    sckGetSHT21();
    SENSORvalue[0] = lastTemperature; // C
    SENSORvalue[1] = lastHumidity; // %
  #else
    if (sckDHT22(IO3))
    {
      SENSORvalue[0] = lastTemperature; // C
      SENSORvalue[1] = lastHumidity; // %
    }
  #endif
    sckGetMICS();
    SENSORvalue[2] = sckGetLight(); // %
    SENSORvalue[3] = sckGetBattery(); //%
    SENSORvalue[4] = sckGetPanel();  // %
    SENSORvalue[5] = sckGetCO(); //Ohm
    SENSORvalue[6] = sckGetNO2(); //Ohm
    SENSORvalue[7] = sckGetNoise(); //dB    
}

void txDebugSD() {
  float dec = 0;
  for(int i=0; i<8; i++) 
  {
    if (i<4) dec = 10;
    else if (i<7) dec = 1000;
    else if (i<8) dec = 100;
    else dec = 1;
    Serial.print(SENSOR[i]); 
    Serial.print((SENSORvalue[i])/dec); 
    Serial.println(UNITS[i]);
  }
  Serial.print(SENSOR[9]);
  Serial.println(sckRTCtime());
  Serial.println(F("*******************"));     
}



