#if wiflyEnabled

// PACHUBE

void txWiFly() {

  char buff[128];  // message lenght 
  if (sleep){Serial.println("Despierta");  digitalWrite(AWAKE, HIGH); }
  
  Serial.print("IP:"); 
  Serial.println(wifly.ip()); 
  
  sprintf(buff,"0,%d.%d\n1,%d.%d\n2,%d.%d\n3,%d.%d\n4,%d.%d\n5,%d.%d\n6,%d.%d\n7,%d.%d\n",(int)TEMPvalue, decimal(TEMPvalue),(int)HUMvalue, decimal(HUMvalue), (int)COvalue, decimal(COvalue), (int)NO2value, decimal(NO2value),
                                                                                          (int)LIGHTvalue, decimal(LIGHTvalue), (int)NOISEvalue, decimal(NOISEvalue), (int)BATvalue, decimal(BATvalue), (int)PANELvalue, decimal(PANELvalue));

  if (wifly.open("api.pachube.com", 80)) {
    Serial1.print("PUT /v2/feeds/");  // APIV2
    Serial1.print(PACHUBE_FEED);
    Serial1.println(".csv HTTP/1.1");
    Serial1.println("Host: api.pachube.com");
    Serial1.print("X-PachubeApiKey: ");
    Serial1.println(APIKEY);
    Serial1.println("User-Agent: SCK 2.0");
    Serial1.print("Content-Type: text/csv\nContent-Length: ");
    Serial1.println(strlen(buff));
    Serial1.println("Connection: close");
    Serial1.println();
    Serial1.print(buff);
    Serial1.println();
    
    Serial.println("Posted to Cosm!");
  } 
  else {
    Serial.println("Error posting on Cosm..!");
  }

  delay(2000);

  if (wifly.isConnected()) {
    Serial.println("Old connection active. Closing");
    wifly.close();
  }
  
  if (sleep)
  {
    wifly.sleep();
    Serial.println("Duerme"); 
    Serial.println("*******************");
    digitalWrite(AWAKE, LOW); 
  }
}

#endif






