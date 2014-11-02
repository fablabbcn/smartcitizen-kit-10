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

#define TEST_TIMEOUT 60000

class SCKTestBase {
private:
  char *name;
  bool can_be_zero;

public:
  SCKTestBase(const char *name, bool can_be_zero);

  bool execute(float &sensor_value);  
  char *get_name();
  virtual float read_sensor(); // Must be overriden by base classes
};

#endif

