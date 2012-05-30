#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxPanZoom.h"
#include "ofxiTexture.h"
#include "ofxiVideoPlayer.h"


struct curve {
    ofVec2f p0;
    vector<vector<ofVec2f> > beziers;
    
};

struct line {
    ofVec2f p0;
    ofVec2f p1;
};


struct shape {
    vector<curve> curves;
    vector<line> lines;
};

struct bitmapInstance {
    
    string libraryItemName;
    
    ofVec2f translation;
    float scale;
    float rotation;
    
    bool bHasTransformationPoint;
    ofVec2f transformationPoint;
    
    ofxiTexture texture;
    float u; // width and height from 0 to 1
    float v;
};

struct symbolItem {
    vector<bitmapInstance> bitmaps;  
    vector<shape> shapes;
};

struct document {
    map<string, pair<int, int> > items;
};



class testApp : public ofxiPhoneApp {
	
public:
	void setup();
	void update();
	void draw();
    void audioOut( float * output, int bufferSize, int nChannels );
	void exit();
	
	void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);
    
    

	void lostFocus();
	void gotFocus();
	void gotMemoryWarning();
	void deviceOrientationChanged(int newOrientation);
    
    document parseDoc(string name);
    
    symbolItem parseSymbol(string name);
    void loadSymbol(symbolItem &item,document &doc);
    void drawSymbol(symbolItem &item);
    void releaseSymbol(symbolItem &item);
    
    
    
    document doc;
    symbolItem renders;
    symbolItem references;
    symbolItem shapes;
    
    ofxPanZoom cam;
    
    ofxiVideoPlayer player;
    
    vector<float> zOffsets;
    
    ofVec2f videoPos;

};


