#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"

#include "ofxiVideoStreamer.h"
#include "ofxCoordinator.h"
#include <Box2D/Box2D.h>

struct videoTile {

	ofRectangle rect;
	ofRectangle tex;
	
};



@class Box2DAccel;
class testApp : public ofxiPhoneApp {
	
public:
	testApp() : world(b2Vec2(0.0f,0.0f),true),m_mouseJoint(NULL),m_stepCount(0) {};
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
	
	void setGravity(float x,float y);
		
	ofxiVideoStreamer streamer;
	
	ofPoint down;
	
	vector<videoTile> testTiles;
	
	
	
	//vector<videoTile>::iterator current;
	
	
	float offset;
	videoTile leftMargin;
	videoTile rightMargin;
	ofRectangle videoRect;
	
	bool bStarted;
	
	float tileSize;
	int tilesPerRow;
	
	ofxCoordinator coordinator;
	
	b2World world;
		
	int32 velocityIterations;
	int32 positionIterations;
	float32 timeStep;
	
	//GLESDebugDraw m_debugDraw;
	
	b2MouseJoint* m_mouseJoint;
	b2Vec2 m_mouseWorld;
	b2Body* m_groundBody;
	int32 m_stepCount;
	
	Box2DAccel *accel;

};


