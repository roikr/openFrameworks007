#pragma once

#include "ofMain.h"
#include "ofxSerial.h"

class testApp : public ofBaseApp{
	
	public:
		void setup();
		void update();
		void draw();
    
        void serialConnect(int baudrate);

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
					// when did we last read?				
		
		ofxSerial	serial;
    
    
    int pin;
    int serialTimer;
    bool bConnected;
};

