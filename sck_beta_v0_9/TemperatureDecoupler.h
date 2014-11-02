/*
 *
 * This file is part of the SCK v0.9 - SmartCitizen
 *
 * Oriol Ferrer Mesià
 * Modified by Miguel Colom - http://mcolom.info
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

#ifndef SmartCitizen_TemperatureDecoupler_h
#define SmartCitizen_TemperatureDecoupler_h

#define BATTERY_HEATUP_MAX			11
#define BATTERY_CHARGE_THRESHOLD	980
#include <Arduino.h>
#include "AccumulatorFilter.h"


class TemperatureDecoupler{

  public:

    /**
     * @brief Initialization of the battery tracker
     *
     **/  
	void setup(){
		_prevBattery = 0;
		filter.setup(0.3);
	}

    /**
     * @brief Updates the battery status and charge level
     * 
     * @param battery : battery level
     **/  
	void update( uint16_t battery ){

		//Serial.println( "# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #" );

		bool charging = false;
		bool doNothing = false;
		if (battery == _prevBattery){
			//doNothing;
			if ( battery > BATTERY_CHARGE_THRESHOLD ){
				charging = true;
				//Serial.println( "Battery same val > 980! charging!");
			}else{
				charging = false;
				//Serial.println( "Battery same val < 980! NOT charging!");
			}
		}else{
			if ( battery > _prevBattery || battery > BATTERY_CHARGE_THRESHOLD ){ //battery is charging!
				//Serial.println( "Battery charging!");
				charging = true;
			}else{ //battery is being drained
				//Serial.println( "Battery dis-charging!");
				charging = false;
			}
		}

		if (!doNothing){
			if(charging)
				filter.goUp();
			else
				filter.goDown();
		}

		//Serial.print(F("battery: ")); Serial.println( battery );
		//Serial.print(F("_prevBattery: ")); Serial.println( _prevBattery );
		//Serial.print(F("filter: ")); Serial.println( filter.getVal() );

		//Serial.println( "# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #" );
		//store last bat reading for future comparing
		_prevBattery = battery;
		lastChargingState = charging;
	}

    /**
     * @brief Gets the compensation value to should be subtracted from the temperature sensor
     * 
     **/  
	short int getCompensation(){
		return (short int) (filter.getVal() * BATTERY_HEATUP_MAX);
	}

	short int _prevBattery;
	AccumulatorFilter filter;
	bool lastChargingState; //true == up, false == down

};


#endif
