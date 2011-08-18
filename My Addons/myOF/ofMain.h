/*
 *  ofMain.h
 *  YesPlastic
 *
 *  Created by Roee Kremer on 1/19/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#define MY_OF // used in ofMainExt.h to distinct from the official ofMain

#include "ofTypes.h"
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <ostream>

using namespace std;


//-------------------------------
//  find the system type --------
//-------------------------------

// 		helpful:
// 		http://www.ogre3d.org/docs/api/html/OgrePlatform_8h-source.html

#if defined( __WIN32__ ) || defined( _WIN32 )
#define TARGET_WIN32
#elif defined( __APPLE_CC__)
#include <TargetConditionals.h>

#if (TARGET_OF_IPHONE_SIMULATOR) || (TARGET_OS_IPHONE) || (TARGET_IPHONE)
#define TARGET_OF_IPHONE
#define TARGET_OPENGLES
#else
#define TARGET_OSX
#endif
#else
#define TARGET_LINUX
#endif
//-------------------------------

#ifndef MIN
#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif

enum ofLogLevel{
	OF_LOG_VERBOSE,
	OF_LOG_NOTICE,
	OF_LOG_WARNING,
	OF_LOG_ERROR,
	OF_LOG_FATAL_ERROR,
	OF_LOG_SILENT	//this one is special and should always be last - set ofSetLogLevel to OF_SILENT to not recieve any messages
};

#define OF_DEFAULT_LOG_LEVEL  OF_LOG_WARNING


struct ofTouchEventArgs {
	float x;
	float y;
	int id;
};



//class ofTrueTypeFont{
//	
//public:
//	
//	
//	ofTrueTypeFont();
//	virtual ~ofTrueTypeFont();
//	
//	// 			-- default, non-full char set, anti aliased:
//	void 		loadFont(string filename, int fontsize);
//	
//	void 		drawString(string s, float x, float y);
//	
//	void 		setLineHeight(float height);	
//};



class ofBaseApp{
	
public:
	ofBaseApp() {
		mouseX = mouseY = 0;
	}
	
	virtual ~ofBaseApp(){}
	
	virtual void setup(){}
	virtual void update(){}
	virtual void draw(){}
	virtual void exit(){}
		
	virtual void windowResized(int w, int h){}
	
	virtual void keyPressed( int key ){}
	virtual void keyReleased( int key ){}
	
	virtual void touchDown(float x, float y, int touchId) {};
	virtual void touchMoved(float x, float y, int touchId) {};
	virtual void touchUp(float x, float y, int touchId) {};
	virtual void touchDoubleTap(float x, float y, int touchId) {};
	
	virtual void audioReceived( float * input, int bufferSize, int nChannels ){}
	virtual void audioRequested( float * output, int bufferSize, int nChannels ){}
	
	int mouseX, mouseY;			// for processing heads
};

typedef ofBaseApp ofSimpleApp;

void ofRegisterTouchEvents(ofBaseApp *app);



void	ofEnableDataPath();
void	ofDisableDataPath();
void ofSetDataPathRoot(string newRoot);
string 	ofToDataPath(string path, bool absolute=false);


void ofLog(int logLevel, string message);
void ofLog(int logLevel, const char* format, ...);
void ofSetLogLevel(int logLevel);

string  ofToString(double value, int precision = 7);
string  ofToString(int  value);

void ofBackground(int r, int g, int b);

void ofPushMatrix();
void ofPopMatrix();
void ofTranslate(float x, float y, float z = 0);
void ofScale(float xAmnt, float yAmnt, float zAmnt = 1);
void ofRotate(float degrees, float vecX, float vecY, float vecZ);
void ofRotate(float degrees);

// color options
void ofSetColor(int r, int g, int b); // 0-255
void ofSetColor(int r, int g, int b, int a); // 0-255
void ofSetColor(int hexColor); // hex, like web 0xFF0033;

// transparency
void ofEnableAlphaBlending();  // this just turns on and off opengl blending, the common mode
void ofDisableAlphaBlending();

void ofRect(float x1,float y1,float w, float h);

// drawing options
void ofNoFill();
void ofFill();

float		ofClamp(float value, float min, float max);
int		ofGetElapsedTimeMillis();

int 		ofGetWidth();			// <-- should we call this ofGetWindowWidth?
int 		ofGetHeight();
float 		ofGetFrameRate();
void 		ofSetFrameRate(int targetRate);

vector<string>	ofSplitString(const string & text, const string & delimiter);

void 		ofSeedRandom();
void 		ofSeedRandom(int val);
float 		ofRandom(float val0, float val1);		// random (x - y)
float 		ofRandomf();							// random (-1 - 1)
float 		ofRandomuf();							// random (0 - 1) 

void ofBeginShape();
void ofVertex(float x, float y);
void ofEndShape(bool bClose = false);
void ofNextContour(bool bClose = false);  // for multi contour shapes!
