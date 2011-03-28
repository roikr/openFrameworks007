#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	m_world.SetDebugDraw(&m_debugDraw);
	
	uint32 flags = b2DebugDraw::e_shapeBit | b2DebugDraw::e_jointBit | b2DebugDraw::e_centerOfMassBit ;
	
//	flags += settings->drawJoints			* b2DebugDraw::e_jointBit;
//	flags += settings->drawAABBs			* b2DebugDraw::e_aabbBit;
//	flags += settings->drawPairs			* b2DebugDraw::e_pairBit;
//	flags += settings->drawCOMs				* b2DebugDraw::e_centerOfMassBit;
	m_debugDraw.SetFlags(flags);
	
	
	b2PolygonShape shape;
	shape.SetAsEdge(b2Vec2(-40.0f, 0.0f), b2Vec2(40.0f, 0.0f));
	
	b2BodyDef bd;
	m_ground = m_world.CreateBody(&bd);
	m_ground->CreateFixture(&shape, 0.0f);
	
	
	b2BodyDef teeterDef;
	teeterDef.type = b2_dynamicBody;
	teeterDef.position.Set(0.0f,2.5f);
	m_teeter = m_world.CreateBody(&teeterDef);
	
	b2PolygonShape teeterBox;
	teeterBox.SetAsEdge(b2Vec2(-10.0f, 0.0f), b2Vec2(10.0f, 0.0f));
	
	b2FixtureDef teeterFixtureDef;
	teeterFixtureDef.shape = &teeterBox;
	teeterFixtureDef.density = 1.0f;
	teeterFixtureDef.friction = 1.0f;
	m_teeter->CreateFixture(&teeterFixtureDef);
	
	
	b2RevoluteJointDef jointDef;
	jointDef.collideConnected = true;
	jointDef.Initialize(m_ground,m_teeter,b2Vec2(0.0f,2.5f)); // teeter->GetWorldCenter()
	
	m_world.CreateJoint(&jointDef);
	
	
	shape.SetAsBox(2.0f, 2.0f, b2Vec2(-8.0f, 2.0f), 0.0f);
	m_fixture1 = m_teeter->CreateFixture(&shape, 5.0f);
	
	m_distance = 8.0f;
	shape.SetAsBox(2.0f, 2.0f, b2Vec2(m_distance, 2.0f), 0.0f);
	m_fixture2 = m_teeter->CreateFixture(&shape, 5.0f);
	
	
	
		
	
	ofSetFrameRate(60);
	
	velocityIterations = 6;
	positionIterations = 2;
	timeStep = 1.0f/60.0f;
	
	coordinator.setup(ofGetWidth(), ofGetHeight(), ofPoint(ofGetWidth()/2,ofGetHeight()), 10);

	
	
	ofBackground(0, 0, 0);
	
}

//--------------------------------------------------------------
void testApp::update(){
	m_world.Step(timeStep, velocityIterations, positionIterations);
	m_world.ClearForces();
//	b2Vec2 position = body->GetPosition();
//	float angle = body->GetAngle();
//	printf("%4.2f %4.2f %4.2f\n",position.x,position.y,angle);
	
}

//--------------------------------------------------------------
void testApp::draw(){
	ofEnableSmoothing();
//	ofPushMatrix();
//	ofTranslate(ofGetWidth()/2, ofGetHeight(), 0);
//	float scale = 1;
//	ofScale(scale, -scale, 1.0);
	coordinator.pushTransform();
	m_world.DrawDebugData();
	coordinator.popTransform();
//	ofPopMatrix();
	ofDisableSmoothing();
	
	ofSetColor(255, 255, 255);
	std::ostringstream ss;
	
	ss << ofGetFrameRate();

	
	ofDrawBitmapString(ss.str(), 20, 20);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch (key)
	{
		case 'q': {
			m_distance-=0.25;
			m_teeter->DestroyFixture(m_fixture2);
			b2PolygonShape shape;
			shape.SetAsBox(2.0f, 2.0f, b2Vec2(m_distance, 2.0f), 0.0f);
			m_fixture2 = m_teeter->CreateFixture(&shape, 5.0f);
			m_teeter->SetAwake(true);
		} break;
			
		case 'w': {
			m_distance+=0.25;
			m_teeter->DestroyFixture(m_fixture2);
			b2PolygonShape shape;
			shape.SetAsBox(2.0f, 2.0f, b2Vec2(m_distance, 2.0f), 0.0f);
			m_fixture2 = m_teeter->CreateFixture(&shape, 5.0f);
			m_teeter->SetAwake(true);
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

