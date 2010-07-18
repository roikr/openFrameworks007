/*
 *  ofxCornerDetector.cpp
 *  ArcLengthExample
 *
 *  Created by Roee Kremer on 6/22/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxCornerDetector.h"


struct side {
	vector<ofPoint>::iterator iter;
	
	float norm;
	float dist;
};




void ofxCornerDetector::setup() {
	curve.clear();
	corners.clear();
}

void ofxCornerDetector::update() {
	for (vector<ofPoint>::iterator iter = curve.begin();  iter!=curve.end(); iter++) {
		checkCandidate(iter);
	}
	
	cout <<"candidates:" << endl;
	//printf("candidates:\n");
	for (vector<candidate>::iterator iter = candidates.begin(); iter!=candidates.end() ; iter++) {
		printf("\tx:%3.0f y:%3.0f alpha:%3.0f sharpness:%3.0f\n",iter->iter->x,iter->iter->y,iter->alpha,iter->sharpness);
	}
	
}

void ofxCornerDetector::addPoint(ofPoint &point) {
	curve.push_back(point);
}

vector<corner> &ofxCornerDetector::getCorners() {
	 
	
	
}




void ofxCornerDetector::checkCandidate(vector<ofPoint>::iterator iter) {
	vector<side> pplus,pminus;
	
	/*
	int pos = distance(curve.begin(), iter);
	
	for (int i=pos;i>=0;i--) {
		ofPoint p = curve[i];
		float dist = ofDist(p.x, p.y, iter->x, iter->y);
		if (dist > dmax)
			break;
		else if (dist>dmin)
			pminus.push_back(p);
	} */
	
		
	
	vector<ofPoint>::iterator piter = iter;
	
	for (piter = iter;  piter!=curve.begin(); piter--) {
		side c;
		c.iter = piter;
		ofPoint vec = *(c.iter) - (*iter);
		c.norm = vec.x*vec.x+vec.y*vec.y;
		c.dist = sqrt(c.norm);
		if (c.dist > MAX_DIST)
			break;
		else if (c.dist>MIN_DIST)
			pminus.push_back(c);
		
	}
	
	
	for (piter = iter;  piter!=curve.end(); piter++) {
		side c;
		c.iter = piter;
		ofPoint vec = *(c.iter) - (*iter);
		c.norm = vec.x*vec.x+vec.y*vec.y;
		c.dist = sqrt(c.norm);
		if (c.dist > MAX_DIST)
			break;
		else if (c.dist>MIN_DIST)
			pplus.push_back(c);
	}
	
	vector<candidate> temp; //candidates
	
	for (vector<side>::iterator citer1 = pminus.begin(); citer1 != pminus.end(); citer1++) {
		for (vector<side>::iterator citer2 = pplus.begin(); citer2 != pplus.end(); citer2++) {
			ofPoint vec = *citer1->iter - *citer2->iter;
			candidate t;
			float norm = vec.x*vec.x+vec.y*vec.y;
			t.alpha = acos((citer1->norm+citer2->norm-norm)/(2*citer1->dist*citer2->dist));
			if (t.alpha <= MAX_ALPHA * M_PI / 180) {
				t.iter = iter;
				t.sharpness = M_PI - fabs(t.alpha);
				temp.push_back(t);
			}
		}
	}
	
	cout << temp.size() << endl;
	
	if (temp.size()>0) {
		vector<candidate>::iterator sharpest = temp.begin();
		
		for (vector<candidate>::iterator titer=temp.begin(); titer!=temp.end(); titer++) {
			if (titer->sharpness>sharpest->sharpness) {
				sharpest = titer;
			}
		}
		
		
		
		candidates.push_back(*sharpest);
	}
	
	
}

bool ofxCornerDetector::isCorner(int index) {
}
