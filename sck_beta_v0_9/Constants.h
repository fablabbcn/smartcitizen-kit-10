/*
 *
 * This file is part of the SCK v0.9 - SmartCitizen
 *
 * This file may be licensed under the terms of of the
 * GNU General Public License Version 2 (the ``GPL'').
 *
 * Software distributed under the License is distributed
 * on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied. See the GPL for the specific language
 * governing rights and limitations.
 *
 * You should have received a copy of the GPL along with this
 * program. If not, go to http://www.gnu.org/licenses/gpl.html
 * or write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */


/*

  Constants.h
  Defines ATMEGA32U4 pins and other SENSORS and COMUNICATIONS static parameters.

*/

#define debugEnabled   true

//Only for version Goteo 1.0
#define decouplerComp   true

#if F_CPU == 8000000 
    #define FirmWare  "1.1-0.9.0-A"
#else
    #define FirmWare  "1.0-0.9.0-A"
#endif

/* 

WIFI AND SERVER STATICS - WiFly, Http server parameters.

*/
// WiFly Auth Modes
#define OPEN   "0"
#define WEP    "1"
#define WPA    "2"
#define MIXED  "3"
#define WPA2   "4"
#define WEP64  "8"

#define EXT_ANT "1" // External Antenna
#define INT_ANT "0" // Internal Antenna

/* 

WIFLY Firmware Setting

*/

#define networks 1
#if (networks > 0)
static char* mySSID[networks]      = { 
  "SSID" };
static char* myPassword[networks]  = { 
  "password" };
static char* wifiEncript[networks] = { 
  WEP };
static char* antennaExt[networks]  = { 
  INT_ANT };
#endif      

#define TWI_FREQ 400000L //Frecuencia bus I2C

#define WIFLY_LATEST_VERSION 441
#define DEFAULT_WIFLY_FIRMWARE "ftp update wifly3-441.img"
#define DEFAULT_WIFLY_FTP_UPDATE "set ftp address 198.175.253.161"

/* 

ARDUINO ports definitions - GPIOS and ADCs 

*/

#define AWAKE  4      // WIFLY AWAKE
#define PANEL A8      // PANEL LEVEL
#define BAT   A7      // BAT LEVEL 

#define IO0 5         // MICS5525_HEATHER
#define IO1 13        // MICS2710_HEATHER
#define IO2 9         // MICS2710_HIGH_IMPEDANCE
#define IO3 10        // MICS2710_HIGH_IMPEDANCE
#define FACTORY 7     // WIFLY - Factory RESET/AP RN131
#define CONTROL 12    // WIFLY - CONTROL

#define S0 A4         //MICS_5525
#define S1 A5         //MICS_2710
#define S2 A2         //SENS_5525
#define S3 A3         //SENS_2710
#define S4 A0         //MICRO
#define S5 A1         //LDR


/* 

SENSOR READINGS - Defaults

*/

#define DEFAULT_TIME_UPDATE  60     //Time between update and update
#define DEFAULT_MIN_UPDATES  1      //Minimum number of updates before posting
#define POST_MAX             20     //Max number of postings at a time
#define DEFAULT_MODE_SENSOR  NORMAL     //Type sensors capture (OFFLINE, NOWIFI, NORMAL, ECONOMIC)


/* 

i2c ADDRESSES 

*/
#define RTC_ADDRESS          0x68    // Direction of the RTC
#define E2PROM               0x50    // Direction of the EEPROM

#if F_CPU == 8000000 
  #define MCP1               0x2E    // Direction of the mcp1 Potenciometers that control the MICS
  #define MCP2               0x2F    // Direction of the mcp2 Potenciometers that control the microfone pickup
  #define bh1730             0x29    // Direction of the light sensor
  #define Temperature        0x40    // Direction of the sht21    
  #define ADXL 0x53    //ADXL345 device address
#else
  #define MCP1               0x2F    // Direction of the mcp1 MICS
  #define MCP2               0x2E    // Direction of the mcp2 REGULATORS
#endif

#if F_CPU == 8000000 
  #define R1  12    //Kohm
#else
  #define R1  82    //Kohm
#endif

#define P1  100   //Kohm 


/* 

Internal EEPROM Memory Addresses

*/ 

// SCK Configuration Parameters 
#define EE_ADDR_TIME_VERSION                        0   //32BYTES 
#define EE_ADDR_TIME_UPDATE                         32  //4BYTES Time between update and update of the sensors in seconds
#define EE_ADDR_SENSOR_MODE                         36  //4BYTES Type sensors capture
#define EE_ADDR_NUMBER_UPDATES                      40  //4BYTES Number of updates before posting
#define EE_ADDR_NUMBER_READ_MEASURE                 44  //4BYTES Number of updates before posting
#define EE_ADDR_NUMBER_WRITE_MEASURE                48  //4BYTES Number of updates before posting
#define EE_ADDR_NUMBER_NETS                         52  //4BYTES Number of networks in the memory 
#define EE_ADDR_APIKEY                              56  //32BYTES Apikey of the device
#define EE_ADDR_MAC                                 100  //32BYTES MAC of the device

