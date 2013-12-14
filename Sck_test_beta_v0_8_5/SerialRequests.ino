#if wiflyEnabled

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
byte check_number_read       = 0;
byte check_number_write      = 0;

byte check_data_read          = 0;
byte check_terminal_mode      = 0;
byte check_sck_mode           = 0;
byte check_terminal_exit      = 0;
byte check_telnet_open        = 0;
byte check_telnet_close       = 0;


/*TIMER*/
void timer1SetPeriod()		// AR modified for atomic access
{ 
  char oldSREG = SREG;				
  cli();							// Disable interrupts for 16 bit register access
   #if F_CPU == 8000000 
    ICR1 = 2000; 
   #else
    ICR1 = 4000;
   #endif
  SREG = oldSREG;
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
  TCCR1B |= _BV(CS10);
}

void timer1Initialize()
    {
      TCCR1A = 0;                 // clear control register A 
      TCCR1B = _BV(WGM13);        // set mode 8: phase and frequency correct pwm, stop the timer
      timer1SetPeriod();
    }

void timer1AttachInterrupt()
{
  TIMSK1 = _BV(TOIE1);                                     // sets the timer overflow interrupt enable bit
  TCCR1B |= _BV(CS10);
}

void timer1Stop()
{
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));          // clears all clock selects bits
}

