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

  AccumulatorFilter.h
  AccumulatorFilter
  Created by Oriol Ferrer Mesià on 09/06/13.
  Modified by Miguel Colom - http://mcolom.info

*/

#ifndef AccumulatorFilter_AccumulatorFilter_h
#define AccumulatorFilter_AccumulatorFilter_h

#define GHETTO_VAL	0.02f

class AccumulatorFilter {
private:
	float upSpeed;
	float val;

public:
	AccumulatorFilter(){
		val = GHETTO_VAL;
		upSpeed = 0.5f;
	}

	void setup(float upSpeed_){
		val = 0.0f;
		upSpeed = upSpeed_;
	}

	void goUp(){
		//Serial.println( "goUP!");
		if (val <= GHETTO_VAL){
			val = GHETTO_VAL;
		}
		val *= (1.0f + upSpeed * 2.0f * (1.0f - val) );
	}

	void goDown(){
		//Serial.println( "goDown!");
		if (val >= 1.0f - GHETTO_VAL){
			val = 1.0f - GHETTO_VAL;
		}
		val /= (1.0f + upSpeed * 2.0f * (1.0f - val) );
	}

	float getVal(){
		return val;
	}

	float getSpeed(){
		return upSpeed;
	}
};

#endif
