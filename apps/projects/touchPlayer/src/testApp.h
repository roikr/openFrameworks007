#pragma once

#include "ofMain.h"

struct item {
	ofPoint pos;
	ofImage thumb;
	ofVideoPlayer movie;
};
	

class testApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);		
	
	vector<item> items;
	vector<item>::iterator citer;
//	ofImage background;
	ofImage videoBackground;
	ofImage back;
	ofPoint backPos;
	float margin;
	float width;
	float height;

	ofVideoPlayer menuVideo;
	ofPoint menuVideoPos;
};

