#include <Wire.h>

#include "SmartCitizenAmbient.h"
#include "TimerOne.h"

#define transmit 60 //Tiempo entre transmision y transmision en segundos, minimo 60 segundos
#define time_int 500 //Tiempo de atencion a la interrupcion en microsegundos
unsigned long timetransmit;

#define USBEnabled      true 
#define wiflyEnabled    true  
#define SDEnabled       false
#define sensorEnabled   true
#define debuggEnabled   true
#define GasEnabled      true  // false if the board hasn't gas sensors!


SmartCitizen sck;

boolean wait        = false;
boolean sleep       = true; 
boolean ap_mode     = false;
boolean iphone_mode = false;
byte server_mode    = 0;

boolean eeprom_write_ssid    = false;
boolean eeprom_write_pass    = false;
boolean eeprom_write_ok      = false;
boolean eeprom_read_ssid     = false;
boolean eeprom_read_pass     = false;
boolean eeprom_read_ok       = false;
unsigned int address_eeprom  = 0;
uint16_t  nets                = 0;

byte check_ssid_read         = 0;
byte check_ssid_write        = 0;
byte check_pass_write        = 0;
byte check_pass_read         = 0;
byte check_auth_write        = 0;
byte check_auth_read         = 0;

byte check_data_read          = 0;
byte check_terminal_mode      = 0;
byte check_terminal_exit      = 0;
byte check_telnet_open        = 0;
byte check_telnet_close       = 0;

void timerIsr()
{
  sei();
  if (eeprom_read_ok) 
  {
    if (nets>0) 
    {
      byte inByte = sck.readEEPROM(address_eeprom);
      if (inByte!=0x00) {
        Serial.write(inByte); 
        address_eeprom = address_eeprom + 1;
      }
      else { 
        Serial.print(' '); 
        nets=nets-1; 
        address_eeprom = address_eeprom + 1;
      }
    }
    else { 
      Serial.println(); 
      eeprom_read_ok = false;
    }
  }

  if (Serial.available())
  {
    byte inByte = Serial.read(); 
    if (eeprom_write_ok) 
    {
      if (inByte!='\r') {
        sck.writeEEPROM(address_eeprom, inByte); 
        address_eeprom = address_eeprom + 1;
      }
      else 
      {
        sck.writeEEPROM(address_eeprom, 0x00);
        if (address_eeprom < EE_ADDR_PASS) sck.writeintEEPROM(EE_ADDR_NUMBER_NETS, sck.readintEEPROM(EE_ADDR_NUMBER_NETS) + 1);
        eeprom_write_ok = false;
      }
    }  
    if (sck.check_text(inByte, "#rs\r", &check_ssid_read)){
      eeprom_read_ok = true; 
      address_eeprom = EE_ADDR_SSID; 
      nets = sck.readintEEPROM(EE_ADDR_NUMBER_NETS);
    }
    if (sck.check_text(inByte, "#rp\r", &check_pass_read)){
      eeprom_read_ok = true; 
      address_eeprom = EE_ADDR_PASS; 
      nets = sck.readintEEPROM(EE_ADDR_NUMBER_NETS);
    }
    if (sck.check_text(inByte, "#ra\r", &check_auth_read)){
      eeprom_read_ok = true; 
      address_eeprom = EE_ADDR_AUTH; 
      nets = sck.readintEEPROM(EE_ADDR_NUMBER_NETS);
    }
    if (sck.check_text(inByte, "set wlan ssid ", &check_ssid_write)){
      eeprom_write_ok = true; 
      address_eeprom = sck.readintEEPROM(EE_ADDR_FREE_SSID);
    }
    if (sck.check_text(inByte, "set wlan phrase ", &check_pass_write)){
      eeprom_write_ok = true; 
      address_eeprom = sck.readintEEPROM(EE_ADDR_FREE_PASS);
    }   
    if (sck.check_text(inByte, "set wlan auth ", &check_auth_write)){
      eeprom_write_ok = true; 
      address_eeprom = sck.readintEEPROM(EE_ADDR_FREE_AUTH);
    }  
    if (sck.check_text(inByte, "#clear ", &check_auth_write)){
      sck.writeintEEPROM(EE_ADDR_NUMBER_NETS, 0x0000);
    } 
    if (sck.check_text(inByte, "exit\r", &check_terminal_exit)) wait = false;
    if (sck.check_text(inByte, "$$$", &check_terminal_mode))
    {
      digitalWrite(AWAKE, HIGH); 
      delayMicroseconds(100);
      digitalWrite(AWAKE, LOW);
      wait = true;
    }
    Serial1.write(inByte); 
  }

  if (Serial1.available()) 
  {
    byte inByte = Serial1.read();
    if (sck.check_text(inByte, "*OPEN*", &check_telnet_open))  { 
      wait = true; 
      server_mode = 0;
    }
    if (sck.check_text(inByte, "*CLOS*", &check_telnet_close)) { 
      wait = false; 
      server_mode = 1;
    }
    if (sck.check_text(inByte, "#data\r", &check_data_read))  
    { 
      uint16_t temp = sck.readintEEPROM(EE_ADDR_POST_MEASURES);
      if (temp > 0) { 
        iphone_mode = true; 
        wait = true; 
        server_mode = 0;
      }
      Serial1.print("updates: ");
      Serial1.println(temp);
    }
    Serial.write(inByte); 
  }
}

