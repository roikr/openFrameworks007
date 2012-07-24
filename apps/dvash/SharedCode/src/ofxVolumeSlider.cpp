//
//  ofxVolumeSlider.cpp
//  emptyExample
//
//  Created by Roee Kremer on 7/24/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxVolumeSlider.h"

#define TILE_SIZE 4
#define TILE_SPACE 1
#define BACKGROUND_COLOR 0x4b4b4b
#define FOREGROUND_COLOR 0x969696 
#define VALUE_COLOR 0x009600

ofxVolumeSlider::ofxVolumeSlider() {

    tileSize=TILE_SIZE;
    tileSpace=TILE_SPACE;
    backgroundColor=BACKGROUND_COLOR;
    foregroundColor=FOREGROUND_COLOR;
    valueColor=VALUE_COLOR;
    minValue = 0.0;
    maxValue=1.0;
    bInside = false;
        
}

void ofxVolumeSlider::setup(ofRectangle rect,float value) {
    this->rect = rect;
    this->value = value;
    
}

void ofxVolumeSlider::draw() {
    
    ofPushStyle();
    ofFill();
    
    int rows = rect.height/(tileSize+tileSpace);
    int cols = rect.width/(tileSize+tileSpace);
    float rectValue = value*rect.width;
    
    float y = 0;
    for (int i=0; i<rows; i++) {
        float x = 0;

        for (int j=0;j<cols;j++) {
            ofSetHexColor(y>=x*rect.height/rect.width ?  backgroundColor : (x<rectValue ? valueColor : foregroundColor));
            ofRect(rect.x+x,rect.y+(rect.height-y),tileSize,tileSize);
            x+=tileSize+tileSpace;
        }
        y+=tileSize+tileSpace;
    }
    ofPopStyle();
    
}

void ofxVolumeSlider::touchDown(ofTouchEventArgs &touch) {
    if (rect.inside(ofVec2f(touch.x,touch.y))) {
        bInside = true;
        value = (touch.x-rect.x)/rect.width;
        value = MIN(value,maxValue);
        value = MAX(value,minValue);
        
        
    }
}

void ofxVolumeSlider::touchMoved(ofTouchEventArgs &touch) {
    if (bInside) {
        value = (touch.x-rect.x)/rect.width;
        value = MIN(value,maxValue);
        value = MAX(value,minValue);

    }
}

void ofxVolumeSlider::touchUp(ofTouchEventArgs &touch) {
    if (bInside) {
        bInside = false;
        value = (touch.x-rect.x)/rect.width;
        value = MIN(value,maxValue);
        value = MAX(value,minValue);
        
        
    }
}
