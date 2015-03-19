/*
  TemperatureDecoupler.h
  SmartCitizen

  Created by Oriol Ferrer Mesià on 03/06/13.
*/

#ifndef SmartCitizen_TemperatureDecoupler_h
#define SmartCitizen_TemperatureDecoupler_h

#define BATTERY_HEATUP_MAX			11
#define BATTERY_CHARGE_THRESHOLD	980
#include <Arduino.h>
#include "AccumulatorFilter.h"


class TemperatureDecoupler{

  public:
  
	void setup(){
		_prevBattery = 0;
		filter.setup(0.3);
	}

	void update( uint16_t battery ){

		//SerialUSB.println( "# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #" );

		bool charging = false;
		bool doNothing = false;
		if (battery == _prevBattery){
			//doNothing;
			if ( battery > BATTERY_CHARGE_THRESHOLD ){
				charging = true;
				//SerialUSB.println( "Battery same val > 980! charging!");
			}else{
				charging = false;
				//SerialUSB.println( "Battery same val < 980! NOT charging!");
			}
		}else{
			if ( battery > _prevBattery || battery > BATTERY_CHARGE_THRESHOLD ){ //battery is charging!
				//SerialUSB.println( "Battery charging!");
				charging = true;
			}else{ //battery is being drained
				//SerialUSB.println( "Battery dis-charging!");
				charging = false;
			}
		}

		if (!doNothing){
			if(charging)
				filter.goUp();
			else
				filter.goDown();
		}

		//SerialUSB.print(F("battery: ")); SerialUSB.println( battery );
		//SerialUSB.print(F("_prevBattery: ")); SerialUSB.println( _prevBattery );
		//SerialUSB.print(F("filter: ")); SerialUSB.println( filter.getVal() );

		//SerialUSB.println( "# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #" );
		//store last bat reading for future comparing
		_prevBattery = battery;
		lastChargingState = charging;
	}

	short int getCompensation(){
		return (short int) (filter.getVal() * BATTERY_HEATUP_MAX);
	}

	short int _prevBattery;
	AccumulatorFilter filter;
	bool lastChargingState; //true == up, false == down

};


#endif
