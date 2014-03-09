void setup() { 
 //Initialize serial and wait for port to open:
  #define CONTROL 12  //Control Mode
  #define FACTORY 7  //factory RESET/AP RN131
  #define AWAKE  4 //Despertar WIFI
  pinMode(AWAKE, OUTPUT);
  digitalWrite(AWAKE, HIGH); 
  pinMode(FACTORY, OUTPUT);
  digitalWrite(FACTORY, HIGH); 
  pinMode(CONTROL, INPUT);
  Serial.begin(9600); 
  Serial1.begin(9600); 
  delay(5000);
  sckRecovery();
  delay(2000);
  Serial1.print("$$$");
  delay(250);
  Serial1.println();
  Serial1.println();
} 

void sckRecovery()
{
    Serial.println("Reseting..."); 
    digitalWrite(FACTORY, HIGH);
    delay(1000);
    digitalWrite(FACTORY, LOW);
    delay(1000);
    digitalWrite(FACTORY, HIGH);
    delay(1000);
    digitalWrite(FACTORY, LOW);
    delay(1000);
    digitalWrite(FACTORY, HIGH);
    delay(1000);
    digitalWrite(FACTORY, LOW);
    delay(1000);
    digitalWrite(FACTORY, HIGH);
    delay(1000);
    digitalWrite(FACTORY, LOW);
    delay(1000);
    digitalWrite(FACTORY, HIGH);
    delay(1000);
    digitalWrite(FACTORY, LOW);
    delay(1000);
    
    Serial1.println("factory R");
    Serial1.println("save");
    delay(1000);
    Serial1.println("reboot");
    Serial.println("Successfully reset"); 
    digitalWrite(FACTORY, HIGH);
}

void loop() { 
  
  if (!digitalRead(CONTROL)) 
  {
    Serial.println("Reseting..."); 
    digitalWrite(FACTORY, HIGH);
    delay(1000);
    digitalWrite(FACTORY, LOW);
    delay(1000);
    digitalWrite(FACTORY, HIGH);
    delay(1000);
    digitalWrite(FACTORY, LOW);
    delay(1000);
    digitalWrite(FACTORY, HIGH);
    delay(1000);
    digitalWrite(FACTORY, LOW);
    delay(1000);
    digitalWrite(FACTORY, HIGH);
    delay(1000);
    digitalWrite(FACTORY, LOW);
    delay(1000);
    digitalWrite(FACTORY, HIGH);
    delay(1000);
    digitalWrite(FACTORY, LOW);
    delay(1000);
    
    Serial1.println("factory R");
    Serial1.println("save");
    delay(1000);
    Serial1.println("reboot");
    Serial.println("Successfully reset"); 
    
    digitalWrite(FACTORY, HIGH);
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
