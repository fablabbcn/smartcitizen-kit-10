void setup() { 
  sckBegin();
  delay(5000);
  sckConnect();
  sckExitCommandMode();
  sckEnterCommandMode();
} 

void loop() { 
  if (!digitalRead(12))
  {
      if (sckConnect()) 
      {
        Serial.println("connected!!");
        sckExitCommandMode();
        sckEnterCommandMode();
        Serial.println("Updating...");
        if (sckUpdate()) Serial.println("Update OK");
        else Serial.println("Update FAIL");
        sckReset();
      }
      else Serial.println("Connection Fail :(");
  }
      
  if (Serial.available()) 
  {
    int inByte = Serial.read();
    Serial1.write(inByte); 
  }
  if (Serial1.available()) {
    int inByte = Serial1.read();
    Serial.write(inByte); 
  }
} 
