#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"


class ofxSndFile;

class testApp : public ofBaseApp {
	
	public:
		
		void setup();
		void update();
		void draw();
		void exit();
	
	void audioRequested 	(float * output, int bufferSize, int nChannels);
		
		
		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
	
	vector<ofxSndFile*> sounds;
	
	
		
			int bufferSize;
			float sampleRate;
			
		ofTrueTypeFont	font;
		float 			synthPosition;
		

};

#endif	