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


class sliderPrefs {
	
public:
	
	
	
	sliderPrefs(){ };
	
	virtual ~sliderPrefs(){};
		
	vector<ofPoint> pages;
	
};



class ofxSlider {
public:
	void setup(float scale,sliderPrefs prefs = sliderPrefs());
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