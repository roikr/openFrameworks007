#pragma once

#include "ofMain.h"

#ifdef TARGET_WIN32

#endif

#if defined(TARGET_OSX) || defined(TARGET_LINUX)
#include "ofxSerial.h"
#endif



struct page {
    int start;
    int loop;
    int end;
    int mask;
    int lang;
};

class testApp : public ofBaseApp,public ofThread{

	public:

		void setup();
		void read();
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


#ifdef TARGET_WIN32
    ofSerial serial;
#endif
#if defined(TARGET_OSX) || defined(TARGET_LINUX)
    ofxSerial *serial;
#endif

    bool bSerial;
    int pins;
    int lang;

};

