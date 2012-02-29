#pragma once

#include "ofMain.h"
#include "ofxTrueTypeFontUC.h"
#include "Box2D.h"
#include "Render.h"

class testApp : public ofBaseApp,public b2DestructionListener{
    
public:
    
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
    
    void addWord(wstring str,ofVec2f pos);
    
    void MouseDown(const b2Vec2& p);
    void MouseUp(const b2Vec2& p);
    void MouseMove(const b2Vec2& p);
    
    void SayGoodbye(b2Fixture* fixture) { B2_NOT_USED(fixture); }
	void SayGoodbye(b2Joint* joint);
    virtual void JointDestroyed(b2Joint* joint) { B2_NOT_USED(joint); }

    ofxTrueTypeFontUC font;
    
    
    b2World *m_world;
	b2Body *m_groundBody;
	
    b2MouseJoint* m_mouseJoint;
    b2Vec2 m_mouseWorld;
    
	int32 velocityIterations;
	int32 positionIterations;
	float32 timeStep;
    int32 m_stepCount;
    
    vector<b2Body*> tiles;
		
    DebugDraw m_debugDraw;
    
    float pixelsToMeter;
    
    vector<pair<wstring,b2Body*> > words;
    
    bool bDrawDebug;
    
};
