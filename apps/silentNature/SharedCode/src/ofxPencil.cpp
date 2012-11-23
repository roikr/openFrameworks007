//
//  ofxPencil.cpp
//  silentNature
//
//  Created by Roee Kremer on 11/23/12.
//
//

#include "ofxPencil.h"


void ofxPencil::setup(ofImage tip,int tipSize,int width,int height) {
    
    this->tip = tip;
    this->tipScale = (float)tipSize/tip.getWidth();
    
    
	fbo.allocate(ofGetWidth(),ofGetHeight());
    
    fbo.begin();
    
    ofClear(0, 0, 0,0);
    fbo.end();
    stroke.setup(1);
    
}

void ofxPencil::draw() {
    fbo.draw(0, 0);
}

void ofxPencil::setColor(ofColor color) {
    this->color = color;
}

void ofxPencil::clear() {
    curveSize = 0; // counter for stroke points
    stroke.clear(); // spacing for interpolation
    fbo.begin();
    
    ofClear(0, 0, 0,0);
    fbo.end();
    
    ofClear(0, 0, 0,0);
    
    
}



void ofxPencil::touch(ofVec2f pos) {
    
    stroke.touch(pos);
    if (stroke.getCurve().size() > curveSize) {
        ofPushStyle();
        
        ofSetColor(color);
        
        fbo.begin();
        
        for (int i=curveSize;i<stroke.getCurve().size();i++) {
            ofPushMatrix();
            ofTranslate(stroke.getCurve()[i]);
            float scale = 1;//ofClamp(stroke.getVelocities()[i].length()/20,0.5,20);
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