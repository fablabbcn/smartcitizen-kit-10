#if wiflyEnabled
void txWiFly() {
  char buff[128];  // message lenght
  
  if (sleep){
    #if debuggEnabled
      Serial.println("Despierta");
    #endif
    digitalWrite(AWAKE, HIGH);
  }
  
  //if (true) //Si va el autoconectar
  if (sck.wifi_ready()) //Si no va el autoconectar
   { 
      #if debuggEnabled
        Serial.println("Conectado!!");  
      #endif 
      
      sprintf(buff,"0,%d.%d\n1,%d.%d\n2,%d.%d\n3,%d.%d\n4,%d.%d\n5,%d.%d\n6,%d.%d\n7,%d.%d\n",(int)TEMPvalue, sck.decimal(TEMPvalue),(int)HUMvalue, sck.decimal(HUMvalue), (int)COvalue, sck.decimal(COvalue), (int)NO2value, sck.decimal(NO2value),
                                                                                              (int)LIGHTvalue, sck.decimal(LIGHTvalue), (int)NOISEvalue, sck.decimal(NOISEvalue), (int)BATvalue, sck.decimal(BATvalue), (int)PANELvalue, sck.decimal(PANELvalue));
      unsigned int pointer = 0;   
      if (sck.open(sck.readCommand(EE_ADDR_UPDATE_WEB, &pointer), 80)) {
        for (byte i=1; i<12; i++)
        {
          if (i==11)
          {
            Serial1.println(strlen(buff));
            Serial1.print(sck.readCommand(pointer, &pointer));  // APIV2
            Serial1.println();
            Serial1.print(buff);
            Serial1.println();
          } 
          else 
          {
            Serial1.print(sck.readCommand(pointer, &pointer));  // APIV2
          }
        }
        #if debuggEnabled
          pointer = 0; 
          Serial.print("open ");
          Serial.print(sck.readCommand(EE_ADDR_UPDATE_WEB, &pointer));
          Serial.println(" 80");
          for (byte i=1; i<12; i++)
          {
            if (i==11)
            {
                Serial.println(strlen(buff));
                Serial.print(sck.readCommand(pointer, &pointer));  // APIV2
                Serial.println();
                Serial.print(buff);
                Serial.println();
            } 
            else 
            {
                Serial.print(sck.readCommand(pointer, &pointer));  // APIV2
            }
          }
            Serial.println("Posted to Cosm!");
        #endif
      } 
      else {
        #if debuggEnabled
          Serial.println("Error posting on Cosm..!");
        #endif
      }
      if (sck.isConnected()) {
        #if debuggEnabled
          Serial.println("Old connection active. Closing");
        #endif
        sck.close();
      }
    }
   #if debuggEnabled
     else Serial.println("Fallo conexion!!");
   #endif

  
  
  if (sleep)
  {
    sck.sleep();
    #if debuggEnabled
      Serial.println("Duerme"); 
      Serial.println("*******************");
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
    myFile.print("0,");
    myFile.print(TEMPvalue);
    myFile.print(" 1,");
    myFile.print(HUMvalue);
    myFile.print(" 2,");
    myFile.print(COvalue);
    myFile.print(" 3,");
    myFile.print(NO2value);
    myFile.print(" 4,");
    myFile.print(LIGHTvalue);
    myFile.print(" 5,");
    myFile.print(NOISEvalue);
    myFile.print(" 6,");
    myFile.print(BATvalue);
    myFile.print(" 7,");
    myFile.print(PANELvalue);
    myFile.println();
    // close the file:
    myFile.close();
  }
}
#endif

#if debuggEnabled
void txDebug() {
    Serial.print("Temperura: "); Serial.print(TEMPvalue); Serial.println("grados");
    Serial.print("Humedad: "); Serial.print(HUMvalue); Serial.println("%");
    Serial.print("CO: "); Serial.print(COvalue); Serial.println("KOhm");
    Serial.print("NO2: "); Serial.print(NO2value); Serial.println("KOhm");
    Serial.print("Luz: "); Serial.print(LIGHTvalue); Serial.println("%");
    Serial.print("Ruido: "); Serial.print(NOISEvalue); Serial.println("dB");
    Serial.print("Bateria: "); Serial.print(BATvalue); Serial.println("%");
    Serial.print("Panel solar: "); Serial.print(PANELvalue); Serial.println("%");
    Serial.println("*******************");
}
#endif




