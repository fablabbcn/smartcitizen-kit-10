/*
 *  Esta libreria esta basada en varias librerias, estas son:
 *  Las librerias para el modulo wifi, Arduino-wifly de furius-luke y WIFLYHQ de harlequin-tech
 *  La libreria para el sensor DHT22 http://playground.arduino.cc/Main/DHTLib
 *  La libreria para el RTC basadas en el codigo de JeeLabs 
 *  La libreria para el timer1 de arduino 
 *  I2C library Copyright (c) 2011-2012 Wayne Truchsess.  All right reserved.
 *  Busca la ultima version en https://github.com/fablabbcn/Smart-Citizen-Kit 
 */
 
#include <Arduino.h>
#include <avr/pgmspace.h>

#define debuggSCK  false

#define DEFAULT_TIME_UPDATE  "60"     //Tiempo entre actualizacion y actualizacion
#define POST_MAX             20       //Maximo numero de posteos a la vez
#define MIN_UPDATES          1       //Minimo numero de actualizaciones antes de postear
#define fast true //Frecuencia del bus i2c, false 100KHz, true 400 kHz

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
  #define bh1730             0x29    // Direcion del sensor de luz
  #define Si7005             0x40    // Direcion del si7005
  
  
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

#define  Rc0  10. //Ohm

#if F_CPU == 8000000 
  #define  Rc1  39. //Ohm
#else
  #define  Rc1  10. //Ohm
#endif

#if F_CPU == 8000000 
  #define  Vcc 3300. //mV 
  #define  VMIC0 2734.
  #define  VMIC1 2734.
#else
  #define  Vcc 5000. //mV 
  #define  VMIC0 5000.
  #define  VMIC1 2500.
#endif

#if F_CPU == 8000000 
  #define  VAL_MAX_BATTERY                             837
  #define  VAL_MIN_BATTERY                             597
#else
  #define  VAL_MAX_BATTERY                             840
  #define  VAL_MIN_BATTERY                             613
#endif


#define IO0 5  //MICS5525_HEATHER
#define IO1 13 //MICS2710_HEATHER
#define IO2 9  //MICS2710_ALTAIMPEDANCIA
#define IO3 10  //MICS2710_ALTAIMPEDANCIA

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


#define AWAKE  4 //Despertar WIFI
#define PANEL A8 //Entrada panel
#define BAT   A7 //Entrada bateria

#define DHTLIB_INVALID_VALUE    -999

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
    void config();
    boolean RTCadjust(char *time);
    char* RTCtime();
    void writeEEPROM(uint16_t eeaddress, uint8_t data );
    void writeintEEPROM(uint16_t eeaddress, uint16_t data );
    byte readEEPROM(uint16_t eeaddress );
    uint16_t readintEEPROM(uint16_t eeaddress);
    char* readData(uint16_t eeaddress, uint16_t pos, uint8_t dec );
    void writeData(uint16_t eeaddress, uint16_t pos, char* text );
    boolean compareDate(char* text, char* text1);
    boolean checkText(byte inByte, char* text, byte *check);
    boolean checkRTC();
    void checkData();
    boolean DHT22(uint8_t pin);
    int getTemperatureC(); 
    int getHumidity(); 
    unsigned long getNO2(); 
    unsigned long getCO(); 
    uint16_t getPanel();
    uint16_t getBattery();
    uint16_t getLight();
    void getSi7005();
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
    boolean server_reconnect();
    void json_update(uint16_t initial);
    boolean sleep();
    boolean reset();
    boolean open(const char *addr, int port=80);
    boolean close();
    boolean enterCommandMode();  
    boolean exitCommandMode();  
    boolean isConnected();
    
    void writeVH(byte device, long voltage );
    float readVH(byte device);
    void writeRL(byte device, long resistor);  
    float readRL(byte device);  
    
    
  private:
    uint8_t bits[5];  // buffer to receive data
    uint16_t readSi7005(uint8_t type);
    int _lastHumidity;
    int _lastTemperature;
    boolean dhtRead(uint8_t pin);
    float average(int anaPin);
    char* itoa(uint32_t number);
    void writeMCP(byte deviceaddress, byte address, int data );
    int readMCP(int deviceaddress, uint16_t address );
    
    void writeRGAIN(byte device, long resistor);  
    float readRGAIN(byte device);  
      
    void heat(byte device, int current); 
    float readMICS(byte device, unsigned long time);  

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

/*TIMER1*/

#define RESOLUTION 65536    // Timer1 is 16 bit

class TimerOne
{
  public:
  /*Timer 1*/
    void initialize(long microseconds=1000000);
    void start();
    void stop();
    void resume();
    void setPeriod(long microseconds);
    unsigned int pwmPeriod;
    unsigned char clockSelectBits;
        char oldSREG;					// To hold Status Register while ints disabled
    void attachInterrupt(void (*isr)(), long microseconds=-1);
    void (*isrCallback)();
};
extern TimerOne Timer1;


/*I2C*/

#define START           0x08
#define REPEATED_START  0x10
#define MT_SLA_ACK	0x18
#define MT_SLA_NACK	0x20
#define MT_DATA_ACK     0x28
#define MT_DATA_NACK    0x30
#define MR_SLA_ACK	0x40
#define MR_SLA_NACK	0x48
#define MR_DATA_ACK     0x50
#define MR_DATA_NACK    0x58
#define LOST_ARBTRTN    0x38
#define TWI_STATUS      (TWSR & 0xF8)
#define SLA_W(address)  (address << 1)
#define SLA_R(address)  ((address << 1) + 0x01)
#define cbi(sfr, bit)   (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit)   (_SFR_BYTE(sfr) |= _BV(bit))

#define MAX_BUFFER_SIZE 32
class I2C
{
  public:
    I2C();
    void begin();
    void end();
    void timeOut(uint16_t);
    void setSpeed(uint8_t); 
    uint8_t available();
    uint8_t receive();
    uint8_t write(uint8_t, uint8_t);
    uint8_t write(int, int); 
    uint8_t write(uint8_t, uint16_t, uint8_t, boolean);
    uint8_t write(int, int, int);
    uint8_t write(uint8_t, uint8_t, char*);
    uint8_t write(uint8_t, uint8_t, uint8_t*, uint8_t);
    uint8_t read(uint8_t, uint8_t);
    uint8_t read(int, int);
    uint8_t read(uint8_t, uint16_t, uint8_t, boolean);
    uint8_t read(int, int, int);
    uint8_t read(uint8_t, uint8_t, uint8_t*);
    uint8_t read(uint8_t, uint8_t, uint8_t, uint8_t*);


  private:
    uint8_t start();
    uint8_t sendAddress(uint8_t);
    uint8_t sendByte(uint8_t);
    uint8_t receiveByte(uint8_t);
    uint8_t stop();
    void lockUp();
    uint8_t returnStatus;
    uint8_t nack;
    uint8_t data[MAX_BUFFER_SIZE];
    static uint8_t bytesAvailable;
    static uint8_t bufferIndex;
    static uint8_t totalBytes;
    static uint16_t timeOutDelay;

};

extern I2C I2c;
