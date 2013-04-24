/*
 *  Esta libreria esta basada en varias librerias, estas son:
 *  Las librerias para el modulo wifi, Arduino-wifly de furius-luke y WIFLYHQ de harlequin-tech
 *  La libreria para el sensor DHT22 de Ben Adams
 *  La libreria para el RTC basadas en el codigo de JeeLabs 
 *  Busca la ultima version en https://github.com/fablabbcn/Smart-Citizen-Kit 
 */
 
#include <Arduino.h>
#include <Wire.h>
#include <avr/pgmspace.h>

#define debuggSCK  false

//Direcciones I2C
#define DS1307_ADDRESS     0x68    // Direcion de la RTC
#define E2PROM             0x50    // Direcion de la EEPROM
#define MCP1               0x2F    // Direcion del mcp1 MICS
#define MCP2               0x2E    // Direcion del mcp2 REGULADORES

//Espacio reservado para los parametros de configuracion del SCK
#define EE_ADDR_CODE_MAYOR_VERSION                  0  //1BYTE               
#define EE_ADDR_CODE_MINOR_VERSION                  1  //1BYTE
#define EE_ADDR_CODE_LETTER_VERSION                 2  //1BYTE
#define EE_ADDR_MAX_BATTERY                         4  //2BYTE
#define EE_ADDR_FREE_ADDR_MEASURES                  6  //2BYTE
#define EE_ADDR_POST_MEASURES                       8  //2BYTE
#define EE_ADDR_NUMBER_MEASURES                     10  //2BYTE
#define EE_ADDR_NUMBER_NETS                         12  //2BYTE
#define EE_ADDR_FREE_SSID                           14  //2BYTE
#define EE_ADDR_FREE_PASS                           16  //2BYTE
#define EE_ADDR_FREE_AUTH                           18  //2BYTE


//Espacio reservado para los SSID y PASS
//uint16_t EE_ADDR_SSID[3] =                            {100, 300, 400};
//uint16_t EE_ADDR_PASS[3] =                            {150, 350, 450};
#define EE_ADDR_SSID                                100
#define EE_ADDR_PASS                                600
#define EE_ADDR_AUTH                                1100  //2BYTE

//Espacio reservado para los datos no posteados a la web
#define EE_ADDR_MEASURES                            1200

#define VAL_MAX_BATTERY                             840
#define FREE_ADDR_UPDATE_WEB                        EE_ADDR_UPDATE_WEB
#define FREE_ADDR_MEASURES                          EE_ADDR_MEASURES


#define TWI_FREQ 400000L //Frecuencia bus I2C

#define RO_MICS_5525 450 //Kohm segun tablas estadisticas del fabricante
#define RO_MICS_2710 3   //Kohm segun tablas estadisticas del fabricante  
#define MICS_5525 0x00
#define MICS_2710 0x01

#define  Rc  10. //Ohm
#define  Vcc 5000. //mV 

#define RES 256
#define R1 82  //Kohm
#define P1 100  //Kohm 
//#define  k  10 //float k= (RES*(float)R1/100)/1000;  //Constante de conversion a tension de los reguladores 
//#define  kr  10 //float kr= ((float)P1*1000)/RES;     //Constante de conversion a resistencia de potenciometros 


#define IO0 5  //MICS5525_HEATHER
#define IO1 13 //MICS2710_HEATHER
#define IO2 9  //MICS2710_ALTAIMPEDANCIA

#define S0 A4 //MICS_5525
#define S1 A5 //MICS_2710
#define S2 A2 //SENS_5525
#define S3 A3 //SENS_2710
#define S4 A0 //MICRO
#define S5 A1 //LDR

#define AWAKE  4 //Despertar WIFI
#define PANEL A8 //Entrada panel
#define BAT   A7 //Entrada bateria

#define DHT22_PIN 10 //Entrada/Salida DHT22
#define DHT22_ERROR_VALUE -995

#define SCAN_SSID_BUFFER_SIZE 33 // 
#define OPEN  "0"
#define WEP   "1"
#define WPA1  "2"
#define WPA2  "4"
#define WEP64 "8"

#define EXT_ANT "1" // antena externa
#define INT_ANT "0" // antena interna
                  
class SmartCitizen {
  public:  
    void begin();
    boolean RTCadjust(char *time);
    char* RTCtime();
    void writeEEPROM(uint16_t eeaddress, byte data );
    void writeintEEPROM(uint16_t eeaddress, uint16_t data );
    byte readEEPROM(uint16_t eeaddress );
    uint16_t readintEEPROM(uint16_t eeaddress );
    char* readCommand(uint16_t eeaddress, uint16_t *pointer );
    void writeCommand(uint16_t eeaddress, char* text );
    boolean check_text(byte inByte, char* text, byte *check);
    void check_data();
    void DHT22(uint8_t pin);
    boolean dhtRead();
    int getTemperatureC(); 
    int getHumidity(); 
    unsigned long getNO2(); 
    unsigned long getCO(); 
    uint16_t getPanel();
    uint16_t getBattery();
    uint16_t getLight();
    uint16_t getNoise();
    void getMICS(unsigned long time0, unsigned long time1);  
    void updateSensors(byte mode);
    
    /*Wifi*/
    boolean connect();
    void APmode(char* ssid);
    boolean ready();
    char* mac();
    char* id();
    char* scan();
    char* WIFItime();
    boolean server_connect();
    void json_updtate();
    boolean sleep();
    boolean open(const char *addr, int port=80);
    boolean close();
    boolean enterCommandMode();  
    boolean exitCommandMode();  
    boolean isConnected();
    
  private:
    uint8_t _bitmask;
    volatile uint8_t *_baseReg;
    unsigned long _lastReadTime;
    short int _lastHumidity;
    short int _lastTemperature;
    float average(int anaPin);
    char* itoa(uint32_t number);
    void writeMCP(byte deviceaddress, byte address, int data );
    int readMCP(int deviceaddress, byte address );
    void VH_MICS(byte device, long voltage );
    float readVH(byte device);
    void RL_MICS(byte device, long resistor);  
    /*Wifi*/
    boolean connected;
    boolean findInResponse(const char *toMatch, unsigned int timeOut);
    void skipRemainderOfResponse(unsigned int timeOut);
    boolean sendCommand(const char *command,
                    boolean isMultipartCommand, // Has default value
                    const char *expectedResponse); // Has default value
    boolean sendCommand(const __FlashStringHelper *command,
                    boolean isMultipartCommand, // Has default value
                    const char *expectedResponse); // Has default value
};


