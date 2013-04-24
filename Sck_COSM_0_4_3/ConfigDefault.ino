#if configDEFAULT

#define mySSID           "COSM"  
#define myPassword       "1c2c3c4c5c6c7"
#define wifiEncript      WPA2
#define antenna          INT_ANT // ANTENNA INTEGRADA

#define  PACHUBE_FEED    "100285" // Hospitalet
#define  APIKEY          "ssaoVAZPglmNtZFoFfw0IQQUz5OSAKxmZDFhVVNENzZjUT0g"  // Hospitalet

#define  WEB             "api.pachube.com"
#define  PUT             "PUT /v2/feeds/"
#define  CSV             ".csv HTTP/1.1\n"
#define  HOST            "Host: api.pachube.com\n"
#define  PACHUBE_KEY     "X-PachubeApiKey: "
#define  AGENT           "User-Agent: SCK 2.0\n"
#define  TYPE            "Content-Type: text/csv\n"
#define  CONTENT         "Content-Length: "
#define  CLOSE           "Connection: close\n"
#define  ENTER           "\n"

void config(){
    
    sck.config(mySSID, myPassword, wifiEncript, antenna);
    
    //delay(1000);
    
    sck.writeEEPROM(EE_ADDR_SIGNATURE_CODE1, SIGNATURE_CODE1);
    sck.writeEEPROM(EE_ADDR_SIGNATURE_CODE2, SIGNATURE_CODE2);
    sck.writeEEPROM(EE_ADDR_SIGNATURE_CODE3, SIGNATURE_CODE3);
    sck.writeEEPROM(EE_ADDR_SIGNATURE_CODE4, SIGNATURE_CODE4);
    sck.writeEEPROM(EE_ADDR_CODE_MAYOR_VERSION, CODE_MAYOR_VERSION);
    sck.writeEEPROM(EE_ADDR_CODE_MINOR_VERSION, CODE_MINOR_VERSION); 
    sck.writeEEPROM(EE_ADDR_MAX_BATTERY , highByte(VAL_MAX_BATTERY));
    sck.writeEEPROM(EE_ADDR_MAX_BATTERY  + 1, lowByte(VAL_MAX_BATTERY)); 
    sck.writeEEPROM(EE_ADDR_FREE_ADDR_UPDATE_WEB, highByte(FREE_ADDR_UPDATE_WEB));
    sck.writeEEPROM(EE_ADDR_FREE_ADDR_UPDATE_WEB + 1, lowByte(FREE_ADDR_UPDATE_WEB));
    sck.writeEEPROM(EE_ADDR_FREE_ADDR_MEASURES, highByte(FREE_ADDR_MEASURES));
    sck.writeEEPROM(EE_ADDR_FREE_ADDR_MEASURES + 1, lowByte(FREE_ADDR_MEASURES)); 
    
    sck.writeCommand(EE_ADDR_FREE_ADDR_UPDATE_WEB, WEB);
    sck.writeCommand(EE_ADDR_FREE_ADDR_UPDATE_WEB, PUT);
    sck.writeCommand(EE_ADDR_FREE_ADDR_UPDATE_WEB, PACHUBE_FEED);
    sck.writeCommand(EE_ADDR_FREE_ADDR_UPDATE_WEB, CSV);
    sck.writeCommand(EE_ADDR_FREE_ADDR_UPDATE_WEB, HOST);
    sck.writeCommand(EE_ADDR_FREE_ADDR_UPDATE_WEB, PACHUBE_KEY);
    sck.writeCommand(EE_ADDR_FREE_ADDR_UPDATE_WEB, APIKEY);
    sck.writeCommand(EE_ADDR_FREE_ADDR_UPDATE_WEB, ENTER);
    sck.writeCommand(EE_ADDR_FREE_ADDR_UPDATE_WEB, AGENT);
    sck.writeCommand(EE_ADDR_FREE_ADDR_UPDATE_WEB, TYPE);
    sck.writeCommand(EE_ADDR_FREE_ADDR_UPDATE_WEB, CONTENT);
    sck.writeCommand(EE_ADDR_FREE_ADDR_UPDATE_WEB, CLOSE);
  }
#endif 
