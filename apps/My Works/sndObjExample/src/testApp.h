#ifndef _TEST_APP
#define _TEST_APP

//#define MACOSX

#include "ofMain.h"
#include "AudioDefs.h"


class testApp : public ofSimpleApp{
	
	public:
		
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased();
		
	int counter;
		
};

#endif	

