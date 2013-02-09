#include "WiFlyHTML.h"
#include <Arduino.h>

boolean WiFlyHTML::findInResponse(const char *toMatch,
                                    unsigned int timeOut = 1000) {
  int byteRead;

  unsigned long timeOutTarget; // in milliseconds

  for (unsigned int offset = 0; offset < strlen(toMatch); offset++) {
    timeOutTarget = millis() + timeOut; // Doesn't handle timer wrapping
    while (!uart->available()) {
      // Wait, with optional time out.
      if (timeOut > 0) {
        if (millis() > timeOutTarget) {
          return false;
        }
      }
      delay(1); // This seems to improve reliability slightly
    }
    byteRead = uart->read();
    //Serial.print((char)byteRead);
    delay(1); // Removing logging may affect timing slightly

    if (byteRead != toMatch[offset]) {
      offset = 0;
      // Ignore character read if it's not a match for the start of the string
      if (byteRead != toMatch[offset]) {
        offset = -1;
      }
      continue;
    }
  }

  return true;
}

void WiFlyHTML::skipRemainderOfResponse() {
    while (!(uart->available() && (uart->read() == '\n'))) {
      // Skip remainder of response
    }
}

boolean WiFlyHTML::sendCommand(const __FlashStringHelper *command,
                                 boolean isMultipartCommand = false,
                                 const char *expectedResponse = "AOK") {
  uart->print(command);
  delay(20);
  if (!isMultipartCommand) {
    uart->flush();
    uart->println();

    // TODO: Handle other responses
    //       (e.g. autoconnect message before it's turned off,
    //        DHCP messages, and/or ERR etc)
    if (!findInResponse(expectedResponse, 3000)) {
      return false;
    }
    //findInResponse(expectedResponse);
  }
  return true;
}

boolean WiFlyHTML::sendCommand(const char *command,
                                 boolean isMultipartCommand = false,
                                 const char *expectedResponse = "AOK") {
  uart->print(command);
  delay(20);
  if (!isMultipartCommand) {
    uart->flush();
    uart->println();

    // TODO: Handle other responses
    //       (e.g. autoconnect message before it's turned off,
    //        DHCP messages, and/or ERR etc)
    if (!findInResponse(expectedResponse, 3000)) {
      return false;
    }
    //findInResponse(expectedResponse);
  }
  return true;
}

#define COMMAND_MODE_ENTER_RETRY_ATTEMPTS 2

#define COMMAND_MODE_GUARD_TIME 250 // in milliseconds

boolean WiFlyHTML::enterCommandMode() {
    for (int retryCount = 0; retryCount < COMMAND_MODE_ENTER_RETRY_ATTEMPTS; retryCount++) 
     {
      delay(COMMAND_MODE_GUARD_TIME);
      uart->print(F("$$$"));
      delay(COMMAND_MODE_GUARD_TIME);
      uart->println();
      uart->println();
      uart->println(F("ver"));
      if (findInResponse("\r\nWiFly Ver", 1000)) 
      {
      return true;
      }
    }
    return false;
}

boolean WiFlyHTML::exitCommandMode() {
    for (int retryCount = 0; retryCount < COMMAND_MODE_ENTER_RETRY_ATTEMPTS; retryCount++) 
     {
      if (sendCommand(F("exit"), false, "EXIT")) 
      {
      return true;
      }
    }
    return false;
}


#define SOFTWARE_REBOOT_RETRY_ATTEMPTS 2

boolean WiFlyHTML::reboot() {
  for (int retryCount = 0; retryCount < SOFTWARE_REBOOT_RETRY_ATTEMPTS; retryCount++) 
    {   
      enterCommandMode();
      uart->println(F("reboot"));
      if (findInResponse("*READY*", 2000)) {
        return true;
      }
    }
  return false;
}

#define SOFTWARE_RESET_RETRY_ATTEMPTS 2

