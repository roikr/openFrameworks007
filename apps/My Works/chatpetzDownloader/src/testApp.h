#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxXmlSettings.h"

enum {
	STATE_NORMAL,
	STATE_HIGHLIGHTED,
	STATE_SELECTED
};

struct bundle {
	string path;
	string title;
	ofPoint pos;
	ofImage *normal;
	ofImage *highlighted;
	ofImage *selected;
	
	int state;
};

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

		void drawTexture(ofTextureData &texData, float x, float y, float w, float h);

		ofxXmlSettings xml;
		ofTrueTypeFont ttf;
	
	ofImage background;
	
	
	ofImage normal;
	ofImage highlighted;
	ofImage selected;
	ofPoint titleOffset;
	
	vector<bundle> bundles;
	
	float progress;
	ofPoint progressPos;
	ofImage progressBar;
	ofImage progressBackground;

};

#endif

