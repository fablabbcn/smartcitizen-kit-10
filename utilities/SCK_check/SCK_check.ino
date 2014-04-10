void setup() { 
  sckBegin();
  delay(5000);
  Serial.println("Trying to connect.");
  if (!sckRepair()) sckRecovery();
  else Serial.println("Device detected.");
  Serial.print("Firmware version: "); 
  char *Version = getWiFlyVersion(1000);
  Serial.println(Version);
  checkWiFlyVersion(Version);
} 

void loop() {   
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