boolean WiFlyHTML::reset() {
  for (int retryCount = 0; retryCount < SOFTWARE_RESET_RETRY_ATTEMPTS; retryCount++) 
    { 
      enterCommandMode();
      if (sendCommand(F("factory RESET"), false, "Set Factory Defaults")) {
        return true;
      }
    }
  return false;
}

boolean WiFlyHTML::sleep() {
      enterCommandMode();
      sendCommand(F("sleep"));
}

void WiFlyHTML::setConfiguration(char* ssid, char* pass, char* auth, char* antenna) {
  if (enterCommandMode())
  {
    #if debug
    /*
      if(sendCommand(F("set comm time 1"))) Serial.println("OK1");
      
      if(sendCommand(F("set comm size 1024"))) Serial.println("OK2");
      
      sendCommand(F("set wlan ssid "), true);
      if(sendCommand(ssid)) Serial.println("OK3");
      sendCommand(F("set wlan phrase "), true);
      if(sendCommand(pass)) Serial.println("OK4");
        
      if(sendCommand(F("set ip proto 10"))) Serial.println("OK5");
        
      if(sendCommand(F("set comm remote 0"))) Serial.println("OK6");
      if(sendCommand(F("set t z 1"))) Serial.println("OK7");
      if(sendCommand(F("set time address 129.6.15.28"))) Serial.println("OK8");
      if(sendCommand(F("set time port 123"))) Serial.println("OK9");
      if(sendCommand(F("set t e 15"))) Serial.println("OK10");
      ///// ///// ///// ///// ///// /////
      sendCommand(F("set wlan auth "), true);
      if(sendCommand(auth)) Serial.println("OK11");
      //sendCommand(F("set wlan auth 4")); // red sin password!!!
      ///// ///// ///// ///// ///// /////
      
      if(sendCommand(F("set wlan ext_antenna 0"))) Serial.println("OK12");
      if(sendCommand(antenna)) Serial.println("OK13");
      
      if(sendCommand(F("set ip dhcp 1"))) Serial.println("OK14");
      if(sendCommand(F("save"), false, "Storing in config")) Serial.println("OK15");
      */
    #else
      // TODO: Handle configuration better
      // Turn off auto-connect
      sendCommand(F("set comm time 1"));
      
      sendCommand(F("set comm size 1024"));
      
      sendCommand(F("set wlan ssid "), true);
      sendCommand(ssid);
      
      sendCommand(F("set wlan auth "), true);
      sendCommand(auth);
      
      if( (auth == "1") || (auth == "8") )
        sendCommand(F("set wlan key "), true);  // WEP, WEP64 -> characters in HEX!!!!!
      else
        sendCommand(F("set wlan phrase "), true);  // WPA1, WPA2, OPEN
      
      sendCommand(pass);
        
      sendCommand(F("set ip proto 10")); //Modo TCP
        
      sendCommand(F("set comm remote 0"));
      sendCommand(F("set t z 1"));  //Zona horaria
      sendCommand(F("set time address 129.6.15.28")); //Servidor de reloj
      sendCommand(F("set time port 123")); //Puerto del servidor
      sendCommand(F("set t e 15")); //Toma hora cada 15 min
      ///// ///// ///// ///// ///// /////

      sendCommand(F("set wlan ext_antenna "), true); // ANTENA: 0-> interna, 1-> externa
      sendCommand(antenna);
      
      sendCommand(F("set ip dhcp 1"));
      sendCommand(F("save"), false, "Storing in config");
    #endif
  }
  exitCommandMode();
}

boolean WiFlyHTML::baudrate(long baudrate_temp)
{
  if (enterCommandMode())
  {
    sendCommand(F("set uart baudrate "), true);
    uart->print(baudrate_temp);
    sendCommand("");
    //delay(100);
    uart->println(F("save"));
    delay(100);
    reboot();
    //uart->println("exit");
    return true;
  }
 return false;
} 

WiFlyHTML::WiFlyHTML(Stream* newUart) {
  uart = newUart;
}

