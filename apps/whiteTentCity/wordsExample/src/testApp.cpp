#include "testApp.h"


void testApp::SayGoodbye(b2Joint* joint)
{
	if (m_mouseJoint == joint)
	{
		m_mouseJoint = NULL;
	}
	else
	{
		JointDestroyed(joint);
	}
}


void testApp::addWord(wstring str,ofVec2f pos) {
    b2PolygonShape shape;
    shape.SetAsBox(font.stringWidth(str)/pixelsToMeter/2.0, font.stringHeight(str)/pixelsToMeter/2.0);
    
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position =  b2Vec2(pos.x/pixelsToMeter,pos.y/pixelsToMeter);
    
    b2Body* body = m_world->CreateBody(&bd);
    tiles.push_back(body);
    body->CreateFixture(&shape, 5.0f);
    
    words.push_back(make_pair(str, body));
}


//--------------------------------------------------------------
void testApp::setup(){
    
    
    font.loadFont("Guttman Hatzvi-Normal.ttf", 50,true,true);
    
    {
        pixelsToMeter = 100;
        
        m_world = new b2World(b2Vec2(0.0f,0.0f)); 
        
        m_world->SetDestructionListener(this);
        m_world->SetDebugDraw(&m_debugDraw);
        
        uint32 flags = b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_centerOfMassBit ;
        
        //	flags += settings->drawJoints			* b2DebugDraw::e_jointBit;
        //	flags += settings->drawAABBs			* b2DebugDraw::e_aabbBit;
        //	flags += settings->drawPairs			* b2DebugDraw::e_pairBit;
        //	flags += settings->drawCOMs				* b2DebugDraw::e_centerOfMassBit;
        m_debugDraw.SetFlags(flags);
        
        
        b2BodyDef bd;
        m_groundBody = m_world->CreateBody(&bd);
        
        float32 width = ofGetWidth()/pixelsToMeter;
        float32 height = ofGetHeight()/pixelsToMeter;
        b2Vec2 vs[4];
        vs[0].Set(0.0, 0.0f);
        vs[1].Set(width, 0.0f);
        vs[2].Set(width, height);
        vs[3].Set(0.0f, height);
        
        
        b2ChainShape loop;
        loop.CreateLoop(vs, 4);
        b2FixtureDef fd;
        fd.shape = &loop;
        fd.density = 0.0f;
        m_groundBody->CreateFixture(&fd);
    }
    
    {
        addWord(L"םעה",ofVec2f(3*ofGetWidth()/4,ofGetHeight()/4));
        addWord(L"שרוד",ofVec2f(ofGetWidth()/4,ofGetHeight()/4));
        addWord(L"קדצ",ofVec2f(3*ofGetWidth()/4,3*ofGetHeight()/4));
        addWord(L"יתרבח",ofVec2f(ofGetWidth()/4,3*ofGetHeight()/4));
        
    }
    
    velocityIterations = 6;
	positionIterations = 2;
	timeStep = 1.0f/120.0f;
	m_stepCount = 0;
    m_mouseJoint = NULL;
    
    //coordinator.setup(ofGetWidth(), ofGetHeight(), ofPoint(ofGetWidth()/2,ofGetHeight()), 20);
    
    ofBackground(0);
    
    ofEnableSmoothing();
	
	// show a faint the non-curve version of the same polygon:
	ofEnableAlphaBlending();
    ofSetColor(150);
    
    bDrawDebug = false;
}

//--------------------------------------------------------------
void testApp::update(){
    m_world->Step(timeStep, velocityIterations, positionIterations);
	
	if (timeStep > 0.0f)
	{
		++m_stepCount;
	}
	
	m_world->ClearForces();
}

