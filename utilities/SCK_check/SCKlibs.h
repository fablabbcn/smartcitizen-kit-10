#ifndef __SCKLIBS_H__
#define __SCKLIBS_H__

#include <Arduino.h>

#define TIME_BUFFER_SIZE 20 

class sck {
public:
boolean checkEEPROM();
void begin();
boolean findInResponse(const char *toMatch,
                                    unsigned int timeOut);
void recovery();
void skipRemainderOfResponse(unsigned int timeOut);
boolean sendCommand(const __FlashStringHelper *command,
                                 boolean isMultipartCommand,
                                 const char *expectedResponse);
boolean sendCommand(const char *command,
                                 boolean isMultipartCommand,
                                 const char *expectedResponse);
boolean enterCommandMode();
boolean reset();
boolean exitCommandMode();
boolean connect();
boolean repair();
boolean ready();  
char* getWiFlyVersion(unsigned long timeOut);
int checkWiFlyVersion(char *text);
boolean webAppRepair();
void writeEEPROM(uint16_t eeaddress, uint8_t data);
byte readEEPROM(uint16_t eeaddress);
boolean checkRTC();
uint16_t getPanel();
float getBattery();
float readCharge();
private:

};
#endif
