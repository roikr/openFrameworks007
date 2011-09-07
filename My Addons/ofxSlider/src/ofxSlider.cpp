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
	adjustPages();  // roikr: if pages in different size need to pass last page size
	setPage(0);
	state = SLIDER_STATE_IDLE;
	
}

void ofxSlider::adjustPages() {
	if (prefs.bCyclic) {
		//ofPoint lastSize = *(prefs.pages.end()-1)-*(prefs.pages.end()-2);
		prefs.pages.insert(prefs.pages.begin(),*(prefs.pages.begin())-prefs.lastPageSize);
		prefs.pages.push_back(*(prefs.pages.end()-1)+prefs.lastPageSize);
	}
}

void ofxSlider::fixCycle() {
	if (prefs.bCyclic) {
		if (currentPage == prefs.pages.begin()) {
			currentPage = prefs.pages.end()-2;
		} else if (currentPage == prefs.pages.end()-1) {
			currentPage = prefs.pages.begin()+1;
		}
	}
	setComponent(translate,-getComponent(*currentPage)*scale);
}

void ofxSlider::update() {
	
		
	if (state == SLIDER_STATE_ANIMATING) {
		float t = (float)(ofGetElapsedTimeMillis() - upTime)/(float)prefs.animDuration;
		if (t >= 1) {
			state = SLIDER_STATE_IDLE;
			fixCycle();
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
		fixCycle();
		
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
		
		if (prefs.bCyclic) {
			setComponent(translate,getComponent(translate)+  getComponent(touches.back().first-(touches.end()-2)->first));
		} else {
			setComponent(translate,getComponent(translate)+  boundsFix(getComponent(touches.back().first-(touches.end()-2)->first)));
		}
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
		
		if (prefs.bCyclic) {
			vector<ofPoint>::iterator previousPage,nextPage;
			nextPage = currentPage+1;
			if (nextPage==prefs.pages.end()) {
				nextPage=prefs.pages.begin();
			}
			
			previousPage = currentPage==prefs.pages.begin() ? prefs.pages.end()-1 : currentPage-1;
			
			if (fabs(vx)>0.5) {
				if (vx<0) {
					currentPage = nextPage;
				}
				else if (vx>0) { 
					currentPage = previousPage;
				}
				
			}  else if (getComponent((*(nextPage) - *currentPage)/2  +translate/scale + *currentPage) <0  ) {
				currentPage = nextPage;
			} else if (getComponent(( *currentPage - *previousPage)/2 +translate/scale+ *previousPage)>0 )  {
				currentPage = previousPage;
			}
			
			
		} else {
				
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
			
			
		}

		dest=-getComponent(*currentPage)*scale;
		
//		dest = -*currentPage*scale;
		
		
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
	int dist = distance(prefs.pages.begin(), currentPage);
	
	if (prefs.bCyclic) {
		if (currentPage == prefs.pages.begin()) {
			dist = prefs.pages.size()-3;
		} else if (currentPage == prefs.pages.end()-1 ) {
			dist = 0;
		} else {
			dist--;
		}
	} 

	return  dist;
}

void ofxSlider::setPage(int page) {
	currentPage = this->prefs.pages.begin()+page + (prefs.bCyclic ? 1 : 0);
	
	setComponent(translate, -getComponent(*currentPage)*scale);
}

void ofxSlider::next() {
	fixCycle();
	state = SLIDER_STATE_ANIMATING;
	currentPage++;
	if (!prefs.bCyclic) {
		if (currentPage==prefs.pages.end()) {
			currentPage=prefs.pages.begin();
		}
	}
	
	dest=-getComponent(*currentPage)*scale;
	upTime = ofGetElapsedTimeMillis();	

}