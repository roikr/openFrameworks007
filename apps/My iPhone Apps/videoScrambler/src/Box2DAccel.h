//
//  Box2DAccel.h
//  videoScrambler
//
//  Created by Roee Kremer on 2/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

class testApp;

@interface Box2DAccel : NSObject<UIAccelerometerDelegate> {
	testApp *app;
}

@property testApp *app;

@end
