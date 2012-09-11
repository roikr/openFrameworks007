//
//  ofxBrush.cpp
//  emptyExample
//
//  Created by Roee Kremer on 9/11/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxBrush.h"

void ofxBrush::setup(ofImage tip,int tipSize,ofImage texture,int width,int height) {
    this->texture = texture;
    this->tip = tip;
    this->tipScale = (float)tipSize/tip.getWidth();
    if (texture.isAllocated()) {
        this->textureScale = (float)tipSize*3.0/texture.getWidth();
    }
   
	fbo.allocate(ofGetWidth(),ofGetHeight());
    strokeFbo.allocate(ofGetWidth(),ofGetHeight());
    
    fbo.begin();
    ofClear(0, 0, 0,0);
    fbo.end();
    
    stroke.setup(3);
    spacing = 5; // spacing for stamps
    
}

void ofxBrush::draw() {
    fbo.draw(0, 0);
}

void ofxBrush::setColor(ofColor color) {
    this->color = color;
}

void ofxBrush::clear() {
    curveSize = 0; // counter for stroke points
    step = 0; // couter for stamp 
    stroke.clear(); // spacing for interpolation
    
    
    fbo.begin();
    ofClear(0, 0, 0,0);
    fbo.end();
    
    strokeFbo.begin();
    ofClear(0, 0, 0,0);
    strokeFbo.end();

}



void ofxBrush::touch(ofVec2f pos) {
    stroke.touch(pos);
    if (stroke.getCurve().size()>curveSize) {
    
        ofPushStyle();
        
        
        ofDisableAlphaBlending();
        ofSetColor(color);
    
        if (floor(stroke.getCurve().size()/spacing) > step) {
            strokeFbo.begin();
            ofPushMatrix();
            ofTranslate(stroke.getCurve().back());
            ofScale(textureScale, textureScale);
            ofRotate(rand(), 0, 0, 1.0);
            ofTranslate(-0.5*ofVec2f(texture.getWidth(),texture.getHeight()));
            texture.draw(0,0);
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
       
        for (vector<ofVec2f>::iterator iter=stroke.getCurve().begin(); iter!=stroke.getCurve().end(); iter++) {
            ofPushMatrix();
            ofTranslate(*iter);
            ofScale(tipScale, tipScale);
            ofTranslate(-0.5*ofVec2f(tip.getWidth(),tip.getHeight()));
            tip.draw(0,0);
            ofPopMatrix();
        }
        
        glColorMask(1, 1, 1, 1); //Enable drawing colors to the screen  
        //Make the stencil test pass only when the pixel is 1 in the stencil buffer  
        glStencilFunc(GL_EQUAL, 1, 1);  
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); //Make the stencil buffer not change  
        //Draw all pixels where the stencil buffer is 1...  
        
        
        
        strokeFbo.draw(0, 0);
        
        glDisable(GL_STENCIL_TEST); //Disable using the stencil buffer  
        
        fbo.end();
        
//        curveSize = stroke.getCurve().size();
        ofEnableAlphaBlending();
        ofPopStyle();
        curveSize = stroke.getCurve().size();
    }
 
}