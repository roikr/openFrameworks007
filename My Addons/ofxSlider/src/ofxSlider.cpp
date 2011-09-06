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

enum  {
	SLIDER_STATE_IDLE,
	SLIDER_STATE_DRAGGING,
	SLIDER_STATE_ANIMATING
};


void ofxSlider::setup(float scale,sliderPrefs prefs) {
	this->scale = scale;
//	this->translate = translate;
	this->prefs = prefs;
	setPage(0);
	state = SLIDER_STATE_IDLE;
	
}



void ofxSlider::update() {
	
		
	if (state == SLIDER_STATE_ANIMATING) {
		float t = (float)(ofGetElapsedTimeMillis() - upTime)/(float)prefs.animDuration;
		if (t >= 1) {
			state = SLIDER_STATE_IDLE;
			setComponent(translate,dest);
		}
		else 
			setComponent(translate, easeOutBack(t,getComponent(translate),dest));
	}	
		
}



void ofxSlider::transform() {
	ofTranslate(translate.x, translate.y, 0);
	ofScale(scale, scale, 1.0);
	
	
	
	
}



void ofxSlider::draw() {
	
	if (touches.size()) {
		ofSetColor(255,130,0);
		ofNoFill();		// draw "filled shapes"
		
		ofPoint pnt=(touches.back().first-translate)/scale;
		ofRect(pnt.x-25,pnt.y-25,50,50);
	}
	
		
}




void ofxSlider::touchDown(int x, int y, int id) {
	
	
	if (id==0) {
		state = SLIDER_STATE_IDLE;
		
		
		touches.push_back(make_pair(ofPoint(x,y), ofGetElapsedTimeMillis()));
		//printf("touchDown: %i %.0f\n",touches.back().second,touches.back().first.x);
	}
		
}


float ofxSlider::boundsFix(float dx)  {
	return (currentPage == prefs.pages.end()-1 && dx < 0) || (currentPage == prefs.pages.begin() && dx > 0) ? dx/2 : dx;
}

float ofxSlider::getComponent(ofPoint pnt) {
	return prefs.direction == SLIDER_HORIZONTAL ? pnt.x : pnt.y;
	
}

void ofxSlider::setComponent(ofPoint &pnt,float x) {
	
	switch (prefs.direction) {
		case SLIDER_HORIZONTAL:
			pnt.x = x;
			break;
		case SLIDER_VERTICAL:
			pnt.y = x;
			break;
		default:
			break;
	}
	
	
}


void ofxSlider::touchMoved(int x, int y, int id) {
	
	
	if (id==0) {
		state = SLIDER_STATE_DRAGGING;
		//printf("touchMoved: %i %.0f\n",touches.back().second,touches.back().first.x);
		touches.push_back(make_pair(ofPoint(x,y), ofGetElapsedTimeMillis()));
		setComponent(translate,getComponent(translate)+boundsFix(getComponent(touches.back().first-(touches.end()-2)->first)));
	}
			
}

void ofxSlider::touchUp(int x, int y, int id) {
	
	
	if (id==0 && touches.size()>1) {
		//printf("touchUp: %i %.0f\n",touches.back().second,touches.back().first.x);
		
		
		
	//	touches.push_back(make_pair(ofPoint(x,y), ofGetElapsedTimeMillis()));
//		float dx = touches.back().first.x-(touches.end()-2)->first.x;
//		if ( (currentPage == prefs.numPages-1 && dx < 0) || (currentPage == 0 && dx > 0)) {
//			dx=dx/2;
//		} 
//		
//		translate.x += dx/scale;
		
		state = SLIDER_STATE_ANIMATING;
		
		
		upTime = ofGetElapsedTimeMillis();	
		
		float dx = getComponent(touches.back().first-(touches.end()-2)->first);
		float dt = touches.back().second-(touches.end()-2)->second;
		float vx = dx/dt;
		
//		printf("velocity: %.2f\n",vx);
		
		if (fabs(vx)>0.5) {
			if (currentPage < prefs.pages.end()-1  && vx<0) 
				currentPage++;
			else if (currentPage > prefs.pages.begin() && vx>0) 
				currentPage--;
			
		}  else if (currentPage < prefs.pages.end()-1 &&  getComponent((*(currentPage+1) - *currentPage)/2  +translate/scale + *currentPage) <0  ) {
			currentPage++;
		} else if (currentPage > prefs.pages.begin() &&  getComponent(( *currentPage - *(currentPage-1))/2 +translate/scale+ *(currentPage-1))>0 )  {
			currentPage--;
		}

		
		
//		dest = -*currentPage*scale;
		dest=-getComponent(*currentPage)*scale;
		
		touches.clear();
	}
}

void ofxSlider::touchDoubleTap(int x, int y, int id) {
	
}

bool ofxSlider::getIsAnimating() {
	return state == SLIDER_STATE_ANIMATING;
}

bool ofxSlider::getIsDragging() {
	return state == SLIDER_STATE_DRAGGING;
}

int	ofxSlider::getCurrentPage() {
	return distance(prefs.pages.begin(), currentPage);
}

void ofxSlider::setPage(int page) {
	currentPage = this->prefs.pages.begin()+page;
	
	setComponent(translate, -getComponent(*currentPage)*scale);
}

void ofxSlider::next() {
	state = SLIDER_STATE_ANIMATING;
	currentPage++;
	if (currentPage==prefs.pages.end()) {
		currentPage=prefs.pages.begin();
	}
	dest=-getComponent(*currentPage)*scale;
	upTime = ofGetElapsedTimeMillis();	

}