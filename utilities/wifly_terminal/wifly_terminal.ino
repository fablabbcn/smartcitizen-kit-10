void setup() { 
  sckBegin();
  sckRepair();
  //sckConnect();
  sckEnterCommandMode();
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
