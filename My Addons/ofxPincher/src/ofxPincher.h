/*
 *  ofxPincher.h
 *  iPhonePinchZoomExample
 *
 *  Created by Roee Kremer on 11/16/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofMain.h"
#include "ofMath.h"

class pincherPrefs {
	
public:
	
	pincherPrefs(){ boundaryBox = ofRectangle(); width = height = 0 ; minScale = maxScale = 1; };
	pincherPrefs(int _w,int _h,ofRectangle _bb,float _min,float _max){
		boundaryBox = _bb;
		
		width = _w;
		height = _h;
		minScale = max(_min,max(_w/_bb.width,_h/_bb.height));
		maxScale = _max;
	}; 
	virtual ~pincherPrefs(){};
	
	
	
	ofRectangle boundaryBox;
	int width;
	int height;
	float minScale;
	float maxScale;
	
};


class ofxPincher {
	
public:
	void setup(ofPoint translate, float scale,pincherPrefs prefs = pincherPrefs());
	void update(float t);
	void transform();
	

	void draw();

	
	void touchDown(int x, int y, int id);
	void touchMoved(int x, int y, int id);
	void touchUp(int x, int y, int id);
	void touchDoubleTap(int x, int y, int id);
	bool getIsAnimating();
	
	
	
private:
	
	void applyTranslation(ofPoint trns,float scl);
	
	float distance(ofPoint pnt);
	
	
	ofPoint pos[2];
	bool active[2];
	
	int numActive;
	
	ofPoint translate;
	float scale;
	
	pincherPrefs prefs;
	
	bool bAnimating;
	
	//int animMode;
	
	float tgtScl;
	ofPoint tgtTrns;
	
};