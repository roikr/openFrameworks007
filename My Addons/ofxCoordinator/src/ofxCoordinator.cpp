/*
 *  ofxCoordinator.cpp
 *  physicsExample
 *
 *  Created by Roee Kremer on 2/6/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxCoordinator.h"

void ofxCoordinator::setup(int width,int height,ofPoint translation,float scale) {
	this->width = width;
	this->height = height;
	this->translation = translation;
	this->scale.x = scale;
	this->scale.y = scale *  -1.0f;
}

void ofxCoordinator::pushTransform() {
	ofPushMatrix();
	ofTranslate(translation.x, translation.y, 0.0f);
	ofScale(scale.x, scale.y, 1.0f);
	
	
}


void ofxCoordinator::popTransform() {
	ofPopMatrix();
}


ofPoint ofxCoordinator::screenSpaceToWorldSpace(ofPoint pos) {
	return (pos-translation)/scale;
}
