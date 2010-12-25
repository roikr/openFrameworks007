/*
 *  easing.cpp
 *  iPhoneSliderExample
 *
 *  Created by Roee Kremer on 12/24/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "easing.h"

float easeInOutQuad(float t, float b, float e) { 
	float d = 1.0;
	float c = e - b;
	if ((t/=d/2) < 1) 
		return c/2*t*t + b; 
	return -c/2 * ((--t)*(t-2) - 1) + b;
}

float easeOutBounce(float t, float b, float e) {
	// function (t, b, c, d) 
	float c = e - b;
	float d = 1.0;
    
	if ((t/=d) < (1/2.75)) {
		return c*(7.5625*t*t) + b;
	} else if (t < (2/2.75)) {
		return c*(7.5625*(t-=(1.5/2.75))*t + .75) + b;
	} else if (t < (2.5/2.75)) {
		return c*(7.5625*(t-=(2.25/2.75))*t + .9375) + b;
	} else {
		return c*(7.5625*(t-=(2.625/2.75))*t + .984375) + b;
	}
}

float easeOutBack (float t, float b, float e) {
	float c = e - b;
	float d = 1.0;
	float s = 1.70158;
	return c*((t=t/d-1)*t*((s+1)*t + s) + 1) + b;
}

float interp (float a,float x,float y) {
	return (1-a)*x+a*y;
}
