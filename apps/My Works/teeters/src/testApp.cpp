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
	
	
			
		
		b2BodyDef bd;
		//bd.type = b2_dynamicBody;
		ground = m_world.CreateBody(&bd);
		
		b2PolygonShape groundShape;
		groundShape.SetAsEdge(b2Vec2(-40.0f, 0.0f), b2Vec2(40.0f, 0.0f));
		ground->CreateFixture(&groundShape,1.0f);
		
		float32 nextt = 4.0f;
		float32 nextp = nextt/2.0f;
		b2Vec2 p1(-nextp/2.0f,0.0f);
		b2Vec2 p2(nextp/2.0f,0.0f);
		b2Vec2 p3(0.0f, 1.5*nextt-(nextp/10.f));
		
		b2Vec2 vertices[3];
		
		vertices[0] = p1;
		vertices[1] = p2;
		vertices[2] = p3;
		
		b2PolygonShape baseShape;
		baseShape.Set(vertices, 3);
		baseFixture = ground->CreateFixture(&baseShape,1.0f);
	
		
	Teeter *teeter = new Teeter(&m_world,4.0f,b2Vec2(0.0f,0.0f),ground,false,3.0f);
	teeters.push_back(teeter);
	teeter = new Teeter(&m_world,3.0f,teeter->getNextPosition(),teeter->getTeeterBody(),false,2.0f);
	teeters.push_back(teeter);
	teeter = new Teeter(&m_world,2.0f,teeter->getNextPosition(),teeter->getTeeterBody(),true,0.0f);
	teeters.push_back(teeter);
	
	

	current = teeters.end()-1;
	
	ofSetFrameRate(60);
	
	velocityIterations = 6;
	positionIterations = 2;
	timeStep = 1.0f/60.0f;
	m_stepCount = 0;
	bias = 1.0f;
	
	coordinator.setup(ofGetWidth(), ofGetHeight(), ofPoint(ofGetWidth()/2,ofGetHeight()), 20);

	
	bTrans = false;
	ofBackground(0, 0, 0);
	
	(*current)->setFocus(0.0f);
	
}

//--------------------------------------------------------------
void testApp::update(){
	m_world.Step(timeStep, velocityIterations, positionIterations);
	
	if (timeStep > 0.0f)
	{
		++m_stepCount;
	}
	
	
	(*current)->update();
	
	m_world.ClearForces();
	
	//m_debugDraw.DrawString(5, m_textLine, "stepCount: %d, speed: %3.1f down: %d",m_stepCount-current->stepCount,current->joint->GetJointSpeed(),current->bDown ? 1 : 0);
	
//	b2Vec2 position = body->GetPosition();
//	float angle = body->GetAngle();
//	printf("%4.2f %4.2f %4.2f\n",position.x,position.y,angle);
	
	if (bTrans) {
		float t = (float)(ofGetElapsedTimeMillis() - animStart)/1000.0;
		if (t >= 1) {
			
			bTrans = false;
		} else {
			
			b2Vec2 npos;
			float32 nscale;
			(*current)->getTransform(npos,nscale);
			scale = easeInOutQuad(t,scale,nscale);
			position.x = easeInOutQuad(t,position.x,npos.x);
			position.y = easeInOutQuad(t,position.y,npos.y);		}
		
	}
	
}



void testApp::DrawShape(b2Fixture* fixture, const b2Transform& xf)
{
	
	b2Color color(0.6f, 0.6f, 0.6f);
	b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
	int32 vertexCount = poly->m_vertexCount;
	b2Assert(vertexCount <= b2_maxPolygonVertices);
	b2Vec2 vertices[b2_maxPolygonVertices];
	
	for (int32 i = 0; i < vertexCount; ++i)
	{
		vertices[i] = b2Mul(xf, poly->m_vertices[i]);
	}
	
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
	glBegin(GL_TRIANGLE_FAN);
	for (int32 i = 0; i < vertexCount; ++i)
	{
		glVertex2f(vertices[i].x, vertices[i].y);
	}
	glEnd();
	glDisable(GL_BLEND);
	
	glColor4f(color.r, color.g, color.b, 1.0f);
	glBegin(GL_LINE_LOOP);
	for (int32 i = 0; i < vertexCount; ++i)
	{
		glVertex2f(vertices[i].x, vertices[i].y);
	}
	glEnd();
	
}

//--------------------------------------------------------------
void testApp::draw(){
	
//	ofPushMatrix();
//	ofTranslate(ofGetWidth()/2, ofGetHeight(), 0);
//	float scale = 1;
//	ofScale(scale, -scale, 1.0);
	coordinator.pushTransform();
	
	if (bTrans) {
		ofScale(scale, scale, 1.0f);
		ofTranslate(-position.x, -position.y, 0.0f);
	} else {
		(*current)->transform();
	}

	
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);	
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	
	//why do we need this?
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
//	m_world.DrawDebugData();
	
	DrawShape(baseFixture,ground->GetTransform());
	
	
	for (vector<Teeter*>::iterator iter=teeters.begin(); iter!=teeters.end(); iter++) {
		(*iter)->draw();
	}
	
	glPopAttrib();
	coordinator.popTransform();
//	ofPopMatrix();
	
	
	ofSetColor(255, 255, 255);
	std::ostringstream ss;
	
	ss << ofGetFrameRate() << " " << m_stepCount << endl;
	
	(*current)->log(ss);
		
	ofDrawBitmapString(ss.str(), 20, 20);
}

// Implement contact listener.
void testApp::BeginContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
	
	
	if (fixtureA->GetUserData() && fixtureB->GetUserData())
	{
		if ((*current)->getJoint() == fixtureA->GetUserData() || (*current)->getJoint() == fixtureB->GetUserData() ) {
			(*current)->setState(TEETER_STATE_TOUCHING);
			cout << "contact" << endl;
		}
	}
	
}

void testApp::EndContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
	
	
	if (fixtureA->GetUserData() && fixtureB->GetUserData())
	{
		if ((*current)->getJoint() == fixtureA->GetUserData() || (*current)->getJoint() == fixtureB->GetUserData() ) {
			(*current)->setState(TEETER_STATE_UNBALLANCED);
			cout << "no contact" << endl;
		}
	}
	
}

void testApp::nextTeeter() {
	if (current!=teeters.begin()) {
		(*current)->getTransform(position,scale);
		current--;
		(*current)->setFocus(0.0f);
		animStart = ofGetElapsedTimeMillis();
		bTrans = true;
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
			nextTeeter();
			break;

			
		case 's':
			bias =1.0f;
			(*current)->start();
			
			break;
			
		case 'j': {
			for (vector<Teeter*>::iterator iter=current; iter!=teeters.end(); iter++) {
				(*iter)->breakTeeter();
			}
//			int32 stage = distance(teeters.begin(),current);
//			(*current)->getBody()->ApplyTorque(-50000.0f*(teeters.size()-stage));
			(*current)->jump();
			
		} break;

		
			
		case OF_KEY_LEFT: {
			bias-=0.05f;
			(*current)->displace(bias);
			
		} break;
			
		case OF_KEY_RIGHT: {
			bias+=0.05f;
			(*current)->displace(bias);
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

