#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxRemoteCameraClient.h"
#include "ProjectedOverlay.h"

#define PIXEL_PER_CENTIMETER 20

class testApp : public ofBaseApp{

	public:
        testApp():overlay(PIXEL_PER_CENTIMETER) {};
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void resized(int w, int h);

		void exit();
		ofxRemoteCameraClient remoteCam;
        ProjectedOverlay overlay;
    
        int counter;
};

#endif
