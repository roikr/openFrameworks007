#pragma once

#include "ofMain.h"
#include "ofxTrueTypeFontUC.h"

typedef struct {

	ofVec3f pos;
	bool 	bBeingDragged;
	bool 	bOver;
	float 	radius;
	
}draggableVertex;

typedef struct {
    vector<int> vertices;
    ofMatrix4x4 glMatrix;
    ofMatrix4x4 matrix;
    
} quad;

typedef struct {
    vector<int> vertices;
} connection;


typedef struct  {
    vector<int> vertices;
} base;


class testApp : public ofBaseApp{
	
	public:
		
		void setup();
		void update();
		void draw();
    
        void calcQuad(quad &q);
        void calcTents();
    
        void findHomography(ofPoint src[4], ofPoint dst[4], float homography[16]);
        void gaussian_elimination(float *input, int n);

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);		
		
		
        vector<ofVec3f> src;
        GLfloat matrix[16];
    
    ofxTrueTypeFontUC font;
    
    quad cityQuad;
    
    vector<draggableVertex>vertices;
    
    vector<ofVec3f> anchors;
    
    vector<base> bases;
    vector<quad> quads;
    vector<int> quadsAnchors;
    
    bool bJustText;
    
    wstring message;
    float messageWidth;
    float messageHeight;
    

};


	
