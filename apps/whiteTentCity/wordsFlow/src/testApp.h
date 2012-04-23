#pragma once

#include "ofMain.h"
#include "ofxTrueTypeFontUC.h"
#include "ofxAssimpModelLoader.h"
#include "Box2D.h"
#include "Render.h"
#include "ofxOsc.h"

#include <set>
#include  "ProjectedOverlay.h"
#include "ofxRemoteCameraClient.h"


#define PROJECTOR_PIXEL_PER_CENTIMETER 20.0f 
#define REAL_PIXEL_PER_CENTIMETER 6.0f 


   
struct word {
    wstring str;
    b2Body* body;
};



#define PORT 12000

class testApp : public ofBaseApp,public b2DestructionListener{
    
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
//    void BeginContact(b2Contact* contact);
//    void EndContact(b2Contact* contact);
    
  


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
    
  
    ProjectedOverlay overlay;
    
    ofEasyCam cam;
    bool bEasyCam;
    ofMatrix4x4 projMat,mvMat;
    
    ofMesh cityMesh;
    ofMesh tentMesh;
    ofLight light;
    
    ofxOscReceiver receiver;
    ofxRemoteCameraClient remoteCam;

    bool bExplode;
    
};
