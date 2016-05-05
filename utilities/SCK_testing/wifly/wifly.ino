

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(4, OUTPUT);  //despierta al wifly
  digitalWrite(4, LOW);

  pinMode(7, OUTPUT);  //factory RESET/AP RN131
  digitalWrite(7, LOW);

  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT);
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
