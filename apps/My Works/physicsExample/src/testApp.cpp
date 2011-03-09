#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	world.SetDebugDraw(&m_debugDraw);
	
	uint32 flags = b2DebugDraw::e_shapeBit | b2DebugDraw::e_jointBit | b2DebugDraw::e_centerOfMassBit ;
	
	//flags += settings->drawJoints			* b2DebugDraw::e_jointBit;
//	flags += settings->drawAABBs			* b2DebugDraw::e_aabbBit;
//	flags += settings->drawPairs			* b2DebugDraw::e_pairBit;
//	flags += settings->drawCOMs				* b2DebugDraw::e_centerOfMassBit;
	m_debugDraw.SetFlags(flags);
	
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f,-10.0f);
	groundBody = world.CreateBody(&groundBodyDef);
	b2PolygonShape groundBox;
	groundBox.SetAsBox(ofGetWidth()*3/2,10.0f);
	groundBody->CreateFixture(&groundBox,1.0f);
	
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(-100.0f,150.0f);
	body = world.CreateBody(&bodyDef);
	
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(5.0f, 5.0f);
	
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.5f;
	fixtureDef.restitution = 0.2f;
	body->CreateFixture(&fixtureDef);
	body->SetLinearVelocity(b2Vec2(0.0f,-250.0f));
	
	b2BodyDef teeterDef;
	teeterDef.type = b2_dynamicBody;
	teeterDef.position.Set(0.0f,10.0f);
	teeter = world.CreateBody(&teeterDef);
	
	b2PolygonShape teeterBox;
	teeterBox.SetAsBox(150, 2);
	
	b2FixtureDef teeterFixtureDef;
	teeterFixtureDef.shape = &teeterBox;
	teeterFixtureDef.density = 1.0f;
	teeterFixtureDef.friction = 0.0f;
	teeter->CreateFixture(&teeterFixtureDef);
	
	b2RevoluteJointDef jointDef;
	jointDef.collideConnected = true;
	jointDef.Initialize(groundBody,teeter,teeter->GetWorldCenter());
	
	world.CreateJoint(&jointDef);
	
	
	
	ofSetFrameRate(60);
	
	velocityIterations = 6;
	positionIterations = 2;
	timeStep = 1.0f/60.0f;
	
	coordinator.setup(ofGetWidth(), ofGetHeight(), ofPoint(ofGetWidth()/2,ofGetHeight()), 1.5);

	
	
	ofBackground(0, 0, 0);
	
}

//--------------------------------------------------------------
void testApp::update(){
	world.Step(timeStep, velocityIterations, positionIterations);
	world.ClearForces();
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
	world.DrawDebugData();
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

