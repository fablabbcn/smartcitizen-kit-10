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

#include "SCKAmbient.h"
#include "SCKTestBase.h"


SCKTestBase::SCKTestBase(const char *name, bool can_be_zero) {
  this->name = (char*)name;
  this->can_be_zero = can_be_zero;
}

bool SCKTestBase::execute(float &sensor_value) {
  float last_value = this->read_sensor();
  
  // Check if the sensor returns exactly zero.
  // It could be possible, but it is likely to be a sensor failure.
  if (!this->can_be_zero && last_value == 0) return false;
  
  unsigned long last_millis = millis();
  float current_value = this->read_sensor();
  while (current_value == last_value &&
         millis() - last_millis < TEST_TIMEOUT) {
    current_value = this->read_sensor();
  }

  sensor_value = current_value;
  return current_value != last_value;
}
  
char* SCKTestBase::get_name() {
  return (char*)this->name;
}

// Must be overriden by base classes
float SCKTestBase::read_sensor() {
  // Do nothing here. Override in derived test classes
}

