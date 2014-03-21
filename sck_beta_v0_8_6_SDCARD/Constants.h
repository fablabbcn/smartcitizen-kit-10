#define debuggSCK     false
#define decouplerComp true
#define DataRaw       true

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

#define DEFAULT_TIME_UPDATE  "60"     //Tiempo entre actualizacion y actualizacion
#define DEFAULT_MIN_UPDATES  "1"      //Minimo numero de actualizaciones antes de postear

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

//Espacio reservado para los parametros de configuracion del SCK  
#define EE_ADDR_TIME_VERSION                        0   //32BYTES 
#define EE_ADDR_TIME_UPDATE                         32  //16BYTES Tiempo entre actualizacion y actualizacion de los sensores en segundos
#define EE_ADDR_NUMBER_UPDATES                      48  //4BYTES  Numero de actualizaciones antes de postear


#define MICS_5525 0x00
#define MICS_2710 0x01

#define  Rc0  10. //Ohm  Resistencia medica de corriente en el sensor MICS_5525/MICS_5524

#if F_CPU == 8000000 
  #define  Rc1  39. //Ohm Resistencia medica de corriente en el sensor MICS_2714
#else
  #define  Rc1  10. //Ohm Resistencia medica de corriente en el sensor MICS_2710
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

#if F_CPU == 8000000 
  #define  VAL_MAX_BATTERY                             4200
  #define  VAL_MIN_BATTERY                             3000
#else
  #define  VAL_MAX_BATTERY                             4050
  #define  VAL_MIN_BATTERY                             3000
#endif


#define DHTLIB_INVALID_VALUE    -999

