#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"

#include "ofxiVideoPlayer.h"

class testApp : public ofxiPhoneApp{
	
	public:
		void setup();
		void update();
		void draw();
		void audioOut( float * output, int bufferSize, int nChannels );
	
		void touchDown(ofTouchEventArgs &touch);
		void touchMoved(ofTouchEventArgs &touch);
		void touchUp(ofTouchEventArgs &touch);
		void touchDoubleTap(ofTouchEventArgs &touch);
        void touchCancelled(ofTouchEventArgs &touch);

    
    ofxiVideoPlayer player;
    ofRectangle tex;
    

};

