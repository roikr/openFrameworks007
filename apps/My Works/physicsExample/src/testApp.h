#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "Box2D.h"
#include "Render.h"
#include "ofxCoordinator.h"

class testApp : public ofBaseApp{

	public:
		testApp() : m_world(b2Vec2(0.0f,-10.0f),true) {};
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
	
	b2World m_world;
	b2Body* m_ground;
	b2Body* m_teeter;
	
	b2Fixture* m_fixture1;
	b2Fixture* m_fixture2;
	
	float32 m_distance;
	
	int32 velocityIterations;
	int32 positionIterations;
	float32 timeStep;
	
	DebugDraw m_debugDraw;
	
	ofxCoordinator coordinator;
	
};

#endif
