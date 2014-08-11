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

#define TIMEOUT 10000

#define COMMAND_MODE_ENTER_RETRY_ATTEMPTS 2

#define COMMAND_MODE_GUARD_TIME 250 // in milliseconds


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
    //
    void writeEEPROM(uint16_t eeaddress, uint8_t data);
    byte readEEPROM(uint16_t eeaddress);
    //
    void writeByte(uint16_t eeaddress, uint8_t data, uint8_t location);
    void writeData(uint32_t eeaddress, long data, uint8_t location);
    void writeData(uint32_t eeaddress, uint16_t pos, char* text, uint8_t location);
    //
    byte readByte(uint16_t eeaddress, uint8_t location);
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
    
    /* i2c and pin communication */
    static void i2c_transaction(int device, int data, int num);
    static void i2c_transaction(int device, int data);
    static void i2c_transaction_reg_val(int device, int address, int val);
    static void i2c_write_many(int device, byte data[], int num);
    static boolean wait_wire_available(int timeout_ms);
    //      
    static boolean wait_pin_change(int pin, int current_value);
    
    
    /*Timer commands*/
    void timer1SetPeriod(long microseconds);
    void timer1Initialize();
    void timer1Stop();
private:
};

static SCKBase base_;

#endif
