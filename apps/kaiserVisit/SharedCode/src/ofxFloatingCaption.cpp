//
//  ofxFloatingCaption.cpp
//  emptyExample
//
//  Created by Roee Kremer on 7/18/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxFloatingCaption.h"

void ofxFloatingCaption::setup(ofRectangle screenRect,ofRectangle captionRect,float radius,ofVec2f anchor) {
    this->screenRect = screenRect;
    this->captionRect = captionRect;
    this->radius = radius;
    
   
    
    this->anchor = anchor;
    dir = (screenRect.getCenter()-anchor).normalized(); // this vector point from anchor to center
        
//     dir = ofVec2f(0,-1);
    
}

void ofxFloatingCaption::draw() {
    ofPushStyle();
    ofSetColor(184,41,35,fade*255);
    ofSetLineWidth(2);
    ofLine(linep1,linep2);
    
    ofPopStyle();
}

void ofxFloatingCaption::setAnchor(ofVec2f anchor) {
    ofVec2f center(screenRect.getCenter());
    ofVec2f d = anchor-center;
    
    ofVec2f diff = anchor-this->anchor;
    this->anchor = anchor;
  
    
    float r=d.length()/radius;
    float inside = (1-r)*dir.angle(-diff)*0.01;
    float outside = dir.angle(-d)*0.1;
    float a=ofMap(r, 0.8, 1.2, 0, 1,true);
    dir.rotate(a*outside+(1-a)*inside);
    
    
    
    float angle = -dir.angle(ofVec2f(1,0))*PI/180;
    pos=ofVec2f(0.5*(screenRect.width-captionRect.width)*cos(angle),0.5*(screenRect.height-captionRect.height)*sin(angle))+center;
    
    ofVec2f vec = getAnchor()-getPos();
    float length = vec.length();
    
    vec.normalize();
    float va = vec.angle(ofVec2f(1,0))*PI/180;
    float captionLength = MIN(captionRect.width/(2.0*abs(cos(va))),captionRect.height/(2.0*abs(sin(va))));
    
    
    linep1 = getPos()+vec*(captionLength+20);
    linep2 = getPos()+vec*(length-13.5-20);
    fade = 1;
    
    float aa = d.angle(ofVec2f(1,0)) * PI/180; // anchor to orbit angle
    ofVec2f apos=ofVec2f(0.5*(screenRect.width-captionRect.width)*cos(aa),0.5*(screenRect.height-captionRect.height)*sin(aa));
    float minPos = apos.length();
    float maxPos =  MIN(0.5*screenRect.width/abs(cos(aa)),0.5*screenRect.height/abs(sin(aa)));
//     cout << aa*180/PI << "\t" << d.length() << "\t" << minPos << "\t" << maxPos << endl;
    fade = ofMap(d.length(), minPos, maxPos, 1, 0,true);
    
}

ofVec2f ofxFloatingCaption::getPos() {
    return pos;
}

ofVec2f ofxFloatingCaption::getAnchor() {
    return anchor;
}

//float ofxFloatingCaption::getLegnth() {
//    return length;
//}

float ofxFloatingCaption::getFade() {
    return fade;
    
}

/*
ofRectangle ofxFloatingCaption::getBoundingBox() {
    return rect;
}
*/