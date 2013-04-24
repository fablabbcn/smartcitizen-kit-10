#if wiflyEnabled
  
void txWiFly() {  
  if (sleep){
    #if debuggEnabled
      Serial.println(F("Despierta"));
    #endif
    digitalWrite(AWAKE, HIGH);
  }
  
  if (sck.connect())
   { 
      #if debuggEnabled
        Serial.println(F("Conectado!!"));  
      #endif     
      if (sck.server_connect()){
        sck.json_updtate();
        #if debuggEnabled
          Serial.println(F("Posted to Server!")); 
        #endif
      } 
      else {
        #if debuggEnabled
          Serial.println(F("Error posting on Server..!"));
        #endif
      }
      if (sck.isConnected()) {
        #if debuggEnabled
          Serial.println(F("Old connection active. Closing"));
        #endif
        sck.close();
      }
    }
   else 
    {
     sck.writeCommand(EE_ADDR_FREE_ADDR_MEASURES, sck.scan());  //Wifi Nets
     sck.writeCommand(EE_ADDR_FREE_ADDR_MEASURES, sck.RTCtime());
     #if debuggEnabled
       Serial.println(F("Fallo conexion!!"));
     #endif
    }
  
  
  if (sleep)
  {
    sck.sleep();
    #if debuggEnabled
      Serial.println(F("Duerme")); 
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
      Serial.println(F("Escribiendo...")); 
    #endif 
    for (int i=0; i<8; i++)
      {
        myFile.print(i);
        myFile.print(", ");
        myFile.print(SENSORvalue[i]);
        myFile.print(' ');
      }
    myFile.println();
    // close the file:
    myFile.close();
    Serial.println(F("Cerrando...")); 
  }
}
#endif

#if USBEnabled
void txDebug() {
    unsigned int pointer = 0; 
    Serial.print(F("Temperatura: ")); Serial.print(atoi(sck.readCommand(EE_ADDR_MEASURES, &pointer))/10.); Serial.println(F(" C"));
    Serial.print(F("Humedad: ")); Serial.print(atoi(sck.readCommand(pointer, &pointer))/10.); Serial.println(F(" %"));
    Serial.print(F("CO: ")); Serial.print(sck.readCommand(pointer, &pointer)); Serial.println(F(" Ohm"));
    Serial.print(F("NO2: ")); Serial.print(sck.readCommand(pointer, &pointer)); Serial.println(F(" Ohm"));
    Serial.print(F("Luz: ")); Serial.print(atoi(sck.readCommand(pointer, &pointer))/10.); Serial.println(F(" %"));
    Serial.print(F("Ruido: ")); Serial.print(atoi(sck.readCommand(pointer, &pointer))/100.); Serial.println(F(" dB"));
    Serial.print(F("Bateria: ")); Serial.print(atoi(sck.readCommand(pointer, &pointer))/10.); Serial.println(F(" %"));
    Serial.print(F("Panel solar: ")); Serial.print(atoi(sck.readCommand(pointer, &pointer))/10.); Serial.println(F(" %"));
    Serial.print(F("Redes: ")); Serial.print(sck.readCommand(pointer, &pointer)); Serial.println();
    Serial.print(F("UTC: ")); Serial.print(sck.readCommand(pointer, &pointer)); Serial.println();
    Serial.println(F("*******************"));
}
#endif




