/*

  SCKServer.h
  
*/


#ifndef __SCKSERVER_H__
#define __SCKSERVER_H__

#include <Arduino.h>

class SCKServer {
public:
   boolean time(char *time);
   void json_update(uint16_t updates, long *value, char *time, boolean isMultipart);
   void send(boolean sleep, boolean *wait_moment, long *value, char *time);
   boolean update(long *value, char *time_);
   boolean connect();
   void addFIFO(long *value, char *time);
   void readFIFO();
private:

};
#endif
