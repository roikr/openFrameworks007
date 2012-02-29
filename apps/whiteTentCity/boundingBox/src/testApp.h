#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxTrueTypeFontUC.h"

#define PIXELS_TO_METER 1000

struct quad{
    vector<ofVec3f> screen;
    vector<ofVec2f> vertices;
    ofMatrix4x4 glMatrix;
    ofMatrix4x4 matrix;
    
} ;

class testApp : public ofBaseApp {
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

		ofEasyCam cam; // add mouse controls for camera movement
        ofxAssimpModelLoader model;	
        ofMesh objectMesh;
    
    vector<pair<int,bool> > selection;
    int choice;
    
    bool bScreen;
    
    quad q;
    bool bProject;
    
    ofxTrueTypeFontUC font;
};
