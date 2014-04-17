#ifndef __SCKSERVER_H__
#define __SCKSERVER_H__

#include <Arduino.h>
//#include "SCKAmbient.h"
class SCKServer {
public:
   boolean time(char *time);
   void json_update(uint16_t updates, boolean terminal, long *value, char *time);
   void send(boolean sleep, boolean *wait_moment, long *value, char *time);
   boolean connect(long *value, char *time_);
   boolean reconnect();
   void addFIFO(long *value, char *time);
   void readFIFO();
private:

};
#endif
