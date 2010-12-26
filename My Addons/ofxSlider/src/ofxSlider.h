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
	
	
	
	sliderPrefs() : direction(SLIDER_HORIZONTAL), animDuration(5000) { };
	
	virtual ~sliderPrefs(){};
		
	vector<ofPoint> pages;
	
	int direction;
	int animDuration;
	
};



class ofxSlider {
public:
	void setup(float scale,sliderPrefs prefs = sliderPrefs());
	void update();
	void transform();
	

	void draw();

	
	void touchDown(int x, int y, int id);
	void touchMoved(int x, int y, int id);
	void touchUp(int x, int y, int id);
	void touchDoubleTap(int x, int y, int id);
	bool getIsAnimating();
	bool getIsDragging();
	int	 getCurrentPage();
	void setPage(int page);
	
	
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
	
	int state;
	
	vector<ofPoint>::iterator currentPage;
	
};