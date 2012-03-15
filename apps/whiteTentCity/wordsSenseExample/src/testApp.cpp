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


// Implement contact listener.
void testApp::BeginContact(b2Contact* contact)
{
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();
    
    if (fixtureA->IsSensor() && !(fixtureB->IsSensor()))
    {
        sensors[fixtureA]->bodies.insert(fixtureB->GetBody());
        
    }
    
    if (fixtureB->IsSensor() && !(fixtureA->IsSensor()))
    {
        sensors[fixtureB]->bodies.insert(fixtureA->GetBody());
    }
    
    
}

// Implement contact listener.
void testApp::EndContact(b2Contact* contact)
{
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();
    
    if (fixtureA->IsSensor() && !(fixtureB->IsSensor()))
    {
        sensors[fixtureA]->bodies.erase(fixtureB->GetBody());
        
    }
    
    if (fixtureB->IsSensor() && !(fixtureA->IsSensor()))
    {
        sensors[fixtureB]->bodies.erase(fixtureA->GetBody());
    }
}


void testApp::addWord(wstring str) {
    
    
    
    word w;
    w.str = str;
    int width = font.stringWidth(str);
    int height = font.stringHeight(str);
    
    vector<word>::iterator iter;
    
    do {
        w.rect = ofRectangle(ofRandom(ofGetWidth()-width),ofRandom(ofGetHeight()-height), width, height);
       
        
        
        for (iter=words.begin(); iter!=words.end(); iter++) {
            
            if (!( w.rect.x+w.rect.width<iter->rect.x ||  w.rect.x>iter->rect.x+iter->rect.width || 
                  w.rect.y+w.rect.height < iter->rect.y || w.rect.y>iter->rect.y+iter->rect.height)){
                break;
            }
        }
        
        
    } while (iter!=words.end());
    
    
    
    b2PolygonShape shape;
    shape.SetAsBox(width/pixelsToMeter/2.0, height/pixelsToMeter/2.0);
    
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position =  b2Vec2((w.rect.x+w.rect.width/2)/pixelsToMeter,(w.rect.y+w.rect.height/2)/pixelsToMeter);
    
    w.body = m_world->CreateBody(&bd);
    w.body->CreateFixture(&shape, 5.0f);
    
    words.push_back(w);
}


