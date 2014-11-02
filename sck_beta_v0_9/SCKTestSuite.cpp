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

#include "SCKTestSuite.h"
#include "SCKTestBase.h"

void SCKTestSuite::run_all_tests() {
  SCKTestBase* tests[NUM_TESTS] = {new LightTest("Light", false),
                                   new NoiseTest("Noise", true),
                                   new COTest("CO", false),
                                   new NO2Test("CO2", false),
                                   new TemperatureTest("Temperature", false),
                                   new HumidityTest("Humidity", false)};
                                   

  Serial.println(F("*** Board in test mode. Starting tests."));

  bool all_passed = true;
  for (int i = 0; i < NUM_TESTS; i++) {
    SCKTestBase *test = tests[i];

    Serial.print(F("- Testing sensor: "));
    Serial.println(test->get_name());
    
    float sensor_value;
    bool res = test->execute(sensor_value);
    if (res) {
      Serial.print(F("Test passed. Sensor value: "));
      Serial.println(sensor_value);
    }
    else {
      all_passed = false;
      Serial.println(F("*** TEST FAILED"));
      break;
    }
  }
  
  Serial.println(all_passed ? F("*** ALL TESTS PASSED") : F("*** TESTS FAILED"));
}

