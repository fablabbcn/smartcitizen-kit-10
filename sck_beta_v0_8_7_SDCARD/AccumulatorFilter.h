//
//  AccumulatorFilter.h
//  AccumulatorFilter
//
//  Created by Oriol Ferrer Mesià on 09/06/13.
//
//

#ifndef AccumulatorFilter_AccumulatorFilter_h
#define AccumulatorFilter_AccumulatorFilter_h

#define GHETTO_VAL	0.02f
class AccumulatorFilter{

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

	float upSpeed;
	float val;
};

#endif
