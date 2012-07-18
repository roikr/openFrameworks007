//
//  ofxFloatingCaption.cpp
//  emptyExample
//
//  Created by Roee Kremer on 7/18/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxFloatingCaption.h"

void ofxFloatingCaption::setup(ofRectangle rect,float margin,float radius) {
    this->rect = rect;
    this->margin = margin;
    this->radius = radius;
    
    dir = ofVec2f(0,-1);
    
}

void ofxFloatingCaption::setAnchor(ofVec2f anchor) {
    ofVec2f center(rect.getCenter());
    ofVec2f d = anchor-center;
    
    ofVec2f diff = anchor-this->anchor;
    this->anchor = anchor;
  
    
    float r=d.length()/radius;
    float inside = (1-r)*dir.angle(-diff)*0.01;
    float outside = dir.angle(-d)*0.1;
    float a=ofMap(r, 0.8, 1.2, 0, 1,true);
    dir.rotate(a*outside+(1-a)*inside);
    
    float angle = -dir.angle(ofVec2f(1,0))*PI/180;
    pos=ofVec2f((rect.width/2-margin)*cos(angle),(rect.height/2-margin)*sin(angle))+center;
    
}

ofVec2f ofxFloatingCaption::getPos() {
    return pos;
}

ofVec2f ofxFloatingCaption::getAnchor() {
    return anchor;
}