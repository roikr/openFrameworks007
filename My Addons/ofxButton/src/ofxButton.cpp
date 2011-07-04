/*
 *  ofxButton.cpp
 *  Musicreed
 *
 *  Created by Roee Kremer on 7/4/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxButton.h"

ofxButton::ofxButton(ofRectangle rect,ofColor color) {
	this->rect = rect;
	this->color = color;
	bDown = false;
}

void ofxButton::draw() {
	ofFill();
	ofSetColor(color.r, color.g, color.b);
	ofRect(rect.x,rect.y,rect.width,rect.height);
	
	ofEnableAlphaBlending();
	
	
	if (!bDown) {
		ofSetColor(255, 255, 255,150);
		ofRect(rect.x, rect.y, rect.x+rect.width, 1);
		ofSetColor(0, 0, 0,50);
		ofRect(rect.x, rect.y+rect.height-2, rect.x+rect.width, 2);
		
	} else {
		ofSetColor(0, 0, 0,50);
		ofRect(rect.x, rect.y, rect.x+rect.width, 2);
		ofSetColor(255, 255, 255,150);
		ofRect(rect.x, rect.y+rect.height-1, rect.x+rect.width, 1);
	}

	ofDisableAlphaBlending();
}

void ofxButton::setDown(bool bDown){
	this->bDown = bDown;
}