#pragma once

#include "ofMain.h"
#include "ofxFBO.h"

class testApp : public ofBaseApp{
	
	public:
		void setup();
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
    ofShader dct_rows_fprog;
    ofShader dct_rows2_fprog;
    ofShader dct_unpack_rows_fprog;
    ofShader dct_cols_fprog;
    ofShader dct_cols2_fprog;
    ofShader dct_unpack_cols_fprog;
    
    GLuint vao,vbo[2];
    
    vector<ofVec2f> vertices;
    vector<ofVec2f> texCoords;
    
    GLuint textureID;
    
    ofxFBO fbo[3];
};

