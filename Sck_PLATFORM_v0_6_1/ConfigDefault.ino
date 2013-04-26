//#define antenna          INT_ANT // ANTENNA INTEGRADA
#define antenna            EXT_ANT // ANTENNA EXTERNA

//Valores por defecto
#define CODE_MAYOR_VERSION          0 
#define CODE_MINOR_VERSION          6
#define CODE_LETTER_VERSION        'b'

void config(){

    if ((sck.readEEPROM(EE_ADDR_CODE_MAYOR_VERSION) != CODE_MAYOR_VERSION)||(sck.readEEPROM(EE_ADDR_CODE_MINOR_VERSION) != CODE_MINOR_VERSION)||(sck.readEEPROM(EE_ADDR_CODE_LETTER_VERSION) != CODE_LETTER_VERSION))
      {
        sck.writeEEPROM(EE_ADDR_CODE_MAYOR_VERSION, CODE_MAYOR_VERSION);
        sck.writeEEPROM(EE_ADDR_CODE_MINOR_VERSION, CODE_MINOR_VERSION); 
        sck.writeEEPROM(EE_ADDR_CODE_LETTER_VERSION, CODE_LETTER_VERSION); 
        
        sck.writeintEEPROM(EE_ADDR_FREE_SSID, EE_ADDR_SSID);
        sck.writeintEEPROM(EE_ADDR_FREE_PASS, EE_ADDR_PASS);
        sck.writeintEEPROM(EE_ADDR_FREE_AUTH, EE_ADDR_AUTH);
        
        sck.writeintEEPROM(EE_ADDR_MAX_BATTERY, VAL_MAX_BATTERY);
        sck.writeintEEPROM(EE_ADDR_POST_MEASURES, 0x0000);
        sck.writeintEEPROM(EE_ADDR_NUMBER_MEASURES, 0x0000);
        sck.writeintEEPROM(EE_ADDR_FREE_ADDR_MEASURES, FREE_ADDR_MEASURES);
      }
  }