// SCK WIFI SETTINGS Parameters
#define DEFAULT_ADDR_SSID                                150  //160 BYTES
#define DEFAULT_ADDR_PASS                                310  //160 BYTES
#define DEFAULT_ADDR_AUTH                                470  //160 BYTES 
#define DEFAULT_ADDR_ANTENNA                             630  //160 BYTES


/* 

External EEPROM Memory Addresses

*/ 

// SCK DATA SPACE (Sensor readings can be stored here to do batch updates)
#define DEFAULT_ADDR_MEASURES                            0


/* 

MICS PARAMETERS - Gas Sensor Addresses and Defaults

*/

#define MICS_5525 0x00
#define MICS_2710 0x01

#define RES 256   // Digital pot. resolution
#define P1  100   //Digital potentiometer resistance 100Kohm

#define  Rc0  10.       //Ohm.  Average current resistance for sensor MICS_5525/MICS_5524

#if F_CPU == 8000000 
  #define  Rc1  39.     //Ohm.  Average current resistance for sensor MICS_2714
#else
  #define  Rc1  10.     //Ohm.  Average current resistance for sensor MICS_2710
#endif

#if F_CPU == 8000000 
  #define  VMIC0 2734.
  #define  VMIC1 2734.
#else
  #define  VMIC0 5000.
  #define  VMIC1 2500.
#endif

#define reference 2560.
#define second 1000
#define minute 60000

/* 

BATTERY PARAMETERS - Battery sensing calibration parameters

*/

#if F_CPU == 8000000 
  #define  VAL_MAX_BATTERY                             4200
  #define  VAL_MIN_BATTERY                             3000
#else
  #define  VAL_MAX_BATTERY                             4050
  #define  VAL_MIN_BATTERY                             3000
#endif


#define DHTLIB_INVALID_VALUE    -999

#define OFFLINE   0  //No connect to server
#define NOWIFI    1  //No connect arduino to wifi module
#define NORMAL    2  //Nomal mode o real time
#define ECONOMIC  3  //Economic mode, sensor gas active one time for hour

#define  SENSORS 9  //Numbers of sensors in the board

#define buffer_length        32
static char buffer[buffer_length];

// Basic Server Posts to the SmartCitizen Platform - EndPoint: http://data.smartcitizen.me/add 
static char* WEB[8]={
                  "data.smartcitizen.me",
                  "PUT /add HTTP/1.1 \n", 
                  "Host: data.smartcitizen.me \n", 
                  "User-Agent: SmartCitizen \n", 
                  "X-SmartCitizenMacADDR: ", 
                  "X-SmartCitizenApiKey: ", 
                  "X-SmartCitizenVersion: ",  
                  "X-SmartCitizenData: "};
  
// Time server request -  EndPoint: http://data.smartcitizen.me/datetime                 
static char* WEBTIME[3]={                  
                  /*Servidor de tiempo*/
                  "GET /datetime HTTP/1.1 \n",
                  "Host: data.smartcitizen.me \n",
                  "User-Agent: SmartCitizen \n\n"  
                  };

// Data JSON structure                  
static char* SERVER[11]={
                  "{\"temp\":\"",
                  "\",\"hum\":\"", 
                  "\",\"light\":\"",
                  "\",\"bat\":\"",
                  "\",\"panel\":\"",
                  "\",\"co\":\"", 
                  "\",\"no2\":\"", 
                  "\",\"noise\":\"", 
                  "\",\"nets\":\"", 
                  "\",\"timestamp\":\"", 
                  "\"}"
                  };
                  
static char* SENSOR[10]={
                  "Temperature: ",
                  "Humidity: ",
                  "Light: ",
                  "Battery: ",
                  "Solar Panel: ",
                  "Carbon Monxide: ",
                  "Nitrogen Dioxide: ",
                  "Noise: ",
                  "Wifi Spots: ",
                  "UTC: " 
                };

static char* UNITS[9]={
                  #if F_CPU == 8000000 
                    " C RAW",
                    " % RAW",
                  #else
                    " C",
                    " %",
                  #endif
                  #if F_CPU == 8000000 
                    " lx",
                  #else
                    " %",
                  #endif
                    " %",
                    " mV",
                    " kOhm",
                    " kOhm",
                    " mV",
                    "",
                  };  