#if SDEnabled
#include <SD.h>
File myFile;

long SENSORvalue[8];

#endif

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
#if debuggEnabled
  Serial.print(F("Initializing SD card..."));
#endif 
  if (!SD.begin(11)) {
#if debuggEnabled
    Serial.println(F("initialization failed!"));
#endif 
    return;
  }
#if debuggEnabled
  Serial.println(F("initialization done."));   
#endif 
  if (!SD.exists("post.txt")) {
#if debuggEnabled
    Serial.println(F("Creating post.txt..."));
#endif 
    myFile = SD.open("post.txt", FILE_WRITE);
    myFile.close();
  }
#endif   

  /*init WiFly*/
#if wiflyEnabled
  digitalWrite(AWAKE, HIGH); 
  server_mode = 1;
  if (transmit < 60) sleep = false;
  else sleep = true;

  config();  

  if (!sck.connect())
  {
    sleep = false;  
    ap_mode = true;
    sck.APmode(sck.id());
#if debuggEnabled
    Serial.println(F("AP initialized!"));
#endif 
  }
  else
  {
#if debuggEnabled
    Serial.println(F("wifly connected!"));
#endif
    byte retry = 0;
    while (!sck.RTCadjust(sck.WIFItime())&&(retry<5)) {
      retry = retry + 1;
    }
  }  

  if(sleep)
  {
    sck.sleep();
#if debuggEnabled
    Serial.println(F("Duerme")); 
#endif
    digitalWrite(AWAKE, LOW); 
  }
#else  
  server_mode = 2; 
#endif  

  Timer1.initialize(time_int); // set a timer of length 1000000 microseconds (or 1 sec - or 1Hz)
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here   
  timetransmit = millis();
}

void loop() {    
#if sensorEnabled  
  if (!ap_mode)
  {
    if (iphone_mode) // Telnet  (#data + *OPEN* detectado )
    {
      Timer1.stop();
      sleep = false;
      digitalWrite(AWAKE, HIGH);
      sck.json_updtate();
      iphone_mode = false;
      Timer1.initialize(time_int); // set a timer of length 1000000 microseconds (or 1 sec - or 1Hz)
    }
    if ((millis()-timetransmit) >= (unsigned long)transmit*1000)
    { 
      timetransmit = millis();
      if(!GasEnabled)
        server_mode = 3;
        
      sck.updateSensors(server_mode);
      if (!wait) // command mode false
      {
        Timer1.stop();
#if wiflyEnabled
        if (server_mode) txWiFly();
#endif
#if SDEnabled
        txSD();
#endif
#if USBEnabled
        txDebug();
#endif
        Timer1.initialize(time_int); // set a timer of length 1000000 microseconds (or 1 sec - or 1Hz)
      }
    }
  }
#endif
}















