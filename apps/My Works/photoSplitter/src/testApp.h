#pragma once

#include "ofMain.h"
#include "ofxDirList.h"

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
	
	void loadImage();
		
	ofImage src;
	ofImage dest;
	
	int x;
	int y;
	int size;
	int lastWidth;
	int lastHeight;
	float scale;
	int col;
	int row;
	int fileCounter;
	
	string path;
	ofxDirList DIR;
	string currentName;
	
};
