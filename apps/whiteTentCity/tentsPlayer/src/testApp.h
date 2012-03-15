#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxTrueTypeFontUC.h"
#include "ofxOsc.h"

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

struct message {
    int tent;
    vector<string> words;
    int current;
    int startTime;
    float alpha;
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
    
        void load();
        void updateMatices();
    
        void sendMessage(int tent,string str);
    

        ofxAssimpModelLoader cityModel;	
        ofxAssimpModelLoader tentModel;	
        ofMesh cityMesh;
        ofMesh tentMesh;
    
    ofMatrix4x4 projMat,mvMat;
   
    
    bool bScreen;
    bool bProject;
    bool bEasyCam;
    
    ofxTrueTypeFontUC font;

    vector<screen> screens;
    vector<tent> tents;
    float scale;
    vector<ofRectangle> rects;
    
    ofLight light;
    
    ofEasyCam cam;
    
    
    vector<message> queue;
    vector<message> display;
    
    ofxOscReceiver receiver;
    
    string lastMessage;
};
