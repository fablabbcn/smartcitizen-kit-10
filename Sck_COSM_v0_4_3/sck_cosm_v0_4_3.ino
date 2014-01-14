#include <Wire.h>

#include "SmartCitizenAmbient.h"
#include "TimerOne.h"

#define transmitPachube 15 //Tiempo entre transmision y transmision en segundos
#define time_int 500 //Tiempo de atencion a la interrupcion

#define wiflyEnabled    true  
#define SDEnabled       false
#define sensorEnabled   true
#define debuggEnabled   true
#define configDEFAULT   false

SmartCitizen sck;

boolean no_wait = true;
boolean sleep = true;  

#define text_eeprom_write         "eepromWrite\r"
#define text_eeprom_read          "eepromRead\r"
#define text_terminal             "$$$"
#define text_terminal_exit        "exit\r"

int check_e2_write           = 0;
int check_e2_read            = 0;
int check_terminal           = 0;
int check_terminal_exit      = 0;

void timerIsr()
{
  cli();
  if (Serial.available())
  {
    byte inByte = Serial.read();

    if (inByte == text_terminal_exit[check_terminal_exit]) 
    {
      check_terminal_exit++;
      if (check_terminal_exit == strlen(text_terminal_exit)) 
      {
        no_wait = true;
        check_terminal_exit = 0;
      }
    }
    else check_terminal_exit = 0;

    if (inByte == text_terminal[check_terminal]) 
    {
      check_terminal++;
      if (check_terminal == strlen(text_terminal)) 
      {
        digitalWrite(AWAKE, HIGH); 
        delayMicroseconds(100);
        digitalWrite(AWAKE, LOW);
        no_wait = false;
        check_terminal = 0;
      }
    }
    else check_terminal = 0;
    
    Serial1.write(inByte); 
  }
  
  if (Serial1.available()) 
  {
    byte inByte = Serial1.read();
    Serial.write(inByte); 
  }
  sei();
}

#if SDEnabled
#include <SD.h>
File myFile;
#endif

float TEMPvalue = 0;
float HUMvalue = 0;
float COvalue = 0;
float NO2value = 0;
float LIGHTvalue = 0;
float NOISEvalue = 0;
float BATvalue = 0;
float PANELvalue = 0;

void setup() {

  Serial.begin(115200);
  Serial1.begin(9600);
  sck.begin();

  pinMode(IO0, OUTPUT); //VH_MICS5525
  pinMode(IO1, OUTPUT); //VH_MICS2710
  pinMode(IO2, OUTPUT); //MICS2710_ALTAIMPEDANCIA
  pinMode(AWAKE, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT);
  digitalWrite(IO0, LOW); //VH_MICS5525
  digitalWrite(IO1, LOW); //VH_MICS2710
  digitalWrite(IO2, LOW); //RADJ_MICS2710
  digitalWrite(AWAKE, LOW); 

#if SDEnabled
  sck.sleep();
  Serial.print("Initializing SD card...");
  if (!SD.begin(11)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");   
  if (!SD.exists("post.txt")) {
    Serial.println("Creating config.txt...");
    myFile = SD.open("post.txt", FILE_WRITE);
    myFile.close();
  }
#endif   

  /*init WiFly*/


#if wiflyEnabled
  digitalWrite(AWAKE, HIGH); 
  sck.wifi_ready();  // tiempo de espera para la conexion!
  #if configDEFAULT
    config();
  #endif  
  if (transmitPachube <= 10) sleep = false;
  else sleep = true;
  #if debuggEnabled
    Serial.println("wifly connected!");
  #endif
  if(sleep)
  {
    sck.sleep();
    #if debuggEnabled
        Serial.println("Duerme"); 
    #endif
    digitalWrite(AWAKE, LOW); 
  }
#endif  



  Timer1.initialize(time_int); // set a timer of length 1000000 microseconds (or 1 sec - or 1Hz)
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here

}

void loop() {    
#if sensorEnabled  
  for (int i = 0; i< transmitPachube; i++) delay(1000);
    updateSensors();
    if (no_wait)
      {
        Timer1.stop();
        #if wiflyEnabled
            txWiFly();
        #endif
        #if SDEnabled
            txSD();
        #endif
        #if debuggEnabled
            txDebug();
        #endif
        Timer1.initialize(time_int); // set a timer of length 1000000 microseconds (or 1 sec - or 1Hz)
      }
#endif

}













