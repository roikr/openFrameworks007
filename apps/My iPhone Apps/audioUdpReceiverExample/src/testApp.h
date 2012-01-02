#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"

class testApp : public ofxiPhoneApp{

	public:

		void setup();
		void update();
		void draw();
		void exit();
	
		void audioOut( float * output, int bufferSize, int nChannels );
	
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
	
		int		sampleRate;
		float	* buffer;
		int		initialBufferSize;
		int		drawCounter, bufferCounter ,buffersReceived;
	
		bool bPlay;
		
};

