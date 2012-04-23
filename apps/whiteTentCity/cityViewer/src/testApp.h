#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxTrueTypeFontUC.h"
#include "ofxOsc.h"
#include "ProjectedOverlay.h"

#define PIXEL_PER_CENTIMETER 20


struct message {
    vector<string> words;
    int current;
    int startTime;
    float alpha;
};



class testApp : public ofBaseApp {
	public:
        testApp():overlay(PIXEL_PER_CENTIMETER) {};
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
    
    
        ofMesh cityMesh;
        ofMesh tentMesh;
    
    ofMatrix4x4 projMat,mvMat;
   
    
    bool bDebug;
    bool bProject;
    bool bEasyCam;
    
    ofxTrueTypeFontUC font;

    ofLight light;
    ofEasyCam cam;
    
    map<int,message> messages;
    
    ofxOscReceiver receiver;
    
    string lastMessage;
    
    ProjectedOverlay overlay;
    
    int wordDuration;
    int messageRepetition;
    
    int cityBrightness;
    int tentBrightness;
};
