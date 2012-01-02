#pragma once

#include "ofMain.h"
#include "ofxDirList.h"

struct tile {
	ofImage image;
	int col;
	int row;
	float scale;
};

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
	
	void load();
	
		
	float tilesScale;
	vector<tile> tiles;
	int width;
	int height;
	
	string path;
	string name;
	
	ofPoint pos;
	float scale;
	
	ofPoint downPos;
	
};
