/*
 *  ofxLine
 *  iPhoneXonix
 *
 *  Created by Roee Kremer on 3/9/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxLine.h"
#include <assert.h>

ofxLine::ofxLine(ofPoint &p1,ofPoint &p2) {
		
	this->p1 = p1;
	this->p2 = p2;
	

	float d = p1.x*p2.y - p2.x*p1.y;
	if (d == 0) {
		c = 0;
		
		if (p1.y != 0) {
			a = 1;
			b = -p1.x/p1.y;
		} else if (p1.x != 0) {
			a = 0;
			b = 1;
		} else if (p2.y != 0) {
			a = 1;
			b = -p2.x/p2.y;
		} else if (p2.x != 0) {
			a = 0;
			b = 1;
		} else {
			assert("ofxLine error");
		}

	} else {
		c = 1;
		b = (p1.x-p2.x)/d;
		a = (p1.y-p2.y)/-d;
	}

}

bool ofxLine::getInterception(ofxLine &l,ofPoint &p) {
	float d = a*l.b-b*l.a;
	if (d == 0) {
		//printf("(%.1f,%.1f)-(%.1f,%.1f) (%.1f,%.1f)-(%.1f,%.1f)\n",l.p1.x,l.p1.y,l.p2.x,l.p2.y,p1.x,p1.y,p2.x,p2.y);
		return false;
	} 
	
	p.y = (a-l.a)/d;
	p.x = (b-l.b)/-d;
	
	//cout << "x: " << p.x << ", y: " << p.y << endl;
	
	return p.x>=min(p1.x,p2.x) && p.x <= max(p1.x,p2.x) && p.y>=min(p1.y,p2.y) && p.y <= max(p1.y,p2.y) && 
	p.x>=min(l.p1.x,l.p2.x) && p.x <= max(l.p1.x,l.p2.x) && p.y>=min(l.p1.y,l.p2.y) && p.y <= max(l.p1.y,l.p2.y);
}
	

void ofxLine::draw() {
	ofLine(p1.x,p1.y,p2.x,p2.y);
}


void ofxLine::dump() {
	cout << "a: " << a << ", b: " << b << ", c: " << c <<endl;
}

void ofxLine::getCrossBorderPoints(ofPoint &p1,ofPoint &p2) {
	ofPoint p = (this->p1+this->p2)/2;
	ofPoint dist = fabs(b)>fabs(a) ? ofPoint(0,2) : ofPoint(2,0);
	
	p1=p-dist;
	p2=p+dist;
		
}
