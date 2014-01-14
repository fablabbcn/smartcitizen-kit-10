//////////////////////////////////////////////////////////////////
//download and upload this firmware to the sck
//https://github.com/fablabbcn/Smart-Citizen-Kit/tree/master/Wifly_terminal
//
//OPEN ARDUINO SERIAL MONITOR and write the next commands:
//
//- ftp upload
//(this download and install the last wifi firmware)
//- factory RESET
//(restart to original factory preset)
//- reboot
//(reboot the wifi)
//////////////////////////////////////////////////////////////////

void setup() { 
 //Initialize serial and wait for port to open:
  #define AWAKE  4 //Wake up WIFI
  pinMode(AWAKE, OUTPUT);
  digitalWrite(AWAKE, HIGH); 
  Serial.begin(9600); 
  Serial1.begin(9600); 
  delay(5000);
  Serial1.print("$$$");
  delay(250);
  Serial1.println();
  Serial1.println();
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
