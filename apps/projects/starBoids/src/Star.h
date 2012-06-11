

#pragma once

#include "SteeredVehicle.h"
#include "ofMath.h"


class Star : public SteeredVehicle {

public:
    Star (float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) : SteeredVehicle(_x, _y, _z),bVisible(true),brightness(1.0f),bStarMode(true)
	{
		
	}
    
    
    
    bool bStarMode;
    bool bVisible;
    float brightness;

};


