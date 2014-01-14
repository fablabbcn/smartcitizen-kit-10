#ifndef __WIFLYHTML_H__
#define __WIFLYHTML_H__

#include <Arduino.h>

#define debug false

#define SCAN_SSID_BUFFER_SIZE 33 // 
#define OPEN  "0"
#define WEP   "1"
#define WPA1  "2"
#define WPA2  "4"
#define WEP64 "8"

#define EXT_ANT "1" // antena externa
#define INT_ANT "0" // antena interna


class network_results {
public:
  char ssid[10][SCAN_SSID_BUFFER_SIZE]; 
};

class WiFlyHTML {
public:
    void begin(char* ssid, char* pass, char* auth, char* antenna);
    boolean join();
    boolean ready();
    const char * ip();
    byte scan(network_results *networks);
    boolean reboot();
    boolean reset();
    boolean sleep();
    boolean open(const char *addr, int port=80);
    boolean close();
    boolean enterCommandMode();  
    boolean exitCommandMode();  
    boolean baudrate(long baudrate); 
    boolean isConnected();
  private:
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

