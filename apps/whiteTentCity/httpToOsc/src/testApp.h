#pragma once

#include "ofMain.h"
#include "ofxHTTPServer.h"
#include "ofxOsc.h"

struct message {
    int tent;
    string str;
    int numWords;
    int startTime;
    
};

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        void getRequest(ofxHTTPServerResponse & response);
//        void postRequest(ofxHTTPServerResponse & response);
    
        
        ofxHTTPServer * server;
        ofxOscSender sender;
    
        vector<message> queue;
        vector<message> display;
    
        int wordDuration;
        int messageRepetition;
		
};
