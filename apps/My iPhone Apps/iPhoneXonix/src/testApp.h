#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxLine.h"
#include "ofxiFBO.h"
#include "cv.h"


struct evil {
	ofPoint pos;
};

class testApp : public ofxiPhoneApp {
	
public:
	void setup();
	void update();
	void draw();
	void exit();
	
	void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);

//	void floodFill(int x,int y,int target,int replacement,int stopCondition=0);
//	int getPixelColor(int x,int y);
	
	ofTexture		tex;
	int texSize;

	int w;
	int h;
	
	unsigned char 	* pixels;
	
	vector<ofxLine> lines;
	vector<ofxLine> strokes;
	vector<ofPoint> intersections;
	ofPoint pnt;
	
	bool bConnected;
	
	ofxiFBO fbo;
	
	bool bFill;
	bool bFilled;
	ofPoint fill1;
	ofPoint fill2;
	
	vector<evil> evils;
	
	IplImage* imagen;
};


