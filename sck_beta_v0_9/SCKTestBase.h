/*
 *
 * This file is part of the SCK v0.9 - SmartCitizen
 *
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
 
// Created by Miguel Colom - http://mcolom.info - October 2nd, 2014

#ifndef __SCKTESTBASE_H__
#define __SCKTESTBASE_H__

/*
 * SCKTestBase is an abstract base class for all tests.
 * It implements the execute() method that reads the sensor values and
 * checks for variations during the testing time (less that TEST_TIMEOUT ms).
 * 
 * The read_sensor() method is virtual and must be overriden by the
 * concrete classes. This method returns the value measured by the
 * sensor when method is called.
*/

// Maximum time to complete a test (ms)
#define TEST_TIMEOUT 60000

/**
 * @brief Abstract base class for the tests. Concrete derived classes must override the method to get the sensor value.
 **/
class SCKTestBase {
private:
  char *name; // Name of the test
  bool can_be_zero; // Is it OK is the sensor returns exactly 0? Error if not.

public:
  /**
   * @brief Constructor
   *
   * @param name : name of the test;
   * @param can_be_zero : If set to zero, the test will fail if the sensor returns exactly 0.
   *
   **/
  SCKTestBase(const char *name, bool can_be_zero);

  /**
   * @brief Execute the test
   *
   * @param sensor_value : the last value of the sensor is returned here;
   * @return true if the test passed, or false otherwise.
   *
   **/
  bool execute(float &sensor_value);  

  /**
   * @brief Getter for the name of the test
   *
   * @return The name of the test.
   *
   **/
  char *get_name();

  /**
   * @brief Virtual method that returns the value of the test.  Must be overriden by concrete subclasses
   *
   * @return Value measured by that sensor when the function is called.
   *
   **/
  virtual float read_sensor() = 0; // Abstract method that must be overriden by derived classes
};

#endif

