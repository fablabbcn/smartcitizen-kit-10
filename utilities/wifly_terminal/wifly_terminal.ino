/*

  Smart Citizen Kit
  Firmware for serial commands communication with the WiFly

  
  - UPDATING WIFLY FIRMWARE !

    - Load this firmware and open the Arduino SERIAL MONITOR.

    - Write the following commands:

      - ftp upload    (this download and install the last WiFly firmware)
      - factory RESET (restart the WiFly module to original factory presets)
      - reboot        (reboots the WiFly module)

*/


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
