#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxTrueTypeFontUC.h"
#include "ProjectedOverlay.h"


#define PIXEL_PER_CENTIMETER 20
#define WORD_LENGTH 15 // CENTIMETER



class testApp : public ofBaseApp {
	public:
        testApp():overlay("settings.xml",PIXEL_PER_CENTIMETER) {};
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
        void drawCity();
    
        
    

		ofEasyCam cam; // add mouse controls for camera movement
        ofxAssimpModelLoader cityModel;	
        ofxAssimpModelLoader tentModel;	
        ofMesh cityMesh;
        ofMesh tentMesh;
        ofLight light;
    
    
    int choice;
    
    bool bScreen;
    
    bool bProject;
    
    ofxTrueTypeFontUC font;
    
    int state;
    
    vector<pair<int,bool> > selection;
    
    int currentScreen;
    
    bool bTent;
    bool bOrigin;
    int origin;
    
    
    float scale;
    
    
    ProjectedOverlay overlay;
    
    vector<ofVec3f> bb;
    
    
};
