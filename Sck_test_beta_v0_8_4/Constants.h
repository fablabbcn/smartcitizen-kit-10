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

#if F_CPU == 8000000 
  #define IO4 A1 //LDR
#else
  #define S5 A1 //LDR
#endif

#define DEFAULT_TIME_UPDATE  "60"     //Tiempo entre actualizacion y actualizacion
#define POST_MAX             20       //Maximo numero de posteos a la vez
#define MIN_UPDATES          5       //Minimo numero de actualizaciones antes de postear
#define fast true //Frecuencia del bus i2c, false 100KHz, true 400 kHz
#define SensorModel 1 //0 DHT22, 1 SHT21, 2 SI7005

#define buffer_length        32

//Direcciones I2C
#if F_CPU == 8000000 
  #define RTC_ADDRESS        0x6F    // Direcion de la RTC
#else
  #define RTC_ADDRESS        0x68    // Direcion de la RTC
#endif

#define E2PROM               0x50    // Direcion de la EEPROM

#if F_CPU == 8000000 
  #define MCP1               0x2E    // Direcion del mcp1 Potenciometros que controlan los MICS
  #define MCP2               0x2F    // Direcion del mcp2 Potenciometros que controlan la ganancia del microfono
  #define MCP3               0x2D    // Direcion del mcp3 Ajuste carga bateria
  #define bh1730             0x29    // Direcion del sensor de luz
  #define Temperature        0x40    // Direcion del si7005    
#else
  #define MCP1               0x2F    // Direcion del mcp1 MICS
  #define MCP2               0x2E    // Direcion del mcp2 REGULADORES
#endif

//Espacio reservado para los parametros de configuracion del SCK  
#define EE_ADDR_TIME_VERSION                        0   //32BYTES 
#define EE_ADDR_TIME_UPDATE                         32  //32BYTES
#define EE_ADDR_NUMBER_MEASURES                     64  //2BYTE
#define EE_ADDR_NUMBER_NETS                         66  //2BYTE
#define EE_ADDR_APIKEY                              68  //32BYTES

//Espacio reservado para los SSID y PASS
#define DEFAULT_ADDR_SSID                                200
#define DEFAULT_ADDR_PASS                                520
#define DEFAULT_ADDR_AUTH                                840  
#define DEFAULT_ADDR_ANTENNA                             1160 

//Espacio reservado para los datos no posteados a la web
#define DEFAULT_ADDR_MEASURES                            1500

#define MICS_5525 0x00
#define MICS_2710 0x01

#define  Rc0  10. //Ohm  Resistencia medica de corriente en el sensor MICS_5525/MICS_5524

#if F_CPU == 8000000 
  #define  Rc1  39. //Ohm Resistencia medica de corriente en el sensor MICS_2714
#else
  #define  Rc1  10. //Ohm Resistencia medica de corriente en el sensor MICS_2710
#endif

#if F_CPU == 8000000 
  #define  Vcc 3000. //mV 
  #define  VMIC0 2734.
  #define  VMIC1 2734.
#else
  #define  Vcc 5000. //mV 
  #define  VMIC0 5000.
  #define  VMIC1 2500.
#endif

#define  VAL_MAX_BATTERY                             4200
#define  VAL_MIN_BATTERY                             3000

#define DHTLIB_INVALID_VALUE    -999

#define OPEN  "0"
#define WEP   "1"
#define WPA1  "2"
#define WPA2  "4"
#define WEP64 "8"

#define EXT_ANT "1" // antena externa
#define INT_ANT "0" // antena interna

