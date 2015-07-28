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

// Only for version Goteo 1.0
#define decouplerComp   true

#if F_CPU == 8000000 
    #define FirmWare  "1.1-0.9.0-A"
#else
    #define FirmWare  "1.0-0.9.0-A"
#endif

/* 

WIFI AND SERVER STATICS - WiFly, Http server parameters.

*/
// WiFly Auth Modes:
#define OPEN   "0"
#define WEP    "1"
#define WPA    "2"
#define MIXED  "3"
#define WPA2   "4"
#define WEP64  "8"

// External Antenna:
#define EXT_ANT "1"

// Internal Antenna:
#define INT_ANT "0"

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

// Frequency of bus I2C
#define TWI_FREQ 400000L

#define WIFLY_LATEST_VERSION 441
#define DEFAULT_WIFLY_FIRMWARE "ftp update wifly3-441.img"
#define DEFAULT_WIFLY_FTP_UPDATE "set ftp address 198.175.253.161"

/* 

ARDUINO ports definitions - GPIOS and ADCs 

*/

// WIFLY AWAKE:
#define AWAKE  4
// PANEL LEVEL:
#define PANEL A8
// BAT LEVEL:
#define BAT   A7

// MICS5525_HEATHER:
#define IO0 5
// MICS2710_HEATHER:
#define IO1 13
// MICS2710_HIGH_IMPEDANCE:
#define IO2 9
// MICS2710_HIGH_IMPEDANCE:
#define IO3 10
// WIFLY - Factory RESET/AP RN131:
#define FACTORY 7
// WIFLY - CONTROL:
#define CONTROL 12

// MICS_5525:
#define S0 A4
// MICS_2710:
#define S1 A5
// SENS_5525:
#define S2 A2
// SENS_2710:
#define S3 A3
// MICRO:
#define S4 A0
// LDR:
#define S5 A1


/* 

SENSOR READINGS - Defaults

*/

// Time between updates:
#define DEFAULT_TIME_UPDATE  60
// Minimum number of updates before posting:
#define DEFAULT_MIN_UPDATES  1
// Max number of postings at a time:
#define POST_MAX             20
// Type of sensors capture (OFFLINE, NOWIFI, NORMAL, ECONOMIC)
#define DEFAULT_MODE_SENSOR  NORMAL


/* 

I2C ADDRESSES 

*/
    // Address of the RTC
#define RTC_ADDRESS          0x68
    // Address of the EEPROM
#define E2PROM               0x50

#if F_CPU == 8000000 
  // Address of the mcp1 Potentiometers that controls the MICS:
  #define MCP1               0x2E
  // Address of the mcp2  that controls the microfone pickup:
  #define MCP2               0x2F
    // Address of the mcp3 Ajust the battery charge:
  #define MCP3             0x2D
  // Address of the light sensor:
  #define bh1730             0x29
 // Address of the sht21:
  #define Temperature        0x40
  //ADXL345 device address:
  #define ADXL 0x53
#else
  // Address of the mcp1 MICS:
  #define MCP1               0x2F
  // Address of the mcp2 REGULATORS:  
  #define MCP2               0x2E
#endif

#if F_CPU == 8000000
  // KOhm
  #define R1  12
#else
  // KOhm
  #define R1  82
#endif

// KOhm
#define P1  100


/* 

Internal EEPROM Memory Addresses

*/ 

// SCK Configuration Parameters 
   //32BYTES:
#define EE_ADDR_TIME_VERSION                        0
// 4 BYTES Time between update and update of the sensors in seconds:
#define EE_ADDR_TIME_UPDATE                         32
// 4 BYTES Type sensors capture:
#define EE_ADDR_SENSOR_MODE                         36
// 4 BYTES Number of updates before posting:
#define EE_ADDR_NUMBER_UPDATES                      40
// 4 BYTES Number of updates before posting:
#define EE_ADDR_NUMBER_READ_MEASURE                 44
// 4 BYTES Number of updates before posting:
#define EE_ADDR_NUMBER_WRITE_MEASURE                48
// 4 BYTES Number of networks in the memory:
#define EE_ADDR_NUMBER_NETS                         52
// 32 BYTES Apikey of the device:
#define EE_ADDR_APIKEY                              56
// 32 BYTES MAC of the device:
#define EE_ADDR_MAC                                 100

// SCK WIFI SETTINGS Parameters
// 160 BYTES
#define DEFAULT_ADDR_SSID                                150
#define DEFAULT_ADDR_PASS                                310
#define DEFAULT_ADDR_AUTH                                470
#define DEFAULT_ADDR_ANTENNA                             630


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

// Digital potentiometer resolution:
#define RES 256
// Digital potentiometer resistance 100KOhm:
#define P1  100

// (Ohms)  Average current resistance for sensor MICS_5525/MICS_5524:
#define  Rc0  10.0

#if F_CPU == 8000000
  // (Ohms)  Average current resistance for sensor MICS_2714
  #define  Rc1  39.0
#else
  // (Ohms)  Average current resistance for sensor MICS_2710
  #define  Rc1  10.0
#endif

#if F_CPU == 8000000 
  #define  VMIC0 2734.0
  #define  VMIC1 2734.0
#else
  #define  VMIC0 5000.0
  #define  VMIC1 2500.0
#endif

#define reference 2560.0
#define second 1000
#define minute 60000

/* 

BATTERY PARAMETERS - Battery sensing calibration parameters

*/

#if F_CPU == 8000000 
  #define  VAL_MAX_BATTERY 4200
  #define  VAL_MIN_BATTERY 3000
#else
  #define  VAL_MAX_BATTERY 4050
  #define  VAL_MIN_BATTERY 3000
#endif


#define DHTLIB_INVALID_VALUE    -999

// Do not connect to server:
#define OFFLINE   0
// Do not connect to WiFi module:
#define NOWIFI    1
// Nomal or real time mode:
#define NORMAL    2
// Economic mode, sensor is gas active once per hour:
#define ECONOMIC  3

// Numbers of sensors on the board:
#define  SENSORS 9

#define buffer_length 32
static char buffer[buffer_length];

// Basic Server Posts to the SmartCitizen Platform - EndPoint: http://data.smartcitizen.me/add 
static char* WEB[8]={
                  "data.smartcitizen.me",
                  "PUT /add HTTP/1.1\n", 
                  "Host: data.smartcitizen.me \n", 
                  "User-Agent: SmartCitizen \n", 
                  "X-SmartCitizenMacADDR: ", 
                  "X-SmartCitizenApiKey: ", 
                  "X-SmartCitizenVersion: ",  
                  "X-SmartCitizenData: "};
  
// Time server request -  EndPoint: http://data.smartcitizen.me/datetime                 
static char* WEBTIME[3]={                  
                  /*Servidor de tiempo*/
                  "GET /datetime HTTP/1.1\n",
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