ISR(TIMER1_OVF_vect)
{
  sei();
  timer1Stop();
  #if F_CPU == 8000000 
//    if (!digitalRead(CONTROL))
//      {
//        for (int i=0; i<7; i++) { digitalWrite(FACTORY, HIGH); delay(1000); digitalWrite(FACTORY, LOW); delay(1000); }
//        Serial.println(F("RESET RN131"));
//      }
      digitalWrite(FACTORY, !digitalRead(CONTROL));
  #endif
  if (eeprom_read_ok) 
  {
    if ((nets>0)||(address_eeprom < DEFAULT_ADDR_SSID))
    {
      byte inByte = sckReadEEPROM(address_eeprom);
      if (inByte!=0x00) {
        Serial.write(inByte); 
        address_eeprom = address_eeprom + 1;
      }
      else if (address_eeprom > DEFAULT_ADDR_SSID){ 
        Serial.print(F(" ")); 
        nets=nets-1;
        if (address_eeprom < DEFAULT_ADDR_PASS) address_eeprom = DEFAULT_ADDR_SSID + (buffer_length * (nets - 1));
        else if (address_eeprom < DEFAULT_ADDR_AUTH) address_eeprom = DEFAULT_ADDR_PASS + (buffer_length * (nets - 1));
        else if (address_eeprom < DEFAULT_ADDR_ANTENNA) address_eeprom = DEFAULT_ADDR_AUTH + (buffer_length * (nets - 1));
        else address_eeprom = DEFAULT_ADDR_ANTENNA + (buffer_length * (nets - 1));
      }
      else
         {
           Serial.print(F("\r\n")); 
           eeprom_read_ok = false;
         }   
    }
    else { 
      Serial.print(F("\r\n")); 
      eeprom_read_ok = false;
    }
  }

  if (Serial.available())
  {
    byte inByte = Serial.read(); 
    if (eeprom_write_ok) 
    {
      if ((inByte!='\r')&&(inByte<0x7F)&&(inByte>0x1F)) {
        if (inByte==' ') sckWriteEEPROM(address_eeprom, '$');
        else sckWriteEEPROM(address_eeprom, inByte); 
        address_eeprom = address_eeprom + 1;
      }
      else 
      {
        sckWriteEEPROM(address_eeprom, 0x00);
        if ((address_eeprom < DEFAULT_ADDR_PASS)&&(address_eeprom >= DEFAULT_ADDR_SSID)) sckWriteintEEPROM(EE_ADDR_NUMBER_NETS, sckReadintEEPROM(EE_ADDR_NUMBER_NETS) + 1);
        eeprom_write_ok = false;
      }
    }  
    if (sckCheckText(inByte, "get wlan ssid\r", &check_ssid_read)){
      eeprom_read_ok = true; 
      nets = sckReadintEEPROM(EE_ADDR_NUMBER_NETS);
      address_eeprom = DEFAULT_ADDR_SSID + (buffer_length * (nets - 1)); 
    }
    if (sckCheckText(inByte, "get wlan phrase\r", &check_pass_read)){
      eeprom_read_ok = true; 
      nets = sckReadintEEPROM(EE_ADDR_NUMBER_NETS);
      address_eeprom = DEFAULT_ADDR_PASS + (buffer_length * (nets - 1)); 
    }
    if (sckCheckText(inByte, "get wlan auth\r", &check_auth_read)){
      eeprom_read_ok = true; 
      nets = sckReadintEEPROM(EE_ADDR_NUMBER_NETS);
      address_eeprom = DEFAULT_ADDR_AUTH + (buffer_length * (nets - 1)); 
    }
    if (sckCheckText(inByte, "get wlan ext_antenna\r", &check_antenna_read)){
      eeprom_read_ok = true; 
      nets = sckReadintEEPROM(EE_ADDR_NUMBER_NETS);
      address_eeprom = DEFAULT_ADDR_ANTENNA + (buffer_length * (nets - 1)); 
    }
    if (sckCheckText(inByte, "get time update\r", &check_time_read)){
      eeprom_read_ok = true; 
      address_eeprom = EE_ADDR_TIME_UPDATE; 
    }
    if (sckCheckText(inByte, "get number updates\r", &check_number_read)){
      eeprom_read_ok = true; 
      address_eeprom = EE_ADDR_NUMBER_UPDATES; 
    }
    if (sckCheckText(inByte, "get apikey\r", &check_api_read)){
      eeprom_read_ok = true; 
      address_eeprom = EE_ADDR_APIKEY; 
    }
    if (sckCheckText(inByte, "set wlan ssid ", &check_ssid_write)){
      if (sckReadintEEPROM(EE_ADDR_NUMBER_NETS)<10)
        {
          address_eeprom = DEFAULT_ADDR_SSID + (sckReadintEEPROM(EE_ADDR_NUMBER_NETS)) * buffer_length;
          eeprom_write_ok = true;
          server_mode = 1; 
          if (TimeUpdate < 60) sleep = false;
          else sleep = true; 
        }
    }
    if (sckCheckText(inByte, "set wlan phrase ", &check_pass_write)){
      if (sckReadintEEPROM(EE_ADDR_NUMBER_NETS)<10)
       {
          address_eeprom = DEFAULT_ADDR_PASS + (sckReadintEEPROM(EE_ADDR_NUMBER_NETS) - 1) * buffer_length;
          eeprom_write_ok = true;
       } 
    }  
    if (sckCheckText(inByte, "set wlan key ", &check_key_write)){
      if (sckReadintEEPROM(EE_ADDR_NUMBER_NETS)<10)
        {
          address_eeprom = DEFAULT_ADDR_PASS + (sckReadintEEPROM(EE_ADDR_NUMBER_NETS) - 1) * buffer_length;
          eeprom_write_ok = true; 
        }
    }  
    if (sckCheckText(inByte, "set wlan ext_antenna ", &check_antenna_write)){
      if (sckReadintEEPROM(EE_ADDR_NUMBER_NETS)<10)
        {
          address_eeprom = DEFAULT_ADDR_ANTENNA + (sckReadintEEPROM(EE_ADDR_NUMBER_NETS) - 1) * buffer_length;
          eeprom_write_ok = true; 
        }
    }  
    if (sckCheckText(inByte, "set wlan auth ", &check_auth_write)){
      if (sckReadintEEPROM(EE_ADDR_NUMBER_NETS)<10)
        {
          address_eeprom = DEFAULT_ADDR_AUTH + (sckReadintEEPROM(EE_ADDR_NUMBER_NETS) - 1) * buffer_length;
          eeprom_write_ok = true; 
        }
    }  
    if (sckCheckText(inByte, "clear nets\r", &check_clear)){
      sckWriteintEEPROM(EE_ADDR_NUMBER_NETS, 0x0000);
    } 
    if (sckCheckText(inByte, "set time update ", &check_time_write)){
      eeprom_write_ok = true; 
      address_eeprom = EE_ADDR_TIME_UPDATE;
    } 
    if (sckCheckText(inByte, "set number updates ", &check_number_write)){
      eeprom_write_ok = true; 
      address_eeprom = EE_ADDR_NUMBER_UPDATES;
    } 
    if (sckCheckText(inByte, "set apikey ", &check_api_write)){
      eeprom_write_ok = true; 
      address_eeprom = EE_ADDR_APIKEY;
    } 
    
    if (sckCheckText(inByte, "exit\r", &check_terminal_exit)) wait = false;
    if ((sckCheckText(inByte, "$$$", &check_terminal_mode))||(sckCheckText(inByte, "###", &check_sck_mode)))
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
    if (sckCheckText(inByte, "*OPEN*", &check_telnet_open))  { 
      wait = true; 
      server_mode = 0;
    }
    if (sckCheckText(inByte, "*CLOS*", &check_telnet_close)) { 
      wait = false; 
      server_mode = 1;
    }
    if (sckCheckText(inByte, "#data\r", &check_data_read))  
    { 
      uint16_t temp = (sckReadintEEPROM(EE_ADDR_NUMBER_MEASURES) + 1)/10;
      if (temp > 0) { 
        iphone_mode = true; 
        wait = true; 
        server_mode = 0;
      }
      Serial1.print(F("updates: "));
      Serial1.println(temp);
    }
    Serial.write(inByte); 
  }
  timer1Initialize(); // set a timer of length 1000000 microseconds (or 1 sec - or 1Hz)
}
#endif
