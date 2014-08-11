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

  SCKAmbient.h
  Supports the sensor reading and calibration functions.

  - Sensors supported (sensors use on board custom peripherials):

    - TEMP / HUM (DHT22 and HPP828E031)
    - NOISE
    - LIGHT (LDR and BH1730FVC)
    - CO (MICS5525 and MICS4514)
    - NO2 (MiCS2710 and MICS4514)

*/


/* 

SENSOR Contants and Defaults

*/

#ifndef __SCKAMBIENT_H__
#define __SCKAMBIENT_H__

#include <Arduino.h>

#define TIME_BUFFER_SIZE 20 

class SCKAmbient {
public:
  void begin();
  void ini();
  void execute();
  void writeGAIN(long value);
  float readGAIN(); 
  void GasSensor(boolean active);
  void getMICS(); 
  unsigned long getCO();
  unsigned long getNO2();

  void getSHT21(); 
  boolean getDHT22();
  #if F_CPU == 8000000 
    uint32_t getTemperature();
    uint32_t getHumidity();
  #else
    int getTemperature();
    int getHumidity();
  #endif
  
  void readADXL(byte address, int num, byte buff[]);
  uint16_t getLight(); 
  unsigned int getNoise();
  
  void txDebug();
  boolean debug_state();
  
  void serialRequests();
private:
  void writeVH(byte device, long voltage );  
  float readVH(byte device);
  void writeRL(byte device, long resistor);
  float readRL(byte device);
  void writeRGAIN(byte device, long resistor);
  float readRGAIN(byte device);
  void getVcc();
  void heat(byte device, int current);
  float readRs(byte device);
  float readMICS(byte device);
  void writeADXL(byte address, byte val);
  void averageADXL();
  void updateSensors(byte mode);
  uint16_t readSHT21(uint8_t type);
  boolean DhtRead(uint8_t pin);
  boolean addData(byte inByte);
  boolean printNetWorks(unsigned int address_eeprom);
  void addNetWork(unsigned int address_eeprom, char* text);  
};

static SCKAmbient ambient_;

#endif
