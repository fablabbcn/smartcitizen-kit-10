#define redes 2
char* mySSID[redes] =      { "COSM"         , "" };
char* myPassword[redes] =  { "1c2c3c4c5c6c7", "" };
char* wifiEncript[redes] = { WPA2           , OPEN };

//#define antenna          INT_ANT // ANTENNA INTEGRADA
#define antenna            EXT_ANT // ANTENNA EXTERNA

//Valores por defecto
#define CODE_MAYOR_VERSION          0 
#define CODE_MINOR_VERSION          6
#define CODE_LETTER_VERSION        'a'

void config(){

    if ((sck.readEEPROM(EE_ADDR_CODE_MAYOR_VERSION) != CODE_MAYOR_VERSION)||(sck.readEEPROM(EE_ADDR_CODE_MINOR_VERSION) != CODE_MINOR_VERSION)||(sck.readEEPROM(EE_ADDR_CODE_LETTER_VERSION) != CODE_LETTER_VERSION))
      {
        sck.writeEEPROM(EE_ADDR_CODE_MAYOR_VERSION, CODE_MAYOR_VERSION);
        sck.writeEEPROM(EE_ADDR_CODE_MINOR_VERSION, CODE_MINOR_VERSION); 
        sck.writeEEPROM(EE_ADDR_CODE_LETTER_VERSION, CODE_LETTER_VERSION); 
        
        sck.writeintEEPROM(EE_ADDR_FREE_SSID, EE_ADDR_SSID);
        sck.writeintEEPROM(EE_ADDR_FREE_PASS, EE_ADDR_PASS);
        sck.writeintEEPROM(EE_ADDR_FREE_AUTH, EE_ADDR_AUTH);
        
        sck.writeintEEPROM(EE_ADDR_NUMBER_NETS, 0x000);
        for(byte i=0; i<redes; i++)
          {
            sck.writeCommand(EE_ADDR_FREE_SSID, mySSID[i]);
            sck.writeCommand(EE_ADDR_FREE_PASS, myPassword[i]);
            sck.writeCommand(EE_ADDR_FREE_AUTH, wifiEncript[i]);
          }

        sck.writeintEEPROM(EE_ADDR_MAX_BATTERY, VAL_MAX_BATTERY);
        sck.writeintEEPROM(EE_ADDR_POST_MEASURES, 0x0000);
        sck.writeintEEPROM(EE_ADDR_NUMBER_MEASURES, 0x0000);
        sck.writeintEEPROM(EE_ADDR_FREE_ADDR_MEASURES, FREE_ADDR_MEASURES);
      }
  }

