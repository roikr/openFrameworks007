#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxTrueTypeFontUC.h"

#define PIXELS_TO_METER 1000

struct screen {
    int index; // index of the first vertex of the screen for deleting by first index
    vector<int> indices;
    ofVec3f origin;
    ofVec3f xVec;
    ofVec3f yVec;
    vector<ofVec2f> proj;
    ofMatrix4x4 glMat;

    
} ;

struct tent {
    int first;
    int second;
    ofMatrix4x4 glMat;
};

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
    
        void drawTent();
        void drawScreen();
        void drawModel();
    
        void load();
        void save();
        void log();
    
        void updateMatices();
    

		ofEasyCam cam; // add mouse controls for camera movement
        ofxAssimpModelLoader cityModel;	
        ofxAssimpModelLoader tentModel;	
        ofMesh cityMesh;
        ofMesh tentMesh;
    
    
    int choice;
    
    bool bScreen;
    
    bool bProject;
    
    ofxTrueTypeFontUC font;
    
    int state;
    
    vector<pair<int,bool> > selection;
    vector<screen> screens;
    int currentScreen;
    
    bool bTent;
    bool bOrigin;
    int origin;
    vector<tent> tents;
    
    float scale;
    
    vector<ofRectangle> rects;
    
    ofVec2f downPos;
    
    
};
