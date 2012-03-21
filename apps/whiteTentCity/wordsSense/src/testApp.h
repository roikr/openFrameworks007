#pragma once

#include "ofMain.h"
#include "ofxTrueTypeFontUC.h"
#include "Box2D.h"
#include "Render.h"

#include "ofxTSPSReceiver.h"
#include <set>
#include  "ProjectedOverlay.h"

#define PROJECTOR_PIXEL_PER_CENTIMETER 20.0f 
#define REAL_PIXEL_PER_CENTIMETER 6.0f 


   
struct word {
    wstring str;
    b2Body* body;
};

struct sensor {
    ofxTSPSPerson* person;
    set<b2Body*> bodies;
};


#define PORT 12000

class testApp : public ofBaseApp,public b2ContactListener,public b2DestructionListener, public ofxTSPSListener{
    
public:
    testApp():overlay(PROJECTOR_PIXEL_PER_CENTIMETER) {};

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
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void addWord(wstring str);
    
    void MouseDown(const b2Vec2& p);
    void MouseUp(const b2Vec2& p);
    void MouseMove(const b2Vec2& p);
    
    void SayGoodbye(b2Fixture* fixture) { B2_NOT_USED(fixture); }
	void SayGoodbye(b2Joint* joint);
    virtual void JointDestroyed(b2Joint* joint) { B2_NOT_USED(joint); }
    void BeginContact(b2Contact* contact);
    void EndContact(b2Contact* contact);
    
    b2Fixture* createSensor(ofxTSPSPerson* person);
    
    //called when the person enters the system
	void personEntered( ofxTSPSPerson* person, ofxTSPSScene* scene );
	//called each time the centroid moves (a lot)
	void personMoved( ofxTSPSPerson* person, ofxTSPSScene* scene );
	//called one frame before the person is removed from the list to let you clean up
	void personWillLeave( ofxTSPSPerson* person, ofxTSPSScene* scene );
	//called every frame no matter what.
	void personUpdated(ofxTSPSPerson* person, ofxTSPSScene* scene);

    ofxTrueTypeFontUC font;
    
    
    b2World *m_world;
	b2Body *m_groundBody;
    b2Body *m_islands;
	
    b2MouseJoint* m_mouseJoint;
    b2Vec2 m_mouseWorld;
    
	int32 velocityIterations;
	int32 positionIterations;
	float32 timeStep;
    int32 m_stepCount;
    
    
    DebugDraw m_debugDraw;
    
    
    vector<word> words;
    
    bool bDrawDebug;
    
    ofxTSPSReceiver* receiver;
    map<b2Fixture*,sensor*> sensors;
    
    ProjectedOverlay overlay;
    
    ofEasyCam cam;
    
};
