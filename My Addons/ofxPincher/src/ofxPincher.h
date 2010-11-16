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

class ofxPincher {
	
public:
	void setup(ofPoint translate, float scale,ofRectangle box=ofRectangle(0,0,0,0),ofPoint view=ofPoint(480,320));
	void transform();
	//void draw();
	
	void touchDown(int x, int y, int id);
	void touchMoved(int x, int y, int id);
	void touchUp(int x, int y, int id);
	
	
	
	
private:
	
	float distance(ofPoint pnt);
	
	ofPoint pos[2];
	bool active[2];
	
	int numActive;
	
	ofPoint translate;
	float scale;
	
	ofRectangle boundry;
	ofPoint port;
	
	
	
};