/*
 *  Teeter.cpp
 *  teeterTotter
 *
 *  Created by Roee Kremer on 3/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Teeter.h"

enum {
	LOOP_IDLE,
	LOOP_FORWARD,
	LOOP_BACKWARD
};


Teeter::Teeter(b2World *m_world,float32 t,b2Vec2 position,b2Body *parent,bool bLeaf,float32 nextt) {
	this->t = t;
	p =t/2;
	this->position = position;
	this->bLeaf = bLeaf;
	this->m_world = m_world;
	
	nextPosition = b2Vec2(0.5*-5.0f*t, p/10);
	
	b2BodyDef teeterDef;
	teeterDef.type = b2_dynamicBody;
	teeterDef.position =  position+b2Vec2(0.0f, 1.5f*t); // raise the teeter
	teeter = m_world->CreateBody(&teeterDef);
	
	b2PolygonShape teeterShape;
	//teeterShape.SetAsEdge(b2Vec2(-t*5.0f, 0.0f), b2Vec2(t*5.0f, 0.0f));
	teeterShape.SetAsBox(t*5.0f, p/10, b2Vec2(0.0f, 0.0f),0.0f);
	
	b2FixtureDef teeterFixtureDef;
	teeterFixtureDef.shape = &teeterShape;
	teeterFixtureDef.density = 1.0f; // thin teeter

	teeterFixture = teeter->CreateFixture(&teeterFixtureDef);
	
	b2RevoluteJointDef jointDef;
	
	
	jointDef.Initialize(parent ,teeter,position+b2Vec2(0.0f,1.5*t)); 
	
	jointDef.lowerAngle = 0.0f * b2_pi;
	jointDef.upperAngle = 0.0f * b2_pi;
	jointDef.enableLimit = true;
	jointDef.collideConnected = 1;
	//jointDef.maxMotorTorque = 1000.0f;
	//jointDef.motorSpeed = 0.0f;
	jointDef.enableMotor = false;
	joint = (b2RevoluteJoint*) m_world->CreateJoint(&jointDef);
	
	teeterFixture->SetUserData(joint);
	
	bias = 0.0f;
	
	playerShape.SetAsBox(2*p/3.0f, p, b2Vec2(5.0f*t-2*p/3.0f, p+p/10), 0.0f);
	playerFixture = 0;
	
	leafImage = 0;
	
	this->bLeaf = bLeaf;
	
	
	if (bLeaf) {
		
		payloadShape.SetAsBox(2*p/3.0f, p, b2Vec2(-5.0f*t+2*p/3.0f, p+p/10), 0.0f);
		leafImage = new ofImage();
		leafImage->loadImage("weight.png");
	} else {
		float32 nextp = nextt/2.0f;
		b2Vec2 pos(0.5f*-5.0f*t+2*p/3.0f, p+p/10);	
		
		b2Vec2 p1(-nextp/2.0f,0.0f);
		b2Vec2 p2(nextp/2.0f,0.0f);
		b2Vec2 p3(0.0f, 1.5*nextt-(nextp/10.f));
		
		b2Vec2 vertices[3];
		
		vertices[0] = p1+nextPosition;
		vertices[1] = p2+nextPosition;
		vertices[2] = p3+nextPosition;
		
		payloadShape.Set(vertices, 3);
	}
	
	payloadFixture = teeter->CreateFixture(&payloadShape,1.0f);
	

	
	scale = 4.0f/t;
	state = TEETER_STATE_LOCKED;
	
	for (int i=0; i<10; i++) {
		speeds.push_back(0.0f);
	}
	
	siter=speeds.begin();
	
	
	for (int j=0;j<60;j++) {
		blobs.push_back(new ofxCvBlob);
	}
	cbiter = blobs.begin();
	
	loopState = LOOP_IDLE;
	
}

b2Vec2 Teeter::getNextPosition() {
	return teeter->GetWorldPoint(nextPosition);
}


void Teeter::setFocus(float centerX) {
	state = TEETER_STATE_RESTING;
	joint->EnableLimit(false);
	
	this->centerX = centerX;
}

void Teeter::start() {
	state = TEETER_STATE_STARTED;
	playerFixture = teeter->CreateFixture(&playerShape,1.0f);
	joint->EnableMotor(true);
	joint->SetMaxMotorTorque(10000);
	//	centerBlob = blob;
	
	for (vector<float32>::iterator iter=speeds.begin();iter!=speeds.end();iter++) {
		*iter = 0.0f; 
	}
}



void Teeter::update() {
	
	// Cache velocities to improve movement on breakage.
	if (state!=TEETER_STATE_BROKEN)
	{
		m_velocity = teeter->GetLinearVelocity();
		m_angularVelocity = teeter->GetAngularVelocity();
	}
	
	if (joint) {
		
		(*siter) = joint->GetJointSpeed()*180.0/b2_pi;
		siter++;
		if (siter==speeds.end()) {
			siter=speeds.begin();
		}
		
		
		minSpeed = maxSpeed = fabs(*siter);
		for (vector<float32>::iterator iter=speeds.begin();iter!=speeds.end();iter++) {
			minSpeed = min(minSpeed,fabs(*iter)); 
			maxSpeed = max(maxSpeed,fabs(*iter)); 
		}
		angle = joint->GetJointAngle()*180.0/b2_pi;
	}
	
	
	switch (state) {
		case TEETER_STATE_LOCKED:
			
			break;

		case TEETER_STATE_RESTING:
			
			break;
			
		case TEETER_STATE_STARTED: {
			
			if (minSpeed > 5) {
				state = TEETER_STATE_UNBALLANCED;
				
			}
			
		}	break;
			
		case TEETER_STATE_UNBALLANCED: {
			
			if (maxSpeed  < 3 && abs(angle) < 5) {
			
				state = TEETER_STATE_BALLANCED;
				joint->SetLimits(-5.0/180.0*b2_pi,5.0/180.0*b2_pi); 
				joint->EnableLimit(true);
				
				loopState = LOOP_BACKWARD;
				
				loopFrame = ofGetFrameNum();
				lbiter = cbiter;
			}
		}	break;	
			
		case TEETER_STATE_BALLANCED: {
			
			if (ofGetFrameNum() > loopFrame) {
				loopFrame=ofGetFrameNum();
				
				
				switch (loopState) {
					case LOOP_BACKWARD:
						cbiter--;
						if (cbiter==blobs.begin()) {
							cbiter = blobs.end()-1;
						}
						
						break;
					case LOOP_FORWARD:
						cbiter++;
						if (cbiter==blobs.end()) {
							cbiter = blobs.begin();
						} 
						break;

					default:
						break;
				}
				
				if (cbiter==lbiter) {
					switch (loopState) {
						case LOOP_BACKWARD:
							loopState = LOOP_FORWARD;
							break;
						case LOOP_FORWARD:
							loopState = LOOP_BACKWARD;
							break;
							
						default:
							break;
					}
				}
			}
		} break;

			
		default:
			break;
	}
	
}

void Teeter::updateBlob(ofxCvBlob& blob) {
	if (state!=TEETER_STATE_BALLANCED) {
		cbiter++;
		if (cbiter==blobs.end()) {
			cbiter=blobs.begin();
		}
		(**cbiter) = blob;
		
		int n=2;
		for( int j=n; j<blob.nPts-n; j++ ) {
			for (int i=1; i<=n; i++) {
				(*cbiter)->pts[j]+=blob.pts[j-i]+blob.pts[j+i];
			}
			(*cbiter)->pts[j] *= (1.0f/(2.0*n+1));
			
		}
		
		float32 center = (*cbiter)->centroid.x-centerX;
	//	if (center > -15 && center < 15) {
	//		bias = 1.5+center/10;
	//		displace(bias);
	//	}
		bias = center/10.0f;
		displace(bias);
	}
}


void Teeter::DrawShape(b2Fixture* fixture, const b2Transform& xf)
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
	
//	glEnable(GL_BLEND);
//	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
	glBegin(GL_TRIANGLE_FAN);
	for (int32 i = 0; i < vertexCount; ++i)
	{
		glVertex2f(vertices[i].x, vertices[i].y);
	}
	glEnd();
//	glDisable(GL_BLEND);
	
//	glColor4f(color.r, color.g, color.b, 1.0f);
//	glBegin(GL_LINE_LOOP);
//	for (int32 i = 0; i < vertexCount; ++i)
//	{
//		glVertex2f(vertices[i].x, vertices[i].y);
//	}
//	glEnd();
	
}

void Teeter::draw() {
	
		
	switch (state) {
		case TEETER_STATE_BROKEN:
			DrawShape(teeterFixture, teeter->GetTransform());
			DrawShape(payloadFixture, payload->GetTransform());
			DrawShape(playerFixture, player->GetTransform());
			break;
		default:
			DrawShape(teeterFixture, teeter->GetTransform());
			DrawShape(payloadFixture, teeter->GetTransform());
			if (playerFixture) {
				DrawShape(playerFixture, teeter->GetTransform());
			}
			break;
	}
	
	
	if (bLeaf) {
		const b2Transform& xf = ( state==TEETER_STATE_BROKEN ? payload : teeter)->GetTransform();
		b2PolygonShape* poly = (b2PolygonShape*)payloadFixture->GetShape();
		int32 vertexCount = poly->m_vertexCount;
		b2Assert(vertexCount <= b2_maxPolygonVertices);
		b2Vec2 vertices[b2_maxPolygonVertices];		
		for (int32 i = 0; i < vertexCount; ++i)
		{
			vertices[i] = b2Mul(xf, poly->m_vertices[i]);
		}
		
		ofPushMatrix();
		float32 scaleFactor = 0.02;
		ofTranslate((vertices[0].x+vertices[1].x)/2, vertices[0].y, 0);
		ofScale(scaleFactor,-scaleFactor,1.0f);
		ofTranslate(-leafImage->width/2, -leafImage->height, 0.0f);
		float degree = atan2(vertices[1].y-vertices[0].y, vertices[1].x-vertices[0].x) / b2_pi * 180;
		ofRotate(-degree);
		ofEnableAlphaBlending();
		leafImage->draw(0,0);
		ofDisableAlphaBlending();
		ofPopMatrix();
		
	}
	
	
	
}

void Teeter::drawPlayer() {
		
	
	
	ofPushMatrix();
	
	ofxCvBlob &blob = **cbiter;
	
	if (state==TEETER_STATE_RESTING) {
		
		//b2Vec2(5.0f*t-2*p/3.0f, p+p/10)
		
		ofTranslate(position.x+5.0*t, position.y, 0);
		
		float32 blobFactor = 0.05 / scale; // to cancel current teeter scaling
		ofScale(blobFactor, -blobFactor,1.0f);
		ofTranslate(-centerX, -(blob.boundingRect.y+blob.boundingRect.height), 0);
		
	} else {
		const b2Transform& xf = ( state==TEETER_STATE_BROKEN ? player : teeter)->GetTransform();
		b2PolygonShape* poly = (b2PolygonShape*)playerFixture->GetShape();
		int32 vertexCount = poly->m_vertexCount;
		b2Assert(vertexCount <= b2_maxPolygonVertices);
		b2Vec2 vertices[b2_maxPolygonVertices];		
		
		for (int32 i = 0; i < vertexCount; ++i)
		{
			vertices[i] = b2Mul(xf, poly->m_vertices[i]);
		}
		
		ofTranslate((vertices[0].x+vertices[1].x)/2, vertices[0].y, 0);
		float degree = atan2(vertices[1].y-vertices[0].y, vertices[1].x-vertices[0].x) / b2_pi * 180;
		ofRotate(degree);
		float32 blobFactor = 0.05 / scale; // to cancel current teeter scaling
		ofScale(blobFactor, -blobFactor,1.0f);
		ofTranslate(-centerX, -(blob.boundingRect.y+blob.boundingRect.height), 0);
		
	}
	
	ofPushStyle();
	
	switch (state) {
		case TEETER_STATE_RESTING:
			ofSetColor(0x808080);
			break;
		case TEETER_STATE_STARTED:
		case TEETER_STATE_UNBALLANCED:
		case TEETER_STATE_TOUCHING:
			ofSetColor(0xFF0000);
			break;
		case TEETER_STATE_BALLANCED:
			ofSetColor(0x000000);
			break;
			
			
		default:
			break;
	}
	
	
	ofFill();
	ofBeginShape();
	for( int j=0; j<blob.nPts; j++ ) {
		ofVertex( blob.pts[j].x, blob.pts[j].y );
	}
	ofEndShape();
	
	ofSetColor(0xFF0000);
	ofRect( blob.centroid.x-2, blob.centroid.y-2,  4, 4 );
	ofNoFill();
	//	ofRect( blob.boundingRect.x, blob.boundingRect.y,  blob.boundingRect.width, blob.boundingRect.height );
	
	ofPopStyle();
	ofPopMatrix();
	
}




void Teeter::displace(float32 bias) {
	

	
	this->bias = bias;
	if (joint) {
		joint->SetMotorSpeed(-bias*b2_pi/10);

	}
		
		
//	teeter->SetAwake(true);

}

void Teeter::breakTeeter() {
	
	m_world->DestroyJoint(joint);
	joint = 0;
	state = TEETER_STATE_BROKEN;
	
	// Create two bodies from one.
	b2Body* body1 = teeter;
	b2Vec2 center = body1->GetWorldCenter();
	
	body1->DestroyFixture(playerFixture);
	playerFixture = NULL;
	
	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.position = teeter->GetPosition();
	bd.angle = teeter->GetAngle();
	
	
	b2Body* body2 = m_world->CreateBody(&bd);
	playerFixture = body2->CreateFixture(&playerShape, 1.0f);

	
	// Compute consistent velocities for new bodies based on
	// cached velocity.
	b2Vec2 center1 = body1->GetWorldCenter();
	b2Vec2 center2 = body2->GetWorldCenter();
	
	b2Vec2 velocity1 = m_velocity + b2Cross(m_angularVelocity, center1 - center);
	b2Vec2 velocity2 = m_velocity + b2Cross(m_angularVelocity, center2 - center);
	
	body1->SetAngularVelocity(m_angularVelocity);
	body1->SetLinearVelocity(velocity1);
	
	body2->SetAngularVelocity(m_angularVelocity);
	body2->SetLinearVelocity(velocity2);
	
	player = body2;
	
	// Create two bodies from one.
	m_velocity = velocity1;
	body1 = teeter;
	center = body1->GetWorldCenter();
	
	body1->DestroyFixture(payloadFixture);
	payloadFixture = NULL;
	
	bd.type = b2_dynamicBody;
	bd.position = teeter->GetPosition();
	bd.angle = teeter->GetAngle();
	
	body2 = m_world->CreateBody(&bd);
	
	payloadFixture = body2->CreateFixture(&payloadShape,1.0f);
	
	// Compute consistent velocities for new bodies based on
	// cached velocity.
	center1 = body1->GetWorldCenter();
	center2 = body2->GetWorldCenter();
	
	velocity1 = m_velocity + b2Cross(m_angularVelocity, center1 - center);
	velocity2 = m_velocity + b2Cross(m_angularVelocity, center2 - center);
	
	body1->SetAngularVelocity(m_angularVelocity);
	body1->SetLinearVelocity(velocity1);
	
	body2->SetAngularVelocity(m_angularVelocity);
	body2->SetLinearVelocity(velocity2);
	 
	payload = body2;
}

void Teeter::jump() {
	player->ApplyForce(b2Vec2(0,-10000),player->GetWorldCenter());
	teeter->ApplyTorque(-50000.0f*t);
}


void Teeter::setState(int state) {
	this->state = state;
}

int Teeter::getState() {
	return this->state;
}



b2RevoluteJoint * Teeter::getJoint() {
	return this->joint;
}




void Teeter::getTransform(b2Vec2 &pos,float32 &scale) {
	pos = position;
	scale = this->scale;
}

void Teeter::transform() {
	ofScale(scale, scale, 1.0f);
	ofTranslate(-position.x, -position.y, 0.0f);
}

b2Body *Teeter::getTeeterBody() {		
	return teeter;
}



void Teeter::log(ostringstream& ss) {
	ss << "state: " << getState() <<  ",bias: "<< bias << ", minSpeed: " << minSpeed	<< ", maxSpeed: " << maxSpeed << ", angle: " << angle	<< endl; // ", speed: " << joint->GetJointSpeed()*180.0/b2_pi	<<
}