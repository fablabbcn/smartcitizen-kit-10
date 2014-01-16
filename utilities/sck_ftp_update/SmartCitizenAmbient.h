/*
 *  Esta libreria esta basada en varias librerias, estas son:
 *  Las librerias para el modulo wifi, Arduino-wifly de furius-luke y WIFLYHQ de harlequin-tech
 *  La libreria para el sensor DHT22 de Ben Adams
 *  La libreria para el RTC basadas en el codigo de JeeLabs 
 *  Busca la ultima version en https://github.com/fablabbcn/Smart-Citizen-Kit 
 */
 
#ifndef SMARTCITIZENAMBIENT_h
#define SMARTCITIZENAMBIENT_h

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
#define EE_ADDR_SIGNATURE_CODE1                     0  //1BYTE
#define EE_ADDR_SIGNATURE_CODE2                     1  //1BYTE
#define EE_ADDR_SIGNATURE_CODE3                     2  //1BYTE
#define EE_ADDR_SIGNATURE_CODE4                     3  //1BYTE
#define EE_ADDR_CODE_MAYOR_VERSION                  4  //1BYTE               
#define EE_ADDR_CODE_MINOR_VERSION                  5  //1BYTE
#define EE_ADDR_MAX_BATTERY                         6  //1BYTE
#define EE_ADDR_FREE_ADDR_UPDATE_WEB                8  //2BYTE                
#define EE_ADDR_FREE_ADDR_MEASURES                  10  //2BYTE

//Espacio reservado para la configuracion de subida a la web
#define EE_ADDR_UPDATE_WEB                          100

//Espacio reservado para los datos no posteados a la web
#define EE_ADDR_MEASURES                            2000

//Valores por defecto
#define SIGNATURE_CODE1             23 // day 
#define SIGNATURE_CODE2             03 // month
#define SIGNATURE_CODE3             20 // century
#define SIGNATURE_CODE4             13 // year of century
#define CODE_MAYOR_VERSION          0 
#define CODE_MINOR_VERSION          4 

#define VAL_MAX_BATTERY             840
#define FREE_ADDR_UPDATE_WEB        EE_ADDR_UPDATE_WEB
#define FREE_ADDR_MEASURES          EE_ADDR_MEASURES

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
//#define  k  10 //float k= (RES*(float)R1/100)/1000;  //Constante de conversion a tensión de los reguladores 
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
  
class ClockTime {
public:
    byte year; 
    byte month; 
    byte day; 
    byte hours; 
    byte minutes; 
    byte seconds; 
};

class SmartCitizen {
  public:  
    void begin();
    void RTCadjust(ClockTime time);
    void RTCtime();
    void writeEEPROM(unsigned int eeaddress, byte data );
    byte readEEPROM(unsigned int eeaddress );
    char* readCommand(unsigned int eeaddress, unsigned int *pointer );
    void writeCommand(unsigned int eeaddress, char* text );
    int decimal(float temp); 
    void DHT22(uint8_t pin);
    boolean dhtRead();
    float getTemperatureC(); 
    float getHumidity(); 
    float getNO2(); 
    float getCO(); 
    float getPanel();
    float getBattery();
    float getLight();
    float getNoise();
    float getMICS(unsigned long time0, unsigned long time1);  
    /*Wifi*/
    void config(char* ssid, char* pass, char* auth, char* antenna);
    boolean wifi_ready();
    const char * ip();
    ClockTime WIFItime();
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
    float mapfloat(long x, long in_min, long in_max, long out_min, long out_max);
    void writeMCP(byte deviceaddress, byte address, int data );
    int readMCP(int deviceaddress, byte address );
    void VH_MICS(byte device, long voltage );
    float readVH(byte device);
    void RL_MICS(byte device, long resistor);  
    uint8_t bcd2bin (uint8_t val);
    uint8_t bin2bcd (uint8_t val);
    /*Wifi*/
    boolean connected;
    boolean findInResponse(const char *toMatch, unsigned int timeOut);
    void skipRemainderOfResponse();
    boolean sendCommand(const char *command,
                    boolean isMultipartCommand, // Has default value
                    const char *expectedResponse); // Has default value
    boolean sendCommand(const __FlashStringHelper *command,
                    boolean isMultipartCommand, // Has default value
                    const char *expectedResponse); // Has default value
};

#endif
