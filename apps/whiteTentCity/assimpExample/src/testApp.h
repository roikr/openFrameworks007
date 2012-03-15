#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofVboMesh.h"
#include "ofxAssimpModelLoader.h"

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
    
        ofxAssimpModelLoader model;

        ofVboMesh mesh1;
        ofVboMesh mesh2;
        ofPoint position;
        float normScale;
        ofPoint scale;
        ofPoint sceneCenter;
        ofMaterial material;
        ofTexture tex;
        ofLight	light;
    
    ofEasyCam cam;
};

#endif
