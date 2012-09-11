//
//  ofxEraser.cpp
//  emptyExample
//
//  Created by Roee Kremer on 9/11/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxEraser.h"


void ofxEraser::setup(int size) {
    this->size = size;
    stroke.setup(7); // spacing for interpolation
}



void ofxEraser::clear() {
    
    curveSize = 0; // counter for stroke points
    stroke.clear(); 
}



void ofxEraser::touch(ofVec2f pos,ofFbo &fbo) {
    stroke.touch(pos);
    if (stroke.getCurve().size() > curveSize) {
        ofPushStyle();
        ofDisableAlphaBlending();
        ofSetColor(0,0,0,0);
                
        fbo.begin();
        
        
        
        
        ofFill();
        for (vector<ofVec2f>::iterator iter=stroke.getCurve().begin()+curveSize; iter!=stroke.getCurve().end(); iter++) {
            ofCircle(*iter, size);
        }
        
        fbo.end();
        
        curveSize = stroke.getCurve().size();
        ofEnableAlphaBlending();
        ofPopStyle();
    }
 
}