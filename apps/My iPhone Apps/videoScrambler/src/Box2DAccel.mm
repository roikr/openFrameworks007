//
//  Box2DAccel.mm
//  videoScrambler
//
//  Created by Roee Kremer on 2/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "Box2DAccel.h"
#include "testApp.h"

#define kAccelerometerFrequency 30

@implementation Box2DAccel

@synthesize app;

- (id)init {
    
    if (self = [super init]) {
        
		
		[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / kAccelerometerFrequency)];
		[[UIAccelerometer sharedAccelerometer] setDelegate:self];
    }
	
	
    return self;
}


- (void) accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration
{
	// Only run for valid values
	if (acceleration.y!=0 && acceleration.x!=0)
	{
		if (app) {
			app->setGravity(acceleration.x,acceleration.y);
		}
	}
}


@end
