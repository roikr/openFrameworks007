#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"

struct param {
	string name;
	vector<float> samples;
	float maxValue;
	float minValue;
};

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
	
		void load();
		void save();
		void dump();

	
	vector<param> params;
	vector<param>::iterator piter;
	int numSamples;
	
	ofRectangle graph;
	
	
};

#endif
