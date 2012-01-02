#pragma once

#include "ofxNetwork.h"
#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"

class testApp : public ofxiPhoneApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();
	
		void audioIn( float * input, int bufferSize, int nChannels );

	
		void touchDown(ofTouchEventArgs &touch);
		void touchMoved(ofTouchEventArgs &touch);
		void touchUp(ofTouchEventArgs &touch);
		void touchDoubleTap(ofTouchEventArgs &touch);
		void touchCancelled(ofTouchEventArgs &touch);
		
		void lostFocus();
		void gotFocus();
		void gotMemoryWarning();
		void deviceOrientationChanged(int newOrientation);
	

		ofxUDPManager udpConnection;

		ofTrueTypeFont  mono;
		ofTrueTypeFont  monosm;
		vector<ofPoint> stroke;
	
		int		initialBufferSize;
		int		sampleRate;
		int		drawCounter, bufferCounter;
		int		buffersSent;
		float 	* buffer;
	
		bool bRecord;
	
};

