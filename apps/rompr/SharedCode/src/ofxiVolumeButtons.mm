//
//  ofxiVolumeButtons.cpp
//  emptyExample
//
//  Created by Roee Kremer on 7/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxiVolumeButtons.h"
#import "RBVolumeButtons.h"

ofEvent<int> ofxiVolumeButtonEvent;

ofxiVolumeButtons::ofxiVolumeButtons():bEnabled(false) {
    AudioSessionInitialize(NULL, NULL, NULL, NULL);}


void ofxiVolumeButtons::start() {
    if (!bEnabled) {
        bEnabled = true;
        
        buttonStealer = [[RBVolumeButtons alloc] init];
        buttonStealer.upBlock = ^{ 
            int button = VOLUME_BUTTON_UP;
            ofNotifyEvent(ofxiVolumeButtonEvent, button);
        };
        buttonStealer.downBlock = ^{ 
            int button = VOLUME_BUTTON_DOWN;
            ofNotifyEvent(ofxiVolumeButtonEvent, button);
        };
    }
}

void ofxiVolumeButtons::stop() {
    if (bEnabled) {
        bEnabled = false;
        [buttonStealer dealloc];
        buttonStealer = nil;
    }
}

bool ofxiVolumeButtons::getIsEnabled() {
    return bEnabled;
}