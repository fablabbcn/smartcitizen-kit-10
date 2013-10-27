//I2C.h   - I2C library
//  Copyright (c) 2011-2012 Wayne Truchsess.  All right reserved.

#include <Arduino.h>
#include <avr/pgmspace.h>                 

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
    uint8_t write(uint8_t, uint16_t, uint8_t, boolean); //Si boolean true es modo direccionamiento 16bits
    uint8_t write(int, int, int);
    uint8_t write(uint8_t, uint8_t, char*);
    uint8_t write(uint8_t, uint8_t, uint8_t*, uint8_t);
    uint8_t read(uint8_t, uint8_t);
    uint8_t read(int, int);
    uint8_t read(uint8_t, uint16_t, uint8_t, boolean); //Si boolean true es modo direccionamiento 16bits
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
