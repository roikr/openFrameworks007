#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxSequencePlayer.h"
#include "ofxOnePointTracks.h"
#include "ofxAtlasMaker.h"

class testApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();
		void exit();
	
		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void resized(int w, int h);

		ofxSequencePlayer movie;
	ofxSequencePlayer baseFrameMovie;
		
	int numberOfMovies;
	int currentMovie;
	string filename;
	
	//bool bStore;
	//ofImage grab;
	
	
	
	//vector< ofImage*> images;
	
	ofxOnePointTracks lips;
	ofxAtlasMaker atlas;
	ofxAtlasMaker lipsAtlas;
	
	vector<string>paths;
	
	bool frameByFrame;
	
	int lipsModesNum;
	
	
	ofRectangle rect;
	ofImage rectImage;
	ofPoint mouseDiff;
	ofPoint moviePos;
	bool bDragRect;
	
	void updateRect();
};

#endif
