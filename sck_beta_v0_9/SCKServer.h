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



#ifndef __SCKSERVER_H__
#define __SCKSERVER_H__

#include <Arduino.h>

class SCKServer {
public:
   boolean time(char *time);
   void json_update(uint16_t updates, long *value, char *time, boolean isMultipart);
   void send(boolean sleep, boolean *wait_moment, long *value, char *time);
   boolean update(long *value, char *time_);
   boolean connect();
   void addFIFO(long *value, char *time);
   void readFIFO();
private:

};
#endif
