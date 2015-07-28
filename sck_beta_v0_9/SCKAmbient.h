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
  /**
   * @brief Board initialization
   **/
  void begin();

  /**
   * @brief Communication and timing initialization
   **/
  void ini();

  /**
   * @brief Timer callback to transmit data to platform
   **/
  void execute();

  /**
   * @brief Setter for the noise sensor gain
   * @param value : noise sensor gain
   **/
  void writeGAIN(long value);

  /**
   * @brief Getter for the noise sensor gain
   * @return The sensor gain
   **/
  float readGAIN(); 

  /**
   * @brief Switches on/off the gas sensor
   * @param active : true to active the sensor/false to deactivate
   **/
  void GasSensor(boolean active);

  /**
   * @brief Heats and updates cached MICS data (CO and NO2 gas levels)
   **/
  void getMICS();
   
  /**
   * @brief Getter for the CO gas level
   **/
  unsigned long getCO();

  /**
   * @brief Getter for the NO2 gas level
   * @return The NO2 level
   **/
  unsigned long getNO2();

  /**
   * @brief Updates SHT21 cached data (temperature and humidity)
   **/
  void getSHT21();
    
  /**
   * @brief Updates SHT21 cached data (temperature and humidity)
   * @return Boolean indicating if the read was successful
   **/
  boolean getDHT22();
  
  #if F_CPU == 8000000 
    /**
     * @brief Getter for the temperature
     * @return Read temperature
     **/
    uint32_t getTemperature();

    /**
     * @brief Getter for the humidity
     * @return Read humidity
     **/
    uint32_t getHumidity();
  #else
    /**
     * @brief Getter for the temperature
     * @return Read temperature
     **/
    int getTemperature();

    /**
     * @brief Getter for the humidity
     * @return Read humidity
     **/
    int getHumidity();
  #endif
  
  /**
   * @brief Reads the accelerometer
   * @param address : address of the accelerometer device
   * @param num : register to read
   * @param buff : buffer where to read accelerometer output
   **/
  void readADXL(byte address, int num, byte buff[]);

  /**
   * @brief Reads the light level
   * @return Light level
   **/
  uint16_t getLight(); 

  /**
   * @brief Reads the noise level
   * @return Noise level
   **/
  unsigned int getNoise();
  
  /**
   * @brief Prints sensor debug information to the serial console
   **/
  void txDebug();

  /**
   * @brief Getter for the debug state (print to the serial console)
   * @return Debug state
   **/
  boolean debug_state();

  /**
   * @brief Wakes up the board the process WiFly requests
   **/
  void serialRequests();

private:
  /**
   * @brief Sets a device voltage
   * @param device : address of the device
   * @param voltage : voltage for the device
   **/
  void writeVH(byte device, long voltage );

  /**
   * @brief Gets the voltage of a device
   * @param device : address of the device
   * @return Voltage of the device
   **/
  float readVH(byte device);

  /**
   * @brief Writes a resistor value to a device
   * @param device : address of the device
   * @param resistor : resistor value
   **/
  void writeRL(byte device, long resistor);

  /**
   * @brief Reads a resistor value from a device
   * @param device : address of the device
   * @return Resistance of the device
   **/
  float readRL(byte device);

  /**
   * @brief Sets the gain of a device according to the resistor
   * @param device : address of the device
   * @param resistor : resistor value
   **/
  void writeRGAIN(byte device, long resistor);

  /**
   * @brief Reads the gain from a device
   * @param device : address of the device
   * @return Gain for the device
   **/
  float readRGAIN(byte device);
    
  /**
   * @brief Updates (in cache) the general Vcc voltage on the board
   **/
  void getVcc();
  
  /**
   * @brief Heats a device with the specified current
   * @param device : address of the device
   * @param current : heating current (mA)
   **/  
  void heat(byte device, int current);
  
  /**
   * @brief Reads the resistance of a device (ohms)
   * @param device : address of the device
   * @return Resistance of the device (ohms)
   **/    
  float readRs(byte device);
  
  /**
   * @brief Reads (without cache) the resistance Rs of the MICS
   * @param device : address of the device
   * @return Resistance Rs of the MICS
   **/      
  float readMICS(byte device);
  
  /**
   * @brief Writes a value at the specified register of the accelerometer
   * @param device : address of the device
   * @param val : value to write
   **/        
  void writeADXL(byte address, byte val);
  
  /**
   * @brief Updates the cached accelerometer values with their average
   **/          
  void averageADXL();
  
  /**
   * @brief Updates the cached values of all sensors on board
   * @param mode : ECONOMIC for economic mode
   **/
  void updateSensors(byte mode);
  

  /**
   * @brief Reads a non-cached value of the temperature
   * @param type : type of measurement
   * @return Temperature value
   **/
  uint16_t readSHT21(uint8_t type);

  /**
   * @brief Reads a pin for the DHT (temperature/humidity) sensor
   * @param pin : pin to read
   * @return Boolean value of the read pin at the DHT
   **/
  boolean DhtRead(uint8_t pin);
  
  /**
   * @brief Adds a byte to the 0-terminated internal buffer
   * @param inByte : byte to add
   * @return True if end of line
   **/  
  int addData(byte inByte);
  
  /**
   * @brief Debug function to print the WiFi networks
   * @param inByte : address of the EEPROM where the number of networks is stored
   **/ 
  void printNetWorks(unsigned int address_eeprom);
  
  /**
   * @brief Adds a WiFi network
   * @param address_eeprom : address of the EEPROM where to write the name of the network
   * @param text : name of the new network
   **/ 
  void addNetWork(unsigned int address_eeprom, char* text);  
};

static SCKAmbient ambient_;

#endif
