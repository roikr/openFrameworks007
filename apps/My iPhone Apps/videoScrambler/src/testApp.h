#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"


#include "ofxiVideoStreamer.h"


struct videoTile {

	ofRectangle rect;
	ofRectangle tex;
	
};




class testApp : public ofxiPhoneApp {
	
public:
	void setup();
	void update();
	void draw();
	
	void fillVideoTile(videoTile &t,ofRectangle rect,ofRectangle videoRect);
	
	void setupTiles();
	
	bool getIsInside(ofRectangle &rect,ofPoint p);
	bool getIsIntersecting(ofRectangle &r1,ofRectangle &r2);
	
	void touchDown(int x, int y, int id);
	void touchMoved(int x, int y, int id);
	void touchUp(int x, int y, int id);
	void touchDoubleTap(int x, int y, int id);
	
	void audioRequested( float * output, int bufferSize, int nChannels );
	
		
	ofxiVideoStreamer streamer;
	
	ofPoint down;
	
	vector<videoTile> tiles;
	
	vector<videoTile>::iterator current;
	
	
	float offset;
	videoTile leftMargin;
	videoTile rightMargin;
	ofRectangle videoRect;
	
	bool bStarted;
	
	float tileSize;
	int tilesPerRow;

};


