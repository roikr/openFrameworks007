#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "Box2D.h"
#include "Render.h"
#include "ofxCoordinator.h"
#include "Teeter.h"
#include "ofxSimpleSegmentator.h"


#define RESET_DELAY 5000
#define OUT_DELAY 1000
#define CENTER_DELAY 2000
#define NUM_TEETERS 12 // 12
#define MIN_BLOB_AREA 2500 // 7500

class testApp : public ofBaseApp, b2ContactListener{
	public:
		testApp() : m_world(b2Vec2(0.0f,-10.0f),true) {};
		void setup();
		void setupPhysics();
		void createTeeters();
		void update();
		void draw();
		void exit();
	
		void drawPointCloud();

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
	
		void next();
		void jump();
		void leave();
		void reset();
	
	
		void BeginContact(b2Contact* contact);
		void EndContact(b2Contact* contact);
		
		void DrawShape(b2Fixture* fixture, const b2Transform& xf);
	

		ofxKinect kinect;

		ofxCvColorImage		colorImg;
		ofxCvGrayscaleImage 	grayImage;
		ofxCvContourFinder 	contourFinder;
		
		int					angle;
		int 				pointCloudRotationY;
	
		void saveImage();
		
		int mode;
		int displayMode;
	
		b2World m_world;
		b2Body* ground;
		b2Fixture *baseFixture;
		b2Fixture *groundFixture;
		
		
		int32 velocityIterations;
		int32 positionIterations;
		float32 timeStep;
		
		DebugDraw m_debugDraw;
		
		ofxCoordinator coordinator;
	
		vector<Teeter*> teeters;
		vector<Teeter*>::iterator citer;
		
		
		bool bTrans;
		int animStart;
		bool bJump;
		bool bReset;
	
		b2Vec2 position;
		float32 scale;
	
		ofxSimpleSegmentator segmentator;
		ofPoint mouseDown;
	
		ofRectangle rect;
			
		int centerTimer;
		int outTimer;
		int resetTimer;
	
		float32 startBlobY;
	
	bool bShowCursor;
	
};
