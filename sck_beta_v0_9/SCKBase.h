/*

  SCKBase.h
  Supports core and data management functions (Power, WiFi, SD storage, RTClock and EEPROM storage)

  - Modules supported:

    - WIFI (Microchip RN131 (WiFly))
    - RTC (DS1339U and DS1307Z)
    - EEPROM (24LC256)
    - POWER MANAGEMENT IC's

*/

#ifndef __SCKBASE_H__
#define __SCKBASE_H__

#include <Arduino.h>

class SCKBase {
public:
    void begin();
    void config();
    float average(int anaPin);
    boolean checkText(char* text, char* text1);
    boolean compareData(char* text, char* text1);
    void writeMCP(byte deviceaddress, byte address, int data );
    int readMCP(int deviceaddress, uint16_t address );
    float readCharge();
    void writeCharge(int current);
    void writeEEPROM(uint16_t eeaddress, uint8_t data);
    byte readEEPROM(uint16_t eeaddress);
    void writeData(uint32_t eeaddress, long data, uint8_t location);
    void writeData(uint32_t eeaddress, uint16_t pos, char* text, uint8_t location);
    char* readData(uint16_t eeaddress, uint16_t pos, uint8_t location);
    uint32_t readData(uint16_t eeaddress, uint8_t location);
    
    uint16_t getPanel(float Vref);
    uint16_t getBattery(float Vref);
    
    /*RTC commands*/
    boolean checkRTC();
    boolean RTCadjust(char *time);
    boolean RTCtime(char *time);

    
    /*Wifi commands*/
    boolean findInResponse(const char *toMatch,
                           unsigned int timeOut);
    void skipRemainderOfResponse(unsigned int timeOut);
    boolean sendCommand(const __FlashStringHelper *command,
                        boolean isMultipartCommand,
                        const char *expectedResponse);
    boolean sendCommand(const char *command,
                        boolean isMultipartCommand,
                        const char *expectedResponse);
    boolean enterCommandMode();
    boolean sleep();
    boolean reset();
    boolean exitCommandMode();
    boolean connect();
    void APmode(char* ssid);
    boolean ready();
    boolean open(const char *addr, int port);
    boolean close();
    char* MAC();
    char* id();
    uint32_t scan();
    int checkWiFly();
    int getWiFlyVersion();
    boolean update();
    void repair();
    
    /*Timer commands*/
    void timer1SetPeriod(long microseconds);
    void timer1Initialize();
    void timer1Stop();
private:
    
};
#endif
