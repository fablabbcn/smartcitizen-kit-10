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

#ifndef __SCKTESTSUITE_H__
#define __SCKTESTSUITE_H__

#include "SCKTestBase.h"
#include "SCKTestSuite.h"
#include "SCKAmbient.h"

#define NUM_TESTS 6

class LightTest: public SCKTestBase {
public:
  LightTest(const char *name, bool can_be_zero) : SCKTestBase(name, can_be_zero) {};

  float read_sensor() {
    return ambient_.getLight();
  }
};

class NoiseTest: public SCKTestBase {
public:
  NoiseTest(const char *name, bool can_be_zero) : SCKTestBase(name, can_be_zero) {};

  float read_sensor() {
    return ambient_.getNoise();
  }
};

class COTest: public SCKTestBase {
public:
  COTest(const char *name, bool can_be_zero) : SCKTestBase(name, can_be_zero) {};

  float read_sensor() {
    ambient_.getMICS();
    return ambient_.getCO();
  }
};

class NO2Test: public SCKTestBase {
public:
  NO2Test(const char *name, bool can_be_zero) : SCKTestBase(name, can_be_zero) {};
  
  float read_sensor() {
    ambient_.getMICS();
    return ambient_.getNO2();
  }
};

class TemperatureTest: public SCKTestBase {
public:
  TemperatureTest(const char *name, bool can_be_zero) : SCKTestBase(name, can_be_zero) {};
  
  float read_sensor() {
    ambient_.getSHT21();
    return ambient_.getTemperature();
  }
};

class HumidityTest: public SCKTestBase {
public:
  HumidityTest(const char *name, bool can_be_zero) : SCKTestBase(name, can_be_zero) {};
  
  float read_sensor() {
    ambient_.getSHT21();
    return ambient_.getHumidity();
  }
};

//

class SCKTestSuite {
public:
  void run_all_tests();
private:
};

#endif
