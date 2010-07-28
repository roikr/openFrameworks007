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




void ofxCornerDetector::setup(int min_dist,int max_dist,int max_alpha) {
	this->min_dist=min_dist;
	this->max_dist=max_dist;
	this->max_alpha = max_alpha;
	curve.clear();
		
}

void ofxCornerDetector::update() {
	
	corners.clear();
	candidates.clear();
	
	//cout << "points: " << curve.size() << endl;
	//printf("%i points\n",curve.size());
	if (firstPass()) {
		//cout << "candidates: " << candidates.size() << endl;
//		for (vector<candidate>::iterator iter = candidates.begin(); iter!=candidates.end() ; iter++) {
//			printf("\tx:%3.0f y:%3.0f alpha:%3.0f sharpness:%3.0f\n",iter->iter->x,iter->iter->y,iter->alpha*180/M_PI,iter->sharpness*180/M_PI);
//		}
		
		secondPass() ;
	}
		
	
}

void ofxCornerDetector::addPoint(ofPoint &point) {
	curve.push_back(point);
}



void ofxCornerDetector::checkPoint(vector<ofPoint>::iterator iter) {
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
		if (c.dist > max_dist)
			break;
		else if (c.dist>min_dist)
			pminus.push_back(c);
		
	}
	
	
	for (piter = iter;  piter!=curve.end(); piter++) {
		side c;
		c.iter = piter;
		ofPoint vec = *(c.iter) - (*iter);
		c.norm = vec.x*vec.x+vec.y*vec.y;
		c.dist = sqrt(c.norm);
		if (c.dist > max_dist)
			break;
		else if (c.dist>min_dist)
			pplus.push_back(c);
	}
	
	vector<candidate> temp; //candidates
	
	for (vector<side>::iterator citer1 = pminus.begin(); citer1 != pminus.end(); citer1++) {
		for (vector<side>::iterator citer2 = pplus.begin(); citer2 != pplus.end(); citer2++) {
			ofPoint vec = *citer1->iter - *citer2->iter;
			candidate t;
			float norm = vec.x*vec.x+vec.y*vec.y;
			t.alpha = acos((citer1->norm+citer2->norm-norm)/(2*citer1->dist*citer2->dist));
			if (t.alpha <= max_alpha * M_PI / 180) {
				t.iter = iter;
				t.sharpness = M_PI - fabs(t.alpha);
				temp.push_back(t);
			}
		}
	}
	
	//cout << temp.size() << endl;
	
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


int ofxCornerDetector::firstPass() {
	
	
	for (vector<ofPoint>::iterator iter = curve.begin();  iter!=curve.end(); iter++) {
		checkPoint(iter);
	}
	
	return candidates.size() ;
}


bool ofxCornerDetector::areNeighbors(candidate &c1,candidate &c2) {
	ofPoint vec = *(c1.iter) - *(c2.iter);
	float norm = vec.x*vec.x+vec.y*vec.y;
	float dist = sqrt(norm);
	return dist<=min_dist;
}

void ofxCornerDetector::addCorner(vector<candidate>::iterator citer) {
	corner c;
	c.index = distance(curve.begin(), citer->iter);
	c.angle = citer->alpha;
	corners.push_back(c);
	//printf("corner: index: %i, x:%3.0f y:%3.0f angle:%3.0f\n",c.index,curve[c.index].x,curve[c.index].y,c.angle*180/M_PI);
	
}


int ofxCornerDetector::secondPass() {
	
	
	vector<candidate>::iterator citer = candidates.begin();
	
	for (vector<candidate>::iterator iter = citer+1; iter!=candidates.end() ; iter++) {
		if (areNeighbors(*iter,*citer)) {
			if (iter->sharpness >= citer->sharpness) {
				citer = iter;
			}
		} else {
			addCorner(citer);
			citer = iter;
			
		}
	}
	
	addCorner(citer);
	
	return corners.size();
	
}


int ofxCornerDetector::getNumCorners() {
	return corners.size();
}


int ofxCornerDetector::getCornerIndex(int cornerNum) {
	
	return cornerNum<corners.size() ? corners[cornerNum].index : -1;
}


float ofxCornerDetector::getPathLength(int index1,int index2) {
	
	if (index1 >= curve.size() || index2 >= curve.size() || index1>index2) {
		return 0;
	}
	
	
	
	float length = 0;
	vector<ofPoint>::iterator iter1,iter2;
	
	for (vector<ofPoint>::iterator iter1= curve.begin()+index1; iter1!=curve.begin()+index2; iter1++) {
		iter2 = iter1+1;
		float sum = pow(iter2->x-iter1->x, 2) + pow(iter2->y-iter1->y, 2);
		if ( sum!=0) {
			length+=sqrt(sum);
		}
	}
	
	return length;
}


float ofxCornerDetector::getTotalPathLength() {
	return getPathLength(0, curve.size()-1);
}



vector<corner> &ofxCornerDetector::getCorners() {
	
	
	
}



bool ofxCornerDetector::isCorner(int index) {
}
