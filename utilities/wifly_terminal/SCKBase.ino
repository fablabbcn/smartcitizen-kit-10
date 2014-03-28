
#define AWAKE  4 //Despertar WIFI
#define PANEL A8 //Entrada panel
#define BAT   A7 //Entrada bateria

#define IO0 5  //MICS5525_HEATHER
#define IO1 13 //MICS2710_HEATHER
#define IO2 9  //MICS2710_ALTAIMPEDANCIA
#define IO3 10  //MICS2710_ALTAIMPEDANCIA
#define FACTORY 7  //factory RESET/AP RN131
#define CONTROL 12  //Control Mode

#define S0 A4 //MICS_5525
#define S1 A5 //MICS_2710
#define S2 A2 //SENS_5525
#define S3 A3 //SENS_2710
#define S4 A0 //MICRO
#define S5 A1 //LDR

#define OPEN  "0"
#define WEP   "1"
#define WPA1  "2"
#define WPA2  "4"
#define WEP64 "8"

#define INT_ANT "0" //EXT_ANT
#define EXT_ANT "1" //EXT_ANT

char* mySSID = "Red";
char* myPassword = "Pass";
char* wifiEncript = WPA2;
char* antenna  = INT_ANT; //EXT_ANT


boolean connected;                  

#define buffer_length        32
static char buffer[buffer_length];

