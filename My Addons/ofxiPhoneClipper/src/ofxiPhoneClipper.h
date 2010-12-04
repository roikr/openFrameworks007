//
//  ofxiPhoneClipper.h
//  iPhoneGrabExample
//
//  Created by Roee Kremer on 11/8/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#include <vector>
using namespace std;


@class MyBuffer;

class ofxiPhoneClipper {
	
public:
	
	//ofxiPhoneClipper();
	//~ofxiPhoneClipper();
	
	void setup();
	void update();
	void draw();
	void exit();
	
	void startSession();
	void stopSession();
	bool getIsSessionRunning();
	void startRecording();
	void stopRecording();
	bool getIsRecording();
	void play();
	
	void startScrubbing();
	void stopScrubbing();
	bool getIsScrubbing();
	void setPosition(float pos);
	
		
protected:
	
	
		
	bool bIsSessionRunning;
	
	MyBuffer *	videoTexture;
	int start;
	
	uint currentTexture;
	
	int state;
};


