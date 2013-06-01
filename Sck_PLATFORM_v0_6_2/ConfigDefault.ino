
void config(){
        
        sck.writeintEEPROM(EE_ADDR_FREE_SSID, EE_ADDR_SSID);
        sck.writeintEEPROM(EE_ADDR_FREE_PASS, EE_ADDR_PASS);
        sck.writeintEEPROM(EE_ADDR_FREE_AUTH, EE_ADDR_AUTH);
        
        sck.writeintEEPROM(EE_ADDR_MAX_BATTERY, VAL_MAX_BATTERY);
        sck.writeintEEPROM(EE_ADDR_POST_MEASURES, 0x0000);
        sck.writeintEEPROM(EE_ADDR_NUMBER_MEASURES, 0x0000);
        sck.writeintEEPROM(EE_ADDR_FREE_ADDR_MEASURES, FREE_ADDR_MEASURES);
  }

