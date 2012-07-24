//
//  ofxVolumeSlider.h
//  emptyExample
//
//  Created by Roee Kremer on 7/24/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"


class ofxVolumeSlider {
public:
    ofxVolumeSlider(); 
    void setup(ofRectangle rect, float value);
    void draw();
    
    void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
    
    
    ofRectangle rect;
    int tileSize;
    int tileSpace;
    int backgroundColor;
    int foregroundColor;
    int valueColor;
    float minValue;
    float maxValue;
    
    float value;
    bool bInside;
};