void WiFlyHTML::begin(char* ssid, char* pass, char* auth, char* ant) {
	//reboot();
        setConfiguration(ssid, pass, auth, ant);
}

boolean WiFlyHTML::open(const char *addr, int port) {
  
  if (connected) {
	close();
    } 
  if (enterCommandMode())
  {
    sendCommand(F("open "), true);
    sendCommand(addr, true);
    uart->print(" ");
    uart->print(port);
    if (sendCommand("", false, "*OPEN*")) 
    {
      connected = true;
      return true;
    }
    else return false;
  }
  else return false;
}

boolean WiFlyHTML::isConnected()
{
    return connected;
}

boolean WiFlyHTML::close() {
  if (!connected) {
	return true;
    }
  if (sendCommand(F("close"), false, "*CLOS*")) {
    //skipRemainderOfResponse();
    connected = false;
    //exitCommandMode();
    //uart->println("exit");
    return true;
  }
  return false;
}

boolean WiFlyHTML::join() {
  if (enterCommandMode())
  {
    if (sendCommand(F("join"), false, "Associated!")) {
      skipRemainderOfResponse();
      exitCommandMode();
      return true;
    }
  }
  return false;
}



#define IP_ADDRESS_BUFFER_SIZE 16 // "255.255.255.255\0"

const char * WiFlyHTML::ip() {
  /*

    The return value is intended to be dropped directly
    into calls to 'print' or 'println' style methods.

   */
  static char ip[IP_ADDRESS_BUFFER_SIZE] = "";

  // TODO: Ensure we're not in a connection?

  if (enterCommandMode()) 
  {
      delay(5000);
      // Version 2.19 of the WiFly firmware has a "get ip a" command but
      // we can't use it because we want to work with 2.18 too.
      if (sendCommand(F("get ip"), false, "IP="))
      {
          char newChar;
          byte offset = 0;
    
          // Copy the IP address from the response into our buffer
          while (offset < IP_ADDRESS_BUFFER_SIZE) {
             newChar = uart->read();
             //Serial.println(newChar);
            if (newChar == ':') {
              ip[offset] = '\x00';
              break;
            } 
            else if (newChar != -1) {
              ip[offset] = newChar;
              offset++;
            }
          }
          ip[IP_ADDRESS_BUFFER_SIZE-1] = '\x00';
          
          // This should skip the remainder of the output.
          // TODO: Handle this better?
          /*waitForResponse("<");
          while (uart->read() != ' ') {
          // Skip the prompt
          }*/
          
          findInResponse("> ");
          exitCommandMode();
      }        
  }
  
  return ip;
}

byte WiFlyHTML::scan(network_results *network) {
  char num = '0';
  char i = 0;
  static char net[5] = { 0x0D, 0x0A, 0x20, 0x31, '\x00'};
  if (enterCommandMode())
    {
      if (sendCommand(F("scan"), false, "Found "))
      {
        num = uart->read();
        if ((num > '0')&&(num <= '9'))
        {
          while ((i+'0') < num) 
            {
              net[3]=i+1+'0';
              if (findInResponse(net)) 
              {
                char newChar;
                byte offset = 0;
                newChar = uart->read();
                while (((newChar == -1)||(newChar == ' '))) newChar = uart->read();
                while (offset < SCAN_SSID_BUFFER_SIZE) {
                    if (newChar == ' ') {
                      network -> ssid[i][offset] = '\x00';
                      break;
                    } 
                    else if ((newChar != -1)) {
                          network -> ssid[i][offset] = newChar;
                          offset++;
                    }
                    newChar = uart->read();
                 }
                network -> ssid[i][SCAN_SSID_BUFFER_SIZE-1] = '\x00';
              }
              i++;
           }
         }
       }
       exitCommandMode();
    }   
  if ((num > '0')&&(num <= '9')) num = num-'0';
  else num = 0;
  
  return (num);
}
