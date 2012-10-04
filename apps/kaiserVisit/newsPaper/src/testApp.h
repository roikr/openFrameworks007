#pragma once

#include "ofMain.h"
#include "ofxSerial.h"

struct page {
    int start;
    int loop;
    int end;
    int mask;
};

class testApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();
    
        void pageUp();
        void pageDown(int pageNum);
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);		

    ofVideoPlayer 		video;
    ofImage             background;
    vector<page> pages;
    
    float alpha;
    int fadeStart;
    bool bFadeOut;
    int currentPage;
    int nextPage;
    bool bPageDown;
    
    ofxSerial *serial;
    
    unsigned int pins;
    unsigned int lang;
    
};

