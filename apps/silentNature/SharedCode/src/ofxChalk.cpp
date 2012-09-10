//
//  ofxChalk.cpp
//  emptyExample
//
//  Created by Roee Kremer on 9/11/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxChalk.h"

void ofxChalk::setup(int width,int height) {
    image.loadImage("chalk_256.png");
	fbo.allocate(ofGetWidth(),ofGetHeight());
    strokeFbo.allocate(ofGetWidth(),ofGetHeight());
    
    fbo.begin();
    glClearColor(0,0,0, 0);
    glClear( GL_COLOR_BUFFER_BIT);
    fbo.end();
    
    strokeFbo.begin();
    glClearColor(0,0,0, 0);
    glClear( GL_COLOR_BUFFER_BIT);
    strokeFbo.end();
}

void ofxChalk::draw() {
    fbo.draw(0, 0);
}

void ofxChalk::setColor(ofColor color) {
    this->color = color;
}

void ofxChalk::clear() {
    curveSize = 0; // counter for stroke points
    step = 0; // couter for stamp 
    stroke.clear(7); // spacing for interpolation
    spacing = 10; // spacing for stamps
    size = 60; // brush texture size
}



void ofxChalk::touch(ofVec2f pos) {
    stroke.addTouch(pos);
    if (stroke.getCurve().size() > curveSize) {
        ofPushStyle();
        ofDisableAlphaBlending();
        ofSetColor(color);
        if (float(stroke.getCurve().size()/spacing) > step) {
            
            ofRectangle rect(0,0,size,size); // brush texture
            strokeFbo.begin();
            ofPushMatrix();
            ofTranslate(stroke.getCurve()[step*spacing]);
            ofRotate(rand(), 0, 0, 1.0);
            ofTranslate(-rect.getCenter());
            image.draw(rect);
            ofPopMatrix();
            strokeFbo.end();
            step++;
            
        }
        
        fbo.begin();
        glClearColor(0,0,0, 0);
        glClear( GL_COLOR_BUFFER_BIT);
        
        glClear(GL_STENCIL_BUFFER_BIT);  
        glEnable(GL_STENCIL_TEST); //Enable using the stencil buffer  
        glColorMask(0, 0, 0, 0); //Disable drawing colors to the screen  
        glStencilFunc(GL_ALWAYS, 1, 1); //Make the stencil test always pass  
        //Make pixels in the stencil buffer be set to 1 when the stencil test passes  
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);  
        //Set all of the pixels below to be 1 in the stencil buffer...  
        
        ofFill();
        for (vector<ofVec2f>::iterator iter=stroke.getCurve().begin(); iter!=stroke.getCurve().end(); iter++) {
            ofRectangle rect;
            rect.setFromCenter(*iter, size/2, size/6); // brush tip
            ofRect(rect);
        }
        
        glColorMask(1, 1, 1, 1); //Enable drawing colors to the screen  
        //Make the stencil test pass only when the pixel is 1 in the stencil buffer  
        glStencilFunc(GL_EQUAL, 1, 1);  
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); //Make the stencil buffer not change  
        //Draw all pixels where the stencil buffer is 1...  
        
        
        strokeFbo.draw(0, 0);
        
        glDisable(GL_STENCIL_TEST); //Disable using the stencil buffer  
        
        fbo.end();
        
        curveSize = stroke.getCurve().size();
        ofEnableAlphaBlending();
        ofPopStyle();
    }
 
}