#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "Box2D.h"
#include "Render.h"
#include "ofxCoordinator.h"
#include "Teeter.h"
#include "ofxSimpleSegmentator.h"

class testApp : public ofBaseApp, b2ContactListener{
	public:
		testApp() : m_world(b2Vec2(0.0f,-10.0f),true) {};
		void setup();
	void setupPhysics();
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
	
		void nextTeeter();
	
		void BeginContact(b2Contact* contact);
		void EndContact(b2Contact* contact);
	
	

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
		b2Body* m_ground;
		b2Fixture *m_groundFixture;
		
		
		int32 velocityIterations;
		int32 positionIterations;
		float32 timeStep;
		
		DebugDraw m_debugDraw;
		
		ofxCoordinator coordinator;
	
		vector<Teeter*> teeters;
		vector<Teeter*>::iterator current;
		vector<Teeter*>::iterator next;
		
		bool bTrans;
		int animStart;
		b2Vec2 position;
		float32 scale;
		
		int32 m_stepCount;
	
	
		ofxSimpleSegmentator segmentator;
		ofPoint mouseDown;
	
		int blobState;
	
};
