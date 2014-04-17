#define debugEnabled   true
#define decouplerComp   true

#define WIFLY_LATEST_VERSION 441
#define DEFAULT_WIFLY_FIRMWARE "ftp update wifly3-441.img"
#define DEFAULT_WIFLY_FTP_UPDATE "set ftp address 198.175.253.161"

#if F_CPU == 8000000 
    #define FirmWare  "1.1-0.9.0-A"
#else
    #define FirmWare  "1.0-0.9.0-A"
#endif

#define AWAKE  4 //Despertar WIFI
#define PANEL A8 //Entrada panel
#define BAT   A7 //Entrada bateria

#define IO0 5  //MICS5525_HEATHER
#define IO1 13 //MICS2710_HEATHER
#define IO2 9  //MICS2710_ALTAIMPEDANCIA
#define IO3 10  //MICS2710_ALTAIMPEDANCIA
#define FACTORY 7  //factory RESET/AP RN131
#define CONTROL 12  //Control Mode

#define S0 A4 //MICS_5525
#define S1 A5 //MICS_2710
#define S2 A2 //SENS_5525
#define S3 A3 //SENS_2710
#define S4 A0 //MICRO
#define S5 A1 //LDR

#define DEFAULT_TIME_UPDATE  60     //Tiempo entre actualizacion y actualizacion
#define DEFAULT_MIN_UPDATES  1      //Minimo numero de actualizaciones antes de postear

#define POST_MAX             20       //Maximo numero de posteos a la vez

//Direcciones I2C
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

#define buffer_length        32
static char buffer[buffer_length];

#define RES 256   //Resolucion de los potenciometros digitales

#if F_CPU == 8000000 
  #define R1  12    //Kohm
#else
  #define R1  82    //Kohm
#endif

#define P1  100   //Kohm 


//Espacio reservado para los parametros de configuracion del SCK  
#define EE_ADDR_TIME_VERSION                        0   //32BYTES 
#define EE_ADDR_TIME_UPDATE                         32  //16BYTES Tiempo entre actualizacion y actualizacion de los sensores en segundos
#define EE_ADDR_NUMBER_UPDATES                      48  //4BYTES  Numero de actualizaciones antes de postear
#define EE_ADDR_NUMBER_READ_MEASURE                 52  //4BYTES  Numero de actualizaciones antes de postear
#define EE_ADDR_NUMBER_WRITE_MEASURE                56  //4BYTES  Numero de actualizaciones antes de postear
#define EE_ADDR_NUMBER_NETS                         60  //4BYTE Numero de redes en memoria
#define EE_ADDR_APIKEY                              64  //32BYTES Apikey del dispositivo
#define EE_ADDR_MAC                                 100  //32BYTES Apikey del dispositivo

//Espacio reservado para los SSID y PASS
#define DEFAULT_ADDR_SSID                                150  //160 BYTES
#define DEFAULT_ADDR_PASS                                310  //160 BYTES
#define DEFAULT_ADDR_AUTH                                470  //160 BYTES 
#define DEFAULT_ADDR_ANTENNA                             630  //160 BYTES

//Espacio reservado para los datos no posteados a la web
#define DEFAULT_ADDR_MEASURES                            0

#define MICS_5525 0x00
#define MICS_2710 0x01

#define RES 256   //Resolucion de los potenciometros digitales
#define P1  100   //Resistencia potenciometro digitales 100Kohm 

#define  Rc0  10. //Ohm  Resistencia medica de corriente en el sensor MICS_5525/MICS_5524

#if F_CPU == 8000000 
  #define  Rc1  39. //Ohm Resistencia medica de corriente en el sensor MICS_2714
#else
  #define  Rc1  10. //Ohm Resistencia medica de corriente en el sensor MICS_2710
#endif

#if F_CPU == 8000000 
  #define  VMIC0 2734.
  #define  VMIC1 2734.
#else
  #define  VMIC0 5000.
  #define  VMIC1 2500.
#endif

#define reference 2560.

#if F_CPU == 8000000 
  #define  VAL_MAX_BATTERY                             4200
  #define  VAL_MIN_BATTERY                             3000
#else
  #define  VAL_MAX_BATTERY                             4050
  #define  VAL_MIN_BATTERY                             3000
#endif


#define DHTLIB_INVALID_VALUE    -999

#define APMODE  0
#define NORMAL  1
#define WPA    2

#define OPEN  "0"
#define WEP   "1"
#define WPA   "2"
#define MIXED  "3"
#define WPA2  "4"
#define WEP64 "8"

#define EXT_ANT "1" // antena externa
#define INT_ANT "0" // antena interna

#define  SENSORS 9  //Numero de sensores en la placa

static char* WEB[8]={
                  "data.smartcitizen.me",
                  "PUT /add HTTP/1.1 \n", 
                  "Host: data.smartcitizen.me \n", 
                  "User-Agent: SmartCitizen \n", 
                  "X-SmartCitizenMacADDR: ", 
                  "X-SmartCitizenApiKey: ", 
                  "X-SmartCitizenVersion: ",  
                  "X-SmartCitizenData: "};
                  
static char* WEBTIME[3]={                  
                  /*Servidor de tiempo*/
                  "GET /datetime HTTP/1.1 \n",
                  "Host: data.smartcitizen.me \n",
                  "User-Agent: SmartCitizen \n\n"  
                  };
                  
static char* SERVER[11]={
                  "{\"temp\":\"",
                  "\",\"hum\":\"", 
                  "\",\"light\":\"",
                  "\",\"bat\":\"",
                  "\",\"panel\":\"",
                  "\",\"co\":\"", 
                  "\",\"no2\":\"", 
                  "\",\"noise\":\"", 
                  "\",\"nets\":\"", 
                  "\",\"timestamp\":\"", 
                  "\"}"
                  };
                  
static char* SENSOR[10]={
                  "Temperature: ",
                  "Humidity: ",
                  "Light: ",
                  "Battery: ",
                  "Solar Panel: ",
                  "Carbon Monxide: ",
                  "Nitrogen Dioxide: ",
                  "Noise: ",
                  "Wifi Spots: ",
                  "UTC: " 
                };

static char* UNITS[9]={
                  #if F_CPU == 8000000 
                    " C RAW",
                    " % RAW",
                  #else
                    " C",
                    " %",
                  #endif
                  #if F_CPU == 8000000 
                    " lx",
                  #else
                    " %",
                  #endif
                    " %",
                    " mV",
                    " kOhm",
                    " kOhm",
                    " mV",
                    "",
                  };  
