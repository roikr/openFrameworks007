#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxHTTPServer.h"
#include "ofxSerial.h"

class testApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();
        void exit();

        void trigger();

		void keyPressed(int key);
        void keyReleased(int key) {};
		void mouseMoved(int x, int y ) {};
		void mouseDragged(int x, int y, int button) {};
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button) {};
		void windowResized(int w, int h) {};
		void dragEvent(ofDragInfo dragInfo) {};
		void gotMessage(ofMessage msg) {};

		ofVideoGrabber 		vidGrabber;

		int 				camWidth;
		int 				camHeight;
		float photoWidth;
		float photoHeight;
		float thumbWidth;
		float thumbHeight;


    map<string,ofxOscSender*> senders;
    ofxOscReceiver receiver;

    int delayTimer;
    bool bTrigger;
    ofSoundPlayer sound;
    int delay;
    ofImage image;

    ofxHTTPServer *server;

    ofxSerial *serial;
    int blinkCounter;
    int blinkTimer;

    int processTimer;
    float lifetime;
    
    int imageCounter;

};
