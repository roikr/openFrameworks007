#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "Box2D.h"
#include "Render.h"
#include "ofxCoordinator.h"
#include "Teeter.h"

class testApp : public ofBaseApp, public b2ContactListener{

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
	
		void BeginContact(b2Contact* contact);
		void EndContact(b2Contact* contact);
	
	
	b2World m_world;
	b2Body* m_ground;
		
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
	float32 bias;
	
	
};

#endif
