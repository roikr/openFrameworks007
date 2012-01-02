#pragma once

#include "ofMain.h"

class testApp : public ofBaseApp{
	
	public:
		void setup();
        void generateGeometry();
		void update();
		void draw();
        void exit();

		void keyPressed  (int key);
		void keyReleased (int key);

		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		
    ofShader shader;
    GLuint vao,vbo[2];
    
    vector<ofVec2f> vertices;
    vector<ofVec2f> texCoords;
    
    GLuint textureID;
};

