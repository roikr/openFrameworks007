//
//  ofxiVolumeButtons.h
//  emptyExample
//
//  Created by Roee Kremer on 7/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "ofEvents.h"
@class RBVolumeButtons;

enum {
    VOLUME_BUTTON_DOWN,
    VOLUME_BUTTON_UP
};

extern ofEvent<int> ofxiVolumeButtonEvent;

template<class T>
void ofxRegisterVolumeButtonsNotification(T * obj){
	ofAddListener(ofxiVolumeButtonEvent,obj,&T::volumeButtonPressed);
}

template<class T>
void ofxUnregisterVolumeButtonsNotification(T * obj){
	ofRemoveListener(ofxiVolumeButtonEvent,obj,&T::volumeButtonPressed);
}

class ofxiVolumeButtons {

public:
    ofxiVolumeButtons();
    void start();
    void stop();
    bool getIsEnabled();
    
private:
    bool bEnabled;
    
    RBVolumeButtons *buttonStealer;
    
    
};


