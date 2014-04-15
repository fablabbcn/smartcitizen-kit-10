/*

  Constants.h
  Defines ATMEGA32U4 pins and other SENSORS and COMUNICATIONS static parameters.

*/


/* 

WIFLY Firmware Setting

*/

#define debuggSCK     false   //Use only for test sensors

#define WIFLY_LATEST_VERSION 441
#define DEFAULT_WIFLY_FIRMWARE "ftp update wifly3-441.img"
#define DEFAULT_WIFLY_FTP_UPDATE "set ftp address 198.175.253.161"

#if F_CPU == 8000000 
  #if DataRaw 
    #define FirmWare  "1.1-0.8.6-A"
  #else
    #define FirmWare  "1.1-0.8.6-B"
  #endif
#else
  #if DataRaw 
    #define FirmWare  "1.0-0.8.6-A"
  #else
    #define FirmWare  "1.0-0.8.6-B"
  #endif
#endif

/* 

ARDUINO ports definitions - GPIOS and ADCs 

*/

#define AWAKE  4      // WIFLY AWAKE
#define PANEL A8      // PANEL LEVEL
#define BAT   A7      // BAT LEVEL 

#define IO0 5         // MICS5525_HEATHER
#define IO1 13        // MICS2710_HEATHER
#define IO2 9         // MICS2710_HIGH_IMPEDANCE
#define IO3 10        // MICS2710_HIGH_IMPEDANCE
#define FACTORY 7     // WIFLY - Factory RESET/AP RN131
#define CONTROL 12    // WIFLY - CONTROL

#define S0 A4         //MICS_5525
#define S1 A5         //MICS_2710
#define S2 A2         //SENS_5525
#define S3 A3         //SENS_2710
#define S4 A0         //MICRO
#define S5 A1         //LDR

/* 

SENSOR READINGS - Defaults

*/

#define DEFAULT_TIME_UPDATE  "60"     //Tiempo entre actualizacion y actualizacion
#define DEFAULT_MIN_UPDATES  "1"      //Minimo numero de actualizaciones antes de postear
#define POST_MAX             20       //Maximo numero de posteos a la vez

/* 

i2c ADDRESSES 

*/

#define RTC_ADDRESS          0x68    // Direcion de la RTC
#define E2PROM               0x50    // Direcion de la EEPROM

#if F_CPU == 8000000 
  #define MCP1               0x2E    // Direcion del mcp1 Potenciometros que controlan los MICS
  #define MCP2               0x2F    // Direcion del mcp2 Potenciometros que controlan la ganancia del microfono
  #define MCP3               0x2D    // Direcion del mcp3 Ajuste carga bateria
  #define bh1730             0x29    // Direcion del sensor de luz
  #define Temperature        0x40    // Direcion del sht21    
  #define ADXL 0x53    //ADXL345 device address
#else
  #define MCP1               0x2F    // Direcion del mcp1 MICS
  #define MCP2               0x2E    // Direcion del mcp2 REGULADORES
#endif

/* 

EEPROM Memory Addresses

*/

// SCK Configuration Parameters 
#define EE_ADDR_TIME_VERSION                        0   //32BYTES 
#define EE_ADDR_TIME_UPDATE                         32  //16BYTES Tiempo entre actualizacion y actualizacion de los sensores en segundos
#define EE_ADDR_NUMBER_UPDATES                      48  //4BYTES  Numero de actualizaciones antes de postear
#define EE_ADDR_NUMBER_MEASURES                     64  //2BYTE Numero de medidas en memoria
#define EE_ADDR_NUMBER_NETS                         66  //2BYTE Numero de redes en memoria
#define EE_ADDR_APIKEY                              68  //32BYTES Apikey del dispositivo
#define EE_ADDR_MAC                              100  //32BYTES Apikey del dispositivo

// SCK WIFI SETTINGS Parameters
#define DEFAULT_ADDR_SSID                                200
#define DEFAULT_ADDR_PASS                                520
#define DEFAULT_ADDR_AUTH                                840  
#define DEFAULT_ADDR_ANTENNA                             1160 

// SCK DATA SPACE (Sensor readings can be stored here to do batch updates)
#define DEFAULT_ADDR_MEASURES                            1500

/* 

MICS PARAMETERS - Gas Sensor Addresses and Defaults

*/


#define MICS_5525 0x00
#define MICS_2710 0x01

#define  Rc0  10.       //Ohm.  Average current resistance for sensor MICS_5525/MICS_5524

#if F_CPU == 8000000 
  #define  Rc1  39.     //Ohm.  Average current resistance for sensor MICS_2714
#else
  #define  Rc1  10.     //Ohm.  Average current resistance for sensor MICS_2710
#endif

#if F_CPU == 8000000 
  float    Vcc = 3300.; //mV 
  #define  VMIC0 2734.
  #define  VMIC1 2734.
#else
  float    Vcc = 5000.; //mV 
  #define  VMIC0 5000.
  #define  VMIC1 2500.
#endif

#define reference 2560.

/* 

BATTERY PARAMETERS - Battery sensing calibration parameters

*/

#if F_CPU == 8000000 
  #define  VAL_MAX_BATTERY                             4200
  #define  VAL_MIN_BATTERY                             3000
#else
  #define  VAL_MAX_BATTERY                             4050
  #define  VAL_MIN_BATTERY                             3000
#endif

/* 

DHT PARAMS - Temp and Hum sensor

*/

#define DHTLIB_INVALID_VALUE    -999


/* 

WIFI AND SERVER STATICS - WiFly, Http server parameters.

*/
// WiFly Auth Modes
#define OPEN  "0"
#define WEP   "1"
#define WPA1  "2"
#define WPA2  "4"
#define WEP64 "8"

#define EXT_ANT "1" // External Antenna
#define INT_ANT "0" // Internal Antenna

// Basic Server Posts to the SmartCitizen Platform - EndPoint: http://data.smartcitizen.me/add 
char* WEB[8]={
                  "data.smartcitizen.me",
                  "PUT /add HTTP/1.1 \n", 
                  "Host: data.smartcitizen.me \n", 
                  "User-Agent: SmartCitizen \n", 
                  "X-SmartCitizenMacADDR: ", 
                  "X-SmartCitizenApiKey: ", 
                  "X-SmartCitizenVersion: ",  
                  "X-SmartCitizenData: "};
            
// Time server request -  EndPoint: http://data.smartcitizen.me/datetime 
char* WEBTIME[3]={                  
                  /*Servidor de tiempo*/
                  "GET /datetime HTTP/1.1 \n",
                  "Host: data.smartcitizen.me \n",
                  "User-Agent: SmartCitizen \n\n"  
                  };

