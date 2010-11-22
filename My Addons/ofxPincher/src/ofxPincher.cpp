/*
 *  ofxPincher.cpp
 *  iPhonePinchZoomExample
 *
 *  Created by Roee Kremer on 11/16/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxPincher.h"
#include <math.h>

void ofxPincher::setup(ofPoint translate, float scale,pincherPrefs prefs) {
	numActive = 0;
	this->translate = translate;
	this->scale = scale;
	this->prefs = prefs;
	
}

void ofxPincher::transform() {
	ofTranslate(translate.x, translate.y, 0);
	ofScale(scale, scale, 1.0);
}

#ifdef OF_DEBUG 

void ofxPincher::draw() {
	
	if (prefs.width ){
		ofSetColor(255, 130, 0);
		ofNoFill();
		ofRect(prefs.boundaryBox.x + 10, prefs.boundaryBox.y + 10, prefs.boundaryBox.width - 20, prefs.boundaryBox.height - 20);
		
		
	}
	
	for (int i=0; i<2; i++) {
		if (active[i]) {
			ofSetColor(255,130,0);
			ofNoFill();		// draw "filled shapes"
			
			ofPoint pnt=(pos[i]-translate)/scale;
			ofCircle(pnt.x,pnt.y,50);
		}
		
		if (numActive==2) {
			ofSetColor(255,130,0);
			ofFill();		// draw "filled shapes"
			
			ofPoint center = (pos[0]+pos[1])/2;
			ofPoint pnt=(center-translate)/scale;
			
			ofCircle(pnt.x,pnt.y,30);
			
		}
		
	}
}

#endif
 

void ofxPincher::touchDown(int x, int y, int id) {
	
	if (id<2) {
		
		
		numActive++;
		active[id] = true;
		pos[id]=ofPoint(x,y);
		
		
		
	}
	
	
}

float ofxPincher::distance(ofPoint pnt) {
	ofPoint norm = pnt*pnt;
	return sqrt(norm.x+norm.y+norm.z);
}

void ofxPincher::touchMoved(int x, int y, int id) {
	
	if (id<2) {
		
		ofPoint tempPos=ofPoint(x,y);
		
		ofPoint trns;
		float scl;
		
		switch (numActive) {
			case 1:
				trns = translate + tempPos-pos[id];
				scl = scale;
				pos[id]=tempPos;
				
				break;
			case 2: {
				
				
				
				
				float factor=distance(tempPos-pos[1-id])/distance(pos[id]-pos[1-id]);
				
				if( scale * factor < prefs.minScale) {
					factor = prefs.minScale / scale;
				}
				
				if( scale * factor > prefs.maxScale) {
					factor = prefs.maxScale / scale;
				}
				
				
				ofPoint center = (pos[0]+pos[1])/2;
				
				trns=center*(1-factor) + translate * factor + (tempPos-pos[id])/2;
				scl= scale * factor;
				
				pos[id]=tempPos;
				
			}	break;
				
			default:
				break;
		}
		
		if (prefs.width ) {
				
			
			if (-trns.x < prefs.boundaryBox.x) {
				trns.x = - prefs.boundaryBox.x;
			} else if (-trns.x>prefs.boundaryBox.x + prefs.boundaryBox.width * scl -prefs.width) {
				trns.x= -(prefs.boundaryBox.x + prefs.boundaryBox.width * scl -prefs.width);
			}
			
			if (-trns.y < prefs.boundaryBox.y) {
				trns.y = - prefs.boundaryBox.y;
			} else if (-trns.y>prefs.boundaryBox.y + prefs.boundaryBox.height * scl -prefs.height) {
				trns.y= -(prefs.boundaryBox.y + prefs.boundaryBox.height * scl -prefs.height);
			}
			
		}
		 
		
		translate = trns;
		scale = scl;

	}
	
	
	
	
	
}

void ofxPincher::touchUp(int x, int y, int id) {
	
	if (id<2) {
		numActive--;
		pos[id]=ofPoint(x,y);
	}	active[id] = false;
	
}