//--------------------------------------------------------------
void testApp::setup(){
    
    receiver = new ofxTSPSReceiver();
	receiver->setListener( this );
	receiver->connect(PORT);
    
    //font.loadFont("Guttman Hatzvi-Normal.ttf", 50,true,true);
	font.loadFont("ThonburiBold.ttf", 30,true,true);
    
    {
        pixelsToMeter = 100;
        
        m_world = new b2World(b2Vec2(0.0f,0.0f)); 
        
        m_world->SetDestructionListener(this);
        m_world->SetContactListener(this);
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
        /*
		addWord(L"םעה",ofVec2f(3*ofGetWidth()/4,ofGetHeight()/4));
        addWord(L"שרוד",ofVec2f(ofGetWidth()/4,ofGetHeight()/4));
        addWord(L"קדצ",ofVec2f(3*ofGetWidth()/4,3*ofGetHeight()/4));
        addWord(L"יתרבח",ofVec2f(ofGetWidth()/4,3*ofGetHeight()/4));
		*/
        
        string wordsArray[] = {"protesters","October","police","City","protests","Occupy movement","Wall Street","Madrid","Berlin","London","New York","Income disparity","increase","Budget Office","Occupy Wall","Congressional Budget","Wall Street","income","income of the top","rich","poor","network","democracy","revolution","spring","together","demand","wake","rise","money","housing","costs","education"};
        vector<string> wordsVec(wordsArray,wordsArray+33);
        for (vector<string>::iterator iter=wordsVec.begin();iter!=wordsVec.end();iter++) {
            std::wstring temp(iter->length(),L' ');
            std::copy(iter->begin(), iter->end(), temp.begin());
            addWord(temp);
        }
        
		
        
    }
    
    velocityIterations = 6;
	positionIterations = 2;
	timeStep = 1.0f/30.0f;
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

b2Fixture* testApp::createSensor(ofxTSPSPerson* person) {
    vector<b2Vec2> vs;
    for(int c = 0; c < person->contour.size(); c++){
        vs.push_back(b2Vec2(person->contour[c].x*ofGetWidth()/pixelsToMeter,person->contour[c].y*ofGetHeight()/pixelsToMeter));
    }
    
    b2ChainShape loop;
    loop.CreateLoop(vs.data(), vs.size());
    b2FixtureDef fd;
    fd.shape = &loop;
    fd.isSensor = true;
    
    return m_groundBody->CreateFixture(&fd);
}

//--------------------------------------------------------------
void testApp::update(){
    
/*
    for(int i = 0; i < receiver->totalPeople(); i++){
		ofxTSPSPerson* person = receiver->personAtIndex(i);
		ofColor* color = (ofColor*)person->customAttributes;
		ofSetColor(color->r, color->g, color->b);
		for(int c = 1; c < person->contour.size(); c++){
			ofLine(person->contour[c-1].x*ofGetWidth(), person->contour[c-1].y*ofGetHeight(), 
				   person->contour[c  ].x*ofGetWidth(), person->contour[c  ].y*ofGetHeight());
		}
    }
*/
    
                
        
        
    
 
    
    
    m_world->Step(timeStep, velocityIterations, positionIterations);
	
	if (timeStep > 0.0f)
	{
		++m_stepCount;
	}
    
    	
	m_world->ClearForces();
    
    for (map<b2Fixture*,sensor*>::iterator siter=sensors.begin();siter!=sensors.end();siter++) {
        for (set<b2Body*>::iterator biter=siter->second->bodies.begin(); biter!=siter->second->bodies.end(); biter++) {
            b2Vec2 d(siter->second->person->velocity.x,siter->second->person->velocity.y);
            
            //d.Normalize();
            b2Vec2 F = 10.0f * d;
            (*biter)->ApplyForce(F, (*biter)->GetPosition());
        }
    }
    
    

    
    
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
    
    for (vector<word>::iterator iter=words.begin(); iter!=words.end(); iter++) {
        
        
        
        const b2Transform& xf = iter->body->GetTransform();
        
        //font.drawString(iter->first, (ofGetWidth()-messageWidth)/2, (ofGetHeight()-messageHeight)/2);
        ofPushMatrix();
        ofTranslate(xf.p.x, xf.p.y);
        ofScale(1.0/pixelsToMeter, 1.0/pixelsToMeter);
        ofRotate(xf.q.GetAngle()*180/M_PI);
        
        float messageWidth = font.stringWidth(iter->str);
        float messageHeight = font.stringHeight(iter->str);
        font.drawString(iter->str, -messageWidth/2,messageHeight/2 );
        ofPopMatrix();
        
    }
    
    ofPopMatrix();
    
//    for(int i = 0; i < receiver->totalPeople(); i++){
//		ofxTSPSPerson* person = receiver->personAtIndex(i);
//		ofColor* color = (ofColor*)person->customAttributes;
//		ofSetColor(color->r, color->g, color->b);
//		for(int c = 1; c < person->contour.size(); c++){
//			ofLine(person->contour[c-1].x*ofGetWidth(), person->contour[c-1].y*ofGetHeight(), 
//				   person->contour[c  ].x*ofGetWidth(), person->contour[c  ].y*ofGetHeight());
//		}
//    }
    
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


//called when the person enters the system
void testApp::personEntered( ofxTSPSPerson* person, ofxTSPSScene* scene )
{
	ofColor* color = new ofColor();
	color->r = ofRandom(0, 255);
	color->g = ofRandom(0, 255);
	color->b = ofRandom(0, 255);
	color->a = 255;
	
	//put a color into the custom attributes field
	person->customAttributes = color;
    
    sensor* s= new sensor;
    s->person = person;
    sensors[createSensor(person)] = s;
        
	
}

//called each time the centroid moves (a lot)
void testApp::personMoved( ofxTSPSPerson* person, ofxTSPSScene* scene )
{
    for (map<b2Fixture*,sensor*>::iterator iter=sensors.begin(); iter!=sensors.end(); iter++) {
        if (iter->second->person==person) {
            m_groundBody->DestroyFixture(iter->first);
            sensor* s= iter->second;
            sensors.erase(iter);
            sensors[createSensor(person)] = s;
            break;
        }
    }
}

//called one frame before the person is removed from the list to let you clean up
void testApp::personWillLeave( ofxTSPSPerson* person, ofxTSPSScene* scene )
{
	//delete the color so we free up memory.
	delete (ofColor*)person->customAttributes;
    for (map<b2Fixture*,sensor*>::iterator iter=sensors.begin(); iter!=sensors.end(); iter++) {
        if (iter->second->person==person) {
            m_groundBody->DestroyFixture(iter->first);
            sensor* s= iter->second;
            sensors.erase(iter);
            delete s;
            break;
        }
    }
    
    
}

//called every frame no matter what.
void testApp::personUpdated(ofxTSPSPerson* person, ofxTSPSScene* scene)
{
    
    
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