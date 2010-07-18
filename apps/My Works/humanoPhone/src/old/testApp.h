#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxAlphaVideoPlayer.h"
#include "ofxOsc.h"


struct note {
	int midi;
	ofxAlphaVideoPlayer noteOn;
	ofxAlphaVideoPlayer idle;
	int x;
	int y;
	float scale;
	
	bool bNoteOn;
	bool bIdle;
	bool bWhite;
	
	bool bPlaying;
	
	int start;
};

class testApp : public ofBaseApp{

	public:
		void setup();
		void loadFromXml();
		void saveToXml();
		void refresh();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
	
	ofImage background;
	map<int,note*> notes;
	map<int,note*>::iterator noteIter;
	
	ofxOscReceiver	receiver;
	
	int downX;
	int downY;
	
	int x0;
	int y0;
	float s0;
	
	map<int,bool> whites;
	
	bool bScaling;
	bool bBackground;
	
};

#endif
