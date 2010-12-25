/*
 *  ofxSlider.cpp
 *  iPhoneSliderExample
 *
 *  Created by Roee Kremer on 12/24/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxSlider.h"
#include "easing.h"
#include <math.h>


void ofxSlider::setup(float scale,sliderPrefs prefs) {
	this->scale = scale;
	translate = ofPoint(0,0);
	this->prefs = prefs;
	bAnimating =false;
	currentPage = this->prefs.pages.begin();
	
}



void ofxSlider::update() {
	
		
	if (bAnimating) {
		float t = (float)(ofGetElapsedTimeMillis() - upTime)/5000.0f;
		if (t >= 1) 
			bAnimating = false;
		else 
			translate.x = easeOutBack(t,translate.x,dest.x);
	}	
		
}



void ofxSlider::transform() {
	ofTranslate(translate.x, translate.y, 0);
	ofScale(scale, scale, 1.0);
}

//#ifdef OF_DEBUG 

void ofxSlider::draw() {
	
	if (touches.size()) {
		ofSetColor(255,130,0);
		ofNoFill();		// draw "filled shapes"
		
		ofPoint pnt=(touches.back().first-translate)/scale;
		ofCircle(pnt.x,pnt.y,50);
	}
	
		
}

//#endif


void ofxSlider::touchDown(int x, int y, int id) {
	
	
	if (id==0) {
		
		bAnimating = false;
		
		touches.push_back(make_pair(ofPoint(x,y), ofGetElapsedTimeMillis()));
		//printf("touchDown: %i %.0f\n",touches.back().second,touches.back().first.x);
	}
		
}





void ofxSlider::touchMoved(int x, int y, int id) {
	
	
	if (id==0) {
		//printf("touchMoved: %i %.0f\n",touches.back().second,touches.back().first.x);
		touches.push_back(make_pair(ofPoint(x,y), ofGetElapsedTimeMillis()));
		
		float dx = touches.back().first.x-(touches.end()-2)->first.x;
		if ( (currentPage == prefs.pages.end()-1 && dx < 0) || (currentPage == prefs.pages.begin() && dx > 0)) {
			dx=dx/2;
		}
		
		translate.x += dx/scale;
	}
			
}

void ofxSlider::touchUp(int x, int y, int id) {
	if (bAnimating)
		return;
	
	if (id==0 && touches.size()>1) {
		//printf("touchUp: %i %.0f\n",touches.back().second,touches.back().first.x);
		
		
		
	//	touches.push_back(make_pair(ofPoint(x,y), ofGetElapsedTimeMillis()));
//		float dx = touches.back().first.x-(touches.end()-2)->first.x;
//		if ( (currentPage == prefs.numPages-1 && dx < 0) || (currentPage == 0 && dx > 0)) {
//			dx=dx/2;
//		} 
//		
//		translate.x += dx/scale;
		
		bAnimating = true;
		
		
		upTime = ofGetElapsedTimeMillis();	
		
		float dx = touches.back().first.x-(touches.end()-2)->first.x;
		float dt = touches.back().second-(touches.end()-2)->second;
		float vx = dx/dt;
		
//		printf("velocity: %.2f\n",vx);
		
		if (fabs(vx)>0.5) {
			if (currentPage < prefs.pages.end()-1  && vx<0) 
				currentPage++;
			else if (currentPage > prefs.pages.begin() && vx>0) 
				currentPage--;
			
		}  else if (currentPage < prefs.pages.end()-1 &&  ((currentPage+1)->x - currentPage->x)/2 < -translate.x - currentPage->x  ) {
			currentPage++;
		} else if (currentPage > prefs.pages.begin() &&  (  currentPage->x - (currentPage-1)->x)/2 >  -translate.x - (currentPage-1)->x)  {
			currentPage--;
		}

		
		
		dest = -*currentPage;
		
		touches.clear();
	}
}

void ofxSlider::touchDoubleTap(int x, int y, int id) {
	
}

bool ofxSlider::getIsAnimating() {
	return bAnimating;
}
