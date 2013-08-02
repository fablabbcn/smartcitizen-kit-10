#include "SmartCitizenAmbient.h"

#define time_int 500 //Tiempo de atencion a la interrupcion en microsegundos

#define USBEnabled      true 
#define wiflyEnabled    true 
#define SDEnabled       false
#define sensorEnabled   true
#define debuggEnabled   true
#define MICSEnabled     true

SmartCitizen sck;

boolean wait        = false;
boolean sleep       = true; 
boolean iphone_mode = false;
byte server_mode    = 0;

boolean eeprom_write_ok      = false;
boolean eeprom_read_ok       = false;
unsigned int address_eeprom  = 0;
uint16_t  nets               = 0;

byte check_ssid_read         = 0;
byte check_ssid_write        = 0;
byte check_pass_read         = 0;
byte check_pass_write        = 0;
byte check_key_write         = 0;
byte check_auth_read         = 0;
byte check_auth_write        = 0;
byte check_antenna_read      = 0;
byte check_antenna_write     = 0;
byte check_clear             = 0;
byte check_time_read         = 0;
byte check_time_write        = 0;
byte check_api_read          = 0;
byte check_api_write         = 0;

byte check_data_read          = 0;
byte check_terminal_mode      = 0;
byte check_sck_mode           = 0;
byte check_terminal_exit      = 0;
byte check_telnet_open        = 0;
byte check_telnet_close       = 0;


uint32_t timetransmit = 0;
uint32_t TimeUpdate   = 10;

