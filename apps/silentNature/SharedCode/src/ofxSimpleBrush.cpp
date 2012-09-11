//
//  ofxSimpleBrush.cpp
//  emptyExample
//
//  Created by Roee Kremer on 9/11/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxSimpleBrush.h"

void ofxSimpleBrush::setup(ofImage tip,int tipSize,int width,int height) {
    
    this->tip = tip;
    this->tipScale = (float)tipSize/tip.getWidth();
    
   
	fbo.allocate(ofGetWidth(),ofGetHeight());
    
    fbo.begin();
    
    ofClear(0, 0, 0,0);
    fbo.end();
    stroke.setup(1);
    
}

void ofxSimpleBrush::draw() {
    fbo.draw(0, 0);
}

void ofxSimpleBrush::setColor(ofColor color) {
    this->color = color;
}

void ofxSimpleBrush::clear() {
    curveSize = 0; // counter for stroke points
    stroke.clear(); // spacing for interpolation
    fbo.begin();
    
    ofClear(0, 0, 0,0);
    fbo.end();   
    
    ofClear(0, 0, 0,0);
    

}



void ofxSimpleBrush::touch(ofVec2f pos) {

    stroke.touch(pos);
    if (stroke.getCurve().size() > curveSize) {
        ofPushStyle();
        
        ofSetColor(color);
        
        fbo.begin();
        
        for (int i=curveSize;i<stroke.getCurve().size();i++) {
            ofPushMatrix();
            ofTranslate(stroke.getCurve()[i]);
            float scale = ofClamp(stroke.getVelocities()[i].length()/20,0.5,20);
            ofScale(tipScale/scale, tipScale/scale);
            
            ofTranslate(-0.5*ofVec2f(tip.getWidth(),tip.getHeight()));
            tip.draw(0,0);
            ofPopMatrix();
        }
        
        fbo.end();
        
        curveSize = stroke.getCurve().size();
        
        ofPopStyle();
    }
 
}