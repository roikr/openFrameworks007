#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"

#include "ofxOsc.h"
#include "ofxCornerDetector.h"

#define HOST "localhost"
#define RECEIVE_PORT 9001
#define SEND_PORT 9000

//--------------------------------------------------------
class testApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

		ofTrueTypeFont		font;

	private:
	
		vector<ofPoint> curve;
		bool	bSmooth;
		
	
		int currentCorner;

		ofxOscSender sender;
		ofxCornerDetector detector;
	
		ofxOscReceiver	receiver;
	
		bool bGotHost;



};

#endif
