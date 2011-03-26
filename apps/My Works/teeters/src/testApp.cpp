#include "testApp.h"
#include "easing.h"
//--------------------------------------------------------------
void testApp::setup(){
	
	m_world.SetContactListener(this);
	m_world.SetDebugDraw(&m_debugDraw);
	
	uint32 flags = b2DebugDraw::e_shapeBit | b2DebugDraw::e_jointBit | b2DebugDraw::e_centerOfMassBit ;
	
//	flags += settings->drawJoints			* b2DebugDraw::e_jointBit;
//	flags += settings->drawAABBs			* b2DebugDraw::e_aabbBit;
//	flags += settings->drawPairs			* b2DebugDraw::e_pairBit;
//	flags += settings->drawCOMs				* b2DebugDraw::e_centerOfMassBit;
	m_debugDraw.SetFlags(flags);
	
	
	b2Body* ground = NULL;
	{
		
		
		b2BodyDef bd;
		//bd.type = b2_dynamicBody;
		ground = m_world.CreateBody(&bd);
		
		b2PolygonShape groundShape;
		groundShape.SetAsEdge(b2Vec2(-40.0f, 0.0f), b2Vec2(40.0f, 0.0f));
		
		b2FixtureDef groundFixtureDef;
		groundFixtureDef.shape=&groundShape;
		groundFixtureDef.filter.categoryBits = 0x0007;
		groundFixtureDef.filter.maskBits = 0x0007;
		ground->CreateFixture(&groundFixtureDef);
		
		
		
	}
	
	
	Teeter *teeter = new Teeter(&m_world,4.0f,ground,b2Vec2(0.0f,0.0f));
	teeters.push_back(teeter);
	
	teeter = new Teeter(&m_world,3.0f,teeter->teeter,b2Vec2(-10.0f,4.0f));
	teeters.push_back(teeter);
	
	teeter = new Teeter(&m_world,2.0f,teeter->teeter,b2Vec2(-15.0f,7.0f),true);
	teeters.push_back(teeter);

	current = teeters.begin();
	
	ofSetFrameRate(60);
	
	velocityIterations = 6;
	positionIterations = 2;
	timeStep = 1.0f/60.0f;
	m_stepCount = 0;
	
	coordinator.setup(ofGetWidth(), ofGetHeight(), ofPoint(ofGetWidth()/2,ofGetHeight()), 20);

	
	bTrans = false;
	ofBackground(0, 0, 0);
	
}

//--------------------------------------------------------------
void testApp::update(){
	m_world.Step(timeStep, velocityIterations, positionIterations);
	
	if (timeStep > 0.0f)
	{
		++m_stepCount;
	}
	
	for (vector<Teeter*>::iterator iter = teeters.begin();iter!=teeters.end();iter++) {
		(*iter)->update(m_stepCount);
	}
	m_world.ClearForces();
	
	//m_debugDraw.DrawString(5, m_textLine, "stepCount: %d, speed: %3.1f down: %d",m_stepCount-current->stepCount,current->joint->GetJointSpeed(),current->bDown ? 1 : 0);
	
//	b2Vec2 position = body->GetPosition();
//	float angle = body->GetAngle();
//	printf("%4.2f %4.2f %4.2f\n",position.x,position.y,angle);
	
	if (bTrans) {
		float t = (float)(ofGetElapsedTimeMillis() - animStart)/1000.0;
		if (t >= 1) {
			current=current-1;
			bTrans = false;
		} else {
			
			
			scale = easeInOutQuad(t,scale,(*(current-1))->scale);
			position.x = easeInOutQuad(t,position.x,(*(current-1))->position.x);
			position.y = easeInOutQuad(t,position.y,(*(current-1))->position.y);
		}
		
	}
	
}

//--------------------------------------------------------------
void testApp::draw(){
	ofEnableSmoothing();
//	ofPushMatrix();
//	ofTranslate(ofGetWidth()/2, ofGetHeight(), 0);
//	float scale = 1;
//	ofScale(scale, -scale, 1.0);
	coordinator.pushTransform();
	
	if (bTrans) {
		ofScale(scale, scale, 1.0f);
		ofTranslate(-position.x, -position.y, 0.0f);
	} else {
		ofScale((*current)->scale, (*current)->scale, 1.0f);
		ofTranslate(-(*current)->position.x, -(*current)->position.y, 0.0f);
	}

	
	
	
	m_world.DrawDebugData();
	coordinator.popTransform();
//	ofPopMatrix();
	ofDisableSmoothing();
	
	ofSetColor(255, 255, 255);
	std::ostringstream ss;
	
	ss << ofGetFrameRate() << " " << m_stepCount << endl;
	ss << (*current)->stepCount << endl;

	
	ofDrawBitmapString(ss.str(), 20, 20);
}

// Implement contact listener.
void testApp::BeginContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
	
	
	if (fixtureA->GetUserData() && fixtureB->GetUserData())
	{
		for (vector<Teeter*>::iterator iter = teeters.begin();iter!=teeters.end();iter++) {
			if (((*iter)->joint == fixtureA->GetUserData() || (*iter)->joint == fixtureB->GetUserData()) && !(*iter)->joint->IsLimitEnabled()) {
				(*iter)->bDown = true;
				cout << "contact" << endl;
				break;
			}
		}
	}
	
}

void testApp::EndContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
	
	
	
	if (fixtureA->GetUserData() && fixtureB->GetUserData())
	{
		for (vector<Teeter*>::iterator iter = teeters.begin();iter!=teeters.end();iter++) {
			if (((*iter)->joint == fixtureA->GetUserData() || (*iter)->joint == fixtureB->GetUserData()) && !(*iter)->joint->IsLimitEnabled()) {
				(*iter)->bDown = false;
				cout << "no contact" << endl;
				break;
			}
		}
	}
	
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch (key)
	{
		case '1':
			current = teeters.begin();
			break;
		case '2':
			current = teeters.begin()+1;
			break;
		case '3':
			current = teeters.begin()+2;
			break;
			
		case 'n':
			if (current!=teeters.begin()) {
				scale = (*current)->scale;
				position = (*current)->position;
				animStart = ofGetElapsedTimeMillis();
				bTrans = true;
			}
			break;

			
		case 't':
			(*current)->joint->EnableLimit(!(*current)->joint->IsLimitEnabled());
			(*current)->stepCount = m_stepCount;
			
			break;
		case 'e':
			(*current)->joint->EnableLimit(false);
			//current->teeter->ApplyTorque(100);
			break;
			
		case 'q': {
			(*current)->bias-=0.05f;
			(*current)->displace();
			
		} break;
			
		case 'w': {
			(*current)->bias+=0.05f;
			(*current)->displace();
		} break;
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

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	ofPoint world = coordinator.screenSpaceToWorldSpace(ofPoint(x,y));
	cout << world.x << "," << world.y << endl;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