void sckBegin() {
  Serial.begin(115200);
  Serial1.begin(9600);
  pinMode(IO0, OUTPUT); //VH_MICS5525
  pinMode(IO1, OUTPUT); //VH_MICS2710
  pinMode(IO2, OUTPUT); //MICS2710_ALTAIMPEDANCIA
  pinMode(AWAKE, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(FACTORY, OUTPUT);
  pinMode(CONTROL, INPUT);
  digitalWrite(AWAKE, LOW); 
  digitalWrite(FACTORY, LOW); 
}  

 boolean sckFindInResponse(const char *toMatch,
                                    unsigned int timeOut = 1000) {
  int byteRead;

  unsigned long timeOutTarget; // in milliseconds

  for (unsigned int offset = 0; offset < strlen(toMatch); offset++) {
    timeOutTarget = millis() + timeOut; // Doesn't handle timer wrapping
    while (!Serial1.available()) {
      // Wait, with optional time out.
      if (timeOut > 0) {
        if (millis() > timeOutTarget) {
          return false;
        }
      }
      delay(1); // This seems to improve reliability slightly
    }
    byteRead = Serial1.read();
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

void sckRecovery()
{
    Serial.println("Reseting..."); 
    digitalWrite(FACTORY, HIGH);
    delay(1000);
    digitalWrite(FACTORY, LOW);
    delay(1000);
    digitalWrite(FACTORY, HIGH);
    delay(1000);
    digitalWrite(FACTORY, LOW);
    delay(1000);
    digitalWrite(FACTORY, HIGH);
    delay(1000);
    digitalWrite(FACTORY, LOW);
    delay(1000);
    digitalWrite(FACTORY, HIGH);
    delay(1000);
    digitalWrite(FACTORY, LOW);
    delay(1000);
    digitalWrite(FACTORY, HIGH);
    delay(1000);
    digitalWrite(FACTORY, LOW);
    delay(1000);
    Serial1.println();
    sckFindInResponse("<WEB_APP", 3000);
    sckReset();
    Serial.println("Successfully reset"); 
    //digitalWrite(FACTORY, HIGH);
    sckEnterCommandMode();
}

void sckSkipRemainderOfResponse(unsigned int timeOut) {
  unsigned long time = millis();
  while (((millis()-time)<timeOut))
  {
    if (Serial1.available())
      { 
        byte temp = Serial1.read();
        //Serial.write(temp);
        time = millis();
      }
  }
}

boolean sckSendCommand(const __FlashStringHelper *command,
                                 boolean isMultipartCommand = false,
                                 const char *expectedResponse = "AOK") {
  Serial1.print(command);
  delay(20);
  if (!isMultipartCommand) {
    Serial1.flush();
    Serial1.println();

    // TODO: Handle other responses
    //       (e.g. autoconnect message before it's turned off,
    //        DHCP messages, and/or ERR etc)
    if (!sckFindInResponse(expectedResponse, 3000)) {
      return false;
    }
    //sckFindInResponse(expectedResponse);
  }
  return true;
}

boolean sckSendCommand(const char *command,
                                 boolean isMultipartCommand = false,
                                 const char *expectedResponse = "AOK") {
  Serial1.print(command);
  delay(20);
  if (!isMultipartCommand) {
    Serial1.flush();
    Serial1.println();

    // TODO: Handle other responses
    //       (e.g. autoconnect message before it's turned off,
    //        DHCP messages, and/or ERR etc)
    if (!sckFindInResponse(expectedResponse, 3000)) {
      return false;
    }
    //findInResponse(expectedResponse);
  }
  return true;
}

#define COMMAND_MODE_ENTER_RETRY_ATTEMPTS 2

#define COMMAND_MODE_GUARD_TIME 250 // in milliseconds

boolean sckEnterCommandMode() {
    for (int retryCount = 0; retryCount < COMMAND_MODE_ENTER_RETRY_ATTEMPTS; retryCount++) 
     {
      delay(COMMAND_MODE_GUARD_TIME);
      Serial1.print(F("$$$"));
      delay(COMMAND_MODE_GUARD_TIME);
      Serial1.println();
      Serial1.println();
      if (sckFindInResponse("\r\n<", 1000))
      {
        return true;
      }
    }
    return false;
}


boolean sckSleep() {
      sckEnterCommandMode();
      sckSendCommand(F("sleep"));
}

boolean sckReset() {
      //sckEnterCommandMode();
      sckSendCommand(F("factory R"), false, "Set Factory Defaults"); // Store settings
      sckSendCommand(F("save"), false, "Storing in config"); // Store settings
      delay(1000);
      sckSendCommand(F("reboot"), false, "*READY*");
}

boolean sckUpdate() {
      if (sckEnterCommandMode())
      {
        sckSendCommand(F("ftp update"));
        delay(1000);
        if (sckFindInResponse("FTP OK.", 10000))
        {
          return true;
        }
      }
      else return false;
}

boolean sckExitCommandMode() {
    for (int retryCount = 0; retryCount < COMMAND_MODE_ENTER_RETRY_ATTEMPTS; retryCount++) 
     {
      if (sckSendCommand(F("exit"), false, "EXIT")) 
      {
      return true;
      }
    }
    return false;
}

boolean sckConnect()
  {
    if (!sckReady())
    {
      if(sckEnterCommandMode())
        {    
            sckSendCommand(F("set wlan join 1")); // Disable AP mode
            sckSendCommand(F("set ip dhcp 1")); // Enable DHCP server
            sckSendCommand(F("set ip proto 10")); //Modo TCP y modo HTML
            sckSendCommand(F("set wlan auth "), true);
            sckSendCommand(wifiEncript);
            boolean mode = true;
            if ((wifiEncript==WEP)||(wifiEncript==WEP64)) mode=false;
            sckSendCommand(F("set wlan ssid "), true);
            sckSendCommand(mySSID);
            if (mode) sckSendCommand(F("set wlan phrase "), true);  // WPA1, WPA2, OPEN
            else sckSendCommand(F("set wlan key "), true);
            sckSendCommand(myPassword);
            sckSendCommand(F("set wlan ext_antenna "), true);
            sckSendCommand(antenna);
            sckSendCommand(F("save"), false, "Storing in config"); // Store settings
            sckSendCommand(F("reboot"), false, "*READY*");
            return true;
        }
      else return false;     
    }
   else return true;  
  }  

uint32_t baud[7]={2400, 4800, 9600, 19200, 38400, 57600, 115200};

void sckRepair()
{
  if(!sckEnterCommandMode())
    {
      boolean repair = true;
      for (int i=6; ((i>=0)&&repair); i--)
      {
        Serial1.begin(baud[i]);
        Serial.println(baud[i]);
        if(sckEnterCommandMode()) 
        {
          sckReset();
          repair = false;
        }
        Serial1.begin(9600);
      }
    }
}

boolean sckReady()
{
  if(!sckEnterCommandMode())
    {
      sckRepair();
    }
  if (sckEnterCommandMode())
    {
      Serial1.println(F("join"));
      if (sckFindInResponse("Associated!", 8000)) 
      {
        sckSkipRemainderOfResponse(3000);
        sckExitCommandMode();
        return(true);
      }
   } 
  else return(false);
}
  
char* itoa(int32_t number)
  {
   byte count = 0;
   uint32_t temp;
   if (number < 0) {temp = number*(-1); count++;} 
   while ((temp/10)!=0) 
   {
     temp = temp/10;
     count++;
   }
   int i;
   if (number < 0) {temp = number*(-1);} 
   else temp = number;
   for (i = count; i>=0; i--) 
   { 
     buffer[i] = temp%10 + '0'; 
     temp = temp/10; 
   }
   if (number < 0) {buffer[0] = '-';} 
   buffer[count + 1] = 0x00;
   return buffer;   
  }
  