//--------------------------------------------------------------
void testApp::draw(){
    
   
    
    
    ofPushMatrix();
    ofScale(pixelsToMeter, pixelsToMeter);
    
    if (bDrawDebug) {
        m_world->DrawDebugData();
        
        if (m_mouseJoint)
        {
            b2Vec2 p1 = m_mouseJoint->GetAnchorB();
            b2Vec2 p2 = m_mouseJoint->GetTarget();
            
            b2Color c;
            c.Set(0.0f, 1.0f, 0.0f);
            m_debugDraw.DrawPoint(p1, 4.0f, c);
            m_debugDraw.DrawPoint(p2, 4.0f, c);
            
            c.Set(0.8f, 0.8f, 0.8f);
            m_debugDraw.DrawSegment(p1, p2, c);
        }
    }
    
    
    
    
    
    
    
    ofSetColor(150);
    
    for (vector<pair<wstring,b2Body*> >::iterator iter=words.begin(); iter!=words.end(); iter++) {
        
        
        
        const b2Transform& xf = iter->second->GetTransform();
        
        //font.drawString(iter->first, (ofGetWidth()-messageWidth)/2, (ofGetHeight()-messageHeight)/2);
        ofPushMatrix();
        ofTranslate(xf.p.x, xf.p.y);
        ofScale(1.0/pixelsToMeter, 1.0/pixelsToMeter);
        ofRotate(xf.q.GetAngle()*180/M_PI);
        
        float messageWidth = font.stringWidth(iter->first);
        float messageHeight = font.stringHeight(iter->first);
        font.drawString(iter->first, -messageWidth/2,messageHeight/2 );
        ofPopMatrix();
        
    }
    
    ofPopMatrix();
    /*
    coordinator.pushTransform();
	
	
        
	
	for (vector<b2Body*>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
		
        b2Color color(0.5f, 0.0f, 0.0f);
        b2Fixture *fixture = (*iter)->GetFixtureList();
        b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
        int32 vertexCount = poly->m_count;
        b2Assert(vertexCount <= b2_maxPolygonVertices);
        b2Vec2 vertices[b2_maxPolygonVertices];
        
        const b2Transform& xf = (*iter)->GetTransform();
        
        for (int32 i = 0; i < vertexCount; ++i)
        {
            vertices[i] = b2Mul(xf, poly->m_vertices[i]);
        }
        
        //	glEnable(GL_BLEND);
        //	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor3f(color.r, color.g,  color.b);
        //glColor3f(0.5f * color.r, 0.5f * color.g, 0.5f * color.b);
        glBegin(GL_TRIANGLE_FAN);
        for (int32 i = 0; i < vertexCount; ++i)
        {
            glVertex2f(vertices[i].x, vertices[i].y);
        }
        glEnd();
	}
    
    coordinator.popTransform();
     */

}

class QueryCallback : public b2QueryCallback
{
public:
	QueryCallback(const b2Vec2& point)
	{
		m_point = point;
		m_fixture = NULL;
	}
    
	bool ReportFixture(b2Fixture* fixture)
	{
		b2Body* body = fixture->GetBody();
		if (body->GetType() == b2_dynamicBody)
		{
			bool inside = fixture->TestPoint(m_point);
			if (inside)
			{
				m_fixture = fixture;
                
				// We are done, terminate the query.
				return false;
			}
		}
        
		// Continue the query.
		return true;
	}
    
	b2Vec2 m_point;
	b2Fixture* m_fixture;
};


void testApp::MouseDown(const b2Vec2& p)
{
	m_mouseWorld = p;
	
	if (m_mouseJoint != NULL)
	{
		return;
	}
    
	// Make a small box.
	b2AABB aabb;
	b2Vec2 d;
	d.Set(0.001f, 0.001f);
	aabb.lowerBound = p - d;
	aabb.upperBound = p + d;
    
	// Query the world for overlapping shapes.
	QueryCallback callback(p);
	m_world->QueryAABB(&callback, aabb);
    
	if (callback.m_fixture)
	{
		b2Body* body = callback.m_fixture->GetBody();
		b2MouseJointDef md;
		md.bodyA = m_groundBody;
		md.bodyB = body;
		md.target = p;
		md.maxForce = 1000.0f * body->GetMass();
		m_mouseJoint = (b2MouseJoint*)m_world->CreateJoint(&md);
		body->SetAwake(true);
	}
}





void testApp::MouseUp(const b2Vec2& p)
{
	if (m_mouseJoint)
	{
		m_world->DestroyJoint(m_mouseJoint);
		m_mouseJoint = NULL;
	}
	
	
}

void testApp::MouseMove(const b2Vec2& p)
{
	m_mouseWorld = p;
	
	if (m_mouseJoint)
	{
		m_mouseJoint->SetTarget(p);
	}
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key) {
        case 'd':
            bDrawDebug =!bDrawDebug;
            break;
            
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    MouseMove(b2Vec2(x/pixelsToMeter,y/pixelsToMeter));
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    MouseDown(b2Vec2(x/pixelsToMeter,y/pixelsToMeter));
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    MouseUp(b2Vec2(x/pixelsToMeter,y/pixelsToMeter));
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}