#if wiflyEnabled
  void timerIsr()
  {
    sei();
    Timer1.stop();
    if (eeprom_read_ok) 
    {
      if ((nets>0)||(address_eeprom < DEFAULT_ADDR_SSID))
      {
        byte inByte = sck.readEEPROM(address_eeprom);
        if (inByte!=0x00) {
          Serial.write(inByte); 
          address_eeprom = address_eeprom + 1;
        }
        else if (address_eeprom > DEFAULT_ADDR_SSID){ 
          Serial.print(' '); 
          nets=nets-1;
          if (address_eeprom < DEFAULT_ADDR_PASS) address_eeprom = DEFAULT_ADDR_SSID + (buffer_length * (nets - 1));
          else if (address_eeprom < DEFAULT_ADDR_AUTH) address_eeprom = DEFAULT_ADDR_PASS + (buffer_length * (nets - 1));
          else if (address_eeprom < DEFAULT_ADDR_ANTENNA) address_eeprom = DEFAULT_ADDR_AUTH + (buffer_length * (nets - 1));
          else address_eeprom = DEFAULT_ADDR_ANTENNA + (buffer_length * (nets - 1));
        }
        else
           {
             Serial.print("\r\n"); 
             eeprom_read_ok = false;
           }   
      }
      else { 
        Serial.print("\r\n"); 
        eeprom_read_ok = false;
      }
    }
  
    if (Serial.available())
    {
      byte inByte = Serial.read(); 
      if (eeprom_write_ok) 
      {
        if ((inByte!='\r')&&(inByte<0x7F)&&(inByte>0x1F)) {
          if (inByte==' ') sck.writeEEPROM(address_eeprom, '$');
          else sck.writeEEPROM(address_eeprom, inByte); 
          address_eeprom = address_eeprom + 1;
        }
        else 
        {
          sck.writeEEPROM(address_eeprom, 0x00);
          if ((address_eeprom < DEFAULT_ADDR_PASS)&&(address_eeprom >= DEFAULT_ADDR_SSID)) sck.writeintEEPROM(EE_ADDR_NUMBER_NETS, sck.readintEEPROM(EE_ADDR_NUMBER_NETS) + 1);
          eeprom_write_ok = false;
        }
      }  
      if (sck.checkText(inByte, "get wlan ssid\r", &check_ssid_read)){
        eeprom_read_ok = true; 
        nets = sck.readintEEPROM(EE_ADDR_NUMBER_NETS);
        address_eeprom = DEFAULT_ADDR_SSID + (buffer_length * (nets - 1)); 
      }
      if (sck.checkText(inByte, "get wlan phrase\r", &check_pass_read)){
        eeprom_read_ok = true; 
        nets = sck.readintEEPROM(EE_ADDR_NUMBER_NETS);
        address_eeprom = DEFAULT_ADDR_PASS + (buffer_length * (nets - 1)); 
      }
      if (sck.checkText(inByte, "get wlan auth\r", &check_auth_read)){
        eeprom_read_ok = true; 
        nets = sck.readintEEPROM(EE_ADDR_NUMBER_NETS);
        address_eeprom = DEFAULT_ADDR_AUTH + (buffer_length * (nets - 1)); 
      }
      if (sck.checkText(inByte, "get wlan ext_antenna\r", &check_antenna_read)){
        eeprom_read_ok = true; 
        nets = sck.readintEEPROM(EE_ADDR_NUMBER_NETS);
        address_eeprom = DEFAULT_ADDR_ANTENNA + (buffer_length * (nets - 1)); 
      }
      if (sck.checkText(inByte, "get time update\r", &check_time_read)){
        eeprom_read_ok = true; 
        address_eeprom = EE_ADDR_TIME_UPDATE; 
      }
      if (sck.checkText(inByte, "get apikey\r", &check_api_read)){
        eeprom_read_ok = true; 
        address_eeprom = EE_ADDR_APIKEY; 
      }
      if (sck.checkText(inByte, "set wlan ssid ", &check_ssid_write)){
        if (sck.readintEEPROM(EE_ADDR_NUMBER_NETS)<10)
          {
            address_eeprom = DEFAULT_ADDR_SSID + (sck.readintEEPROM(EE_ADDR_NUMBER_NETS)) * buffer_length;
            eeprom_write_ok = true;
            server_mode = 1; 
            if (TimeUpdate < 60) sleep = false;
            else sleep = true; 
          }
      }
      if (sck.checkText(inByte, "set wlan phrase ", &check_pass_write)){
        if (sck.readintEEPROM(EE_ADDR_NUMBER_NETS)<10)
         {
            address_eeprom = DEFAULT_ADDR_PASS + (sck.readintEEPROM(EE_ADDR_NUMBER_NETS) - 1) * buffer_length;
            eeprom_write_ok = true;
         } 
      }  
      if (sck.checkText(inByte, "set wlan key ", &check_key_write)){
        if (sck.readintEEPROM(EE_ADDR_NUMBER_NETS)<10)
          {
            address_eeprom = DEFAULT_ADDR_PASS + (sck.readintEEPROM(EE_ADDR_NUMBER_NETS) - 1) * buffer_length;
            eeprom_write_ok = true; 
          }
      }  
      if (sck.checkText(inByte, "set wlan ext_antenna ", &check_antenna_write)){
        if (sck.readintEEPROM(EE_ADDR_NUMBER_NETS)<10)
          {
            address_eeprom = DEFAULT_ADDR_ANTENNA + (sck.readintEEPROM(EE_ADDR_NUMBER_NETS) - 1) * buffer_length;
            eeprom_write_ok = true; 
          }
      }  
      if (sck.checkText(inByte, "set wlan auth ", &check_auth_write)){
        if (sck.readintEEPROM(EE_ADDR_NUMBER_NETS)<10)
          {
            address_eeprom = DEFAULT_ADDR_AUTH + (sck.readintEEPROM(EE_ADDR_NUMBER_NETS) - 1) * buffer_length;
            eeprom_write_ok = true; 
          }
      }  
      if (sck.checkText(inByte, "clear nets\r", &check_clear)){
        sck.writeintEEPROM(EE_ADDR_NUMBER_NETS, 0x0000);
      } 
      if (sck.checkText(inByte, "set time update ", &check_time_write)){
        eeprom_write_ok = true; 
        address_eeprom = EE_ADDR_TIME_UPDATE;
      } 
      if (sck.checkText(inByte, "set apikey ", &check_api_write)){
        eeprom_write_ok = true; 
        address_eeprom = EE_ADDR_APIKEY;
      } 
      
      if (sck.checkText(inByte, "exit\r", &check_terminal_exit)) wait = false;
      if ((sck.checkText(inByte, "$$$", &check_terminal_mode))||(sck.checkText(inByte, "###", &check_sck_mode)))
      {
        digitalWrite(AWAKE, HIGH); 
        delayMicroseconds(100);
        digitalWrite(AWAKE, LOW);
        wait = true;
      }
      Serial1.write(inByte); 
    }
  
    else if (Serial1.available()) 
    {
      byte inByte = Serial1.read();
      if (sck.checkText(inByte, "*OPEN*", &check_telnet_open))  { 
        wait = true; 
        server_mode = 0;
      }
      if (sck.checkText(inByte, "*CLOS*", &check_telnet_close)) { 
        wait = false; 
        server_mode = 1;
      }
      if (sck.checkText(inByte, "#data\r", &check_data_read))  
      { 
        uint16_t temp = (sck.readintEEPROM(EE_ADDR_NUMBER_MEASURES) + 1)/10;
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
    Timer1.initialize(time_int); // set a timer of length 1000000 microseconds (or 1 sec - or 1Hz)
  }
#endif

#if SDEnabled
  #include <SD.h>
  File myFile;
  long SENSORvalue[8];
#endif

void setup() {

  sck.begin();
   
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
  server_mode = 1;  //Modo normal
  sck.config(); 
  TimeUpdate = atol(sck.readData(EE_ADDR_TIME_UPDATE, 0, 0)); //Tiempo entre transmision y transmision en segundos, minimo 60 segundos
  if (TimeUpdate < 60) sleep = false;
  else sleep = true; 
  if (!sck.connect())
  {
    sleep = false;  
    server_mode = 0; //Modo AP
    sck.APmode(sck.id());
    #if debuggEnabled
        Serial.println(F("AP initialized!"));
    #endif 
  }
  else
  {
    #if debuggEnabled
        Serial.println(F("SCK Connected!!"));
    #endif
    byte retry = 0;
    if (sck.checkRTC())
     {
      while (!sck.RTCadjust(sck.WIFItime())&&(retry<5))
       {
         retry = retry + 1;
       }
     }
  }  
    
  if(sleep)
    {
      sck.sleep();
      #if debuggEnabled
          Serial.println(F("SCK Sleeping...")); 
      #endif
      digitalWrite(AWAKE, LOW); 
    }
  Timer1.initialize(time_int); // set a timer of length 1000000 microseconds (or 1 sec - or 1Hz)
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here  
  #if !MICSEnabled
    server_mode = 3; 
  #endif  
#else 
  #if !MICSEnabled
    server_mode = 4; 
  #else 
    server_mode = 2; 
  #endif  
#endif  
}

void loop() {     
#if sensorEnabled  
    #if wiflyEnabled
      if (iphone_mode) // Telnet  (#data + *OPEN* detectado )
      {
        Timer1.stop();
        sleep = false;
        digitalWrite(AWAKE, HIGH);
        sck.json_update(0);
        iphone_mode = false;
        Timer1.initialize(time_int); // set a timer of length 1000000 microseconds (or 1 sec - or 1Hz)
      }
    #endif
    if ((millis()-timetransmit) >= (unsigned long)TimeUpdate*1000)
    {  
      timetransmit = millis();
      #if wiflyEnabled
        sck.updateSensors(server_mode); 
        if (!wait) // command mode false
        {
          Timer1.stop();
          #if wiflyEnabled
            if (server_mode) txWiFly();
          #endif
          #if USBEnabled
            txDebug();
          #endif
          Timer1.initialize(time_int); // set a timer of length 1000000 microseconds (or 1 sec - or 1Hz)
        }
      #else
        #if SDEnabled
          updateSensorsSD();
          txSD();
          #if USBEnabled
            txDebugSD();
          #endif
        #endif
        #if (USBEnabled && !SDEnabled)
          sck.updateSensors(server_mode); 
          txDebug();
        #endif
      #endif
    }
#endif
}















