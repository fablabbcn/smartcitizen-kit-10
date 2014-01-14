#include <Wire.h>
#include "DHT22.h"
#include "RTClib.h"

#define transmitPachube 20 

#define Vref 5000

#define IO0 5  //MICS5525_HEATHER
#define IO1 13 //MICS2710_HEATHER
#define IO2 9  //MICS2710_ALTAIMPEDANCIA
#define IO3 10 //RHT22

#define S0 A4 //MICS_5525
#define S1 A5 //MICS_2710
#define S2 A2 //SENS_5525
#define S3 A3 //SENS_2710
#define S4 A0 //MICRO
#define S5 A1 //LDR

#define AWAKE  4 //Despertar WIFI
#define PANEL A8 //Entrada panel
#define BAT   A7 //Entrada bateria
#define MICS_5525 0
#define MICS_2710 1
#define DHT22_PIN 10

DHT22 myDHT22(DHT22_PIN);

RTC_DS1307 RTC;

#define wiflyEnabled    true
#define SDEnabled       false

#if wiflyEnabled
#include "WiFlyHTML.h"
WiFlyHTML wifly;

//
//char mySSID[] = "IAAC";  
//char myPassword[] = "enteriaac2013";  //
//char wifiEncript[] = WPA2;
//char antenna[] = INT_ANT; // ANTENNA EXTERNA
//#define PACHUBE_FEED "86270" // Bordils
//#define APIKEY "nKn8ldoeUYHFdcLymuWAAm6KfblczFJFCIc8GT2_G30"

/*
char mySSID[] = "COSM";  
char myPassword[] = "1c2c3c4c5c6c7";  //
char wifiEncript[] = WPA2;
char antenna[] = INT_ANT; // ANTENNA EXTERNA
#define PACHUBE_FEED "59775" // FabLab
#define APIKEY "nKn8ldoeUYHFdcLymuWAAm6KfblczFJFCIc8GT2_G30"
*/

/*
char mySSID[] = "mid";  
char myPassword[] = "";
char wifiEncript[] = OPEN;
char antenna[] = EXT_ANT; // ANTENNA EXTERNA
#define PACHUBE_FEED "86270" // Bordils
#define APIKEY "nKn8ldoeUYHFdcLymuWAAm6KfblczFJFCIc8GT2_G30"
*/


char mySSID[] = "hangar_nau3";  
char myPassword[] = "m1cr0fug4s";
char wifiEncript[] = WPA2;
char antenna[] = INT_ANT; // ANTENNA INTEGRADA
//#define PACHUBE_FEED "86270" // Bordils
#define PACHUBE_FEED "42124" // Hangar
#define APIKEY "nKn8ldoeUYHFdcLymuWAAm6KfblczFJFCIc8GT2_G30"


/*
char mySSID[] = "Mi$Red";  
char myPassword[] = "FINALFANTASY";
char wifiEncript[] = WPA2;
char antenna[] = INT_ANT; // ANTENNA INTEGRADA
*/
//#define PACHUBE_FEED "100285"
//#define APIKEY "ssaoVAZPglmNtZFoFfw0IQQUz5OSAKxmZDFhVVNENzZjUT0g"



#endif


#if SDEnabled
#include <SD.h>
File myFile;
#endif

#define eeprom 0x50    // Direcion de la eeprom

float TEMPvalue = 0;
float HUMvalue = 0;
float COvalue = 0;
float NO2value = 0;
float LIGHTvalue = 0;
float NOISEvalue = 0;
float BATvalue = 0;
float PANELvalue = 0;

boolean DHT22ok = false;
boolean sleep   = true;


void setup() {

  Serial.begin(115200);
  Serial1.begin(9600);

  Wire.begin();
  RTC.begin();

  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  /*init Sensors*/

  pinMode(IO0, OUTPUT); //VH_MICS5525
  pinMode(IO1, OUTPUT); //VH_MICS2710
  pinMode(IO2, OUTPUT); //MICS2710_ALTAIMPEDANCIA
  pinMode(IO3, OUTPUT); //DHT22
  pinMode(AWAKE, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT);
  digitalWrite(IO0, LOW); //VH_MICS5525
  digitalWrite(IO1, LOW); //VH_MICS2710
  digitalWrite(IO2, LOW); //RADJ_MICS2710
  digitalWrite(IO3, HIGH); //DHT22

  digitalWrite(AWAKE, HIGH); 
  
#if SDEnabled
  Serial.print("Initializing SD card...");
  if (!SD.begin(11)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");   
  /*if (!SD.exists("config.txt")) {
   Serial.println("Creating config.txt...");
   myFile = SD.open("config.txt", FILE_WRITE);
   myFile.close();
   }*/
#endif   

  /*init WiFly*/

#if wiflyEnabled
  digitalWrite(AWAKE, HIGH); 
  wifly.ready();  // tiempo de espera para la conexion!
  while (!wifly.join()) {  // Cuidado!! Es bloqueante!!
    Serial.println("wifly disconnected!");
    Serial.println("connecting...");
    wifly.begin(mySSID, myPassword, wifiEncript, antenna);   
  }

  if (transmitPachube <= 10) sleep = false;
  else sleep = true;

  Serial.println("wifly connected!");
  Serial.print("IP:"); 
  Serial.println(wifly.ip()); 

  if(sleep)
  {
    wifly.sleep();
    Serial.println("Duerme"); 
    digitalWrite(AWAKE, LOW); 
  }

  updateSensors();
  txWiFly();

#endif


}


void loop() { 

#if wiflyEnabled
  delay(transmitPachube*1000); //CAMBIAR POR TEMPORIZADOR DE MILISEGUNDOS
  updateSensors();
  txWiFly();
#endif

}
















