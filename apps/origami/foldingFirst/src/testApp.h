#pragma once

#include "ofMain.h"

struct face {
    vector<ofVec2f> vertices;
    ofColor color;
   
};

class origami {
public:
    
    void setup();
    void cut(ofVec2f p0,ofVec2f p1);
    void draw();
    
    
    vector<face> faces;
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
    

    origami obj;
    ofVec2f downPos;
		
};
