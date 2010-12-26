/*
 *  ofxSlider.h
 *  iPhoneSliderExample
 *
 *  Created by Roee Kremer on 12/24/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofMain.h"
#include "ofMath.h"

enum  {
	SLIDER_HORIZONTAL,
	SLIDER_VERTICAL
};

class sliderPrefs {
	
public:
	
	
	
	sliderPrefs() : direction(SLIDER_HORIZONTAL) { };
	
	virtual ~sliderPrefs(){};
		
	vector<ofPoint> pages;
	
	int direction;
	
};



class ofxSlider {
public:
	void setup(int startPage,float scale,sliderPrefs prefs = sliderPrefs());
	void update();
	void transform();
	
//#ifdef OF_DEBUG
	void draw();
//#endif
	
	void touchDown(int x, int y, int id);
	void touchMoved(int x, int y, int id);
	void touchUp(int x, int y, int id);
	void touchDoubleTap(int x, int y, int id);
	bool getIsAnimating();
	
	
	
private:
	
	float boundsFix(float d);
	float getComponent(ofPoint pnt);
	void setComponent(ofPoint &pnt,float x);
	
	vector <pair<ofPoint,int> > touches;
	ofPoint dest;
	ofPoint vel;
		
	int upTime;

	float scale;
	ofPoint translate;
	
	sliderPrefs prefs;
	
	bool bAnimating;
	
	vector<ofPoint>::iterator currentPage;
	
};