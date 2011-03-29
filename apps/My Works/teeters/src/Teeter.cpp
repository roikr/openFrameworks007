/*
 *  Teeter.cpp
 *  teeterTotter
 *
 *  Created by Roee Kremer on 3/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Teeter.h"


Teeter::Teeter(b2World *m_world,float32 t,b2Body *parent,b2Vec2 position,bool bLeaf) {
	float32 p =t/2;
	this->position = position;
		
	
	b2BodyDef teeterDef;
	teeterDef.type = b2_dynamicBody;
	teeterDef.position =  position;
	teeter = m_world->CreateBody(&teeterDef);
	
	b2PolygonShape teeterShape;
	//teeterShape.SetAsEdge(b2Vec2(-t*5.0f, 0.0f), b2Vec2(t*5.0f, 0.0f));
	teeterShape.SetAsBox(t*5.0f, p/10, b2Vec2(0.0f, t),0.0f);
	
	b2FixtureDef teeterFixtureDef;
	teeterFixtureDef.shape = &teeterShape;
	teeterFixtureDef.density = 1.0f; // thin teeter
	teeterFixtureDef.filter.categoryBits = 0x0002;
	teeterFixtureDef.filter.maskBits = 0x0002;
	//teeterFixtureDef.isSensor = true;
	teeterFixture = teeter->CreateFixture(&teeterFixtureDef);
	
	b2RevoluteJointDef jointDef;
	
	jointDef.Initialize(parent,teeter,position+b2Vec2(0.0f,t)); 
	
	jointDef.lowerAngle = 0.0f * b2_pi;
	jointDef.upperAngle = 0.0f * b2_pi;
	jointDef.enableLimit = true;
	jointDef.collideConnected = 1;
	//jointDef.maxMotorTorque = 1000.0f;
	//jointDef.motorSpeed = 0.0f;
	jointDef.enableMotor = false;
	joint = (b2RevoluteJoint*) m_world->CreateJoint(&jointDef);
	
	teeterFixture->SetUserData(joint);
	
	
	b2FixtureDef payloadFixtureDef;
	b2PolygonShape payloadShape;
	
	
	
	payloadShape.SetAsBox(2*p/3.0f, p, b2Vec2(5.0f*t, t+p+p/10), 0.0f);
	payloadFixtureDef.shape = &payloadShape;
	payloadFixtureDef.density = 1.0f;
	payloadFixtureDef.filter.categoryBits = 0x0002;
	payloadFixtureDef.filter.maskBits = 0x0002;
	fixture = teeter->CreateFixture(&payloadFixtureDef);
	
	if (bLeaf) {
		payloadShape.SetAsBox(2*p/3.0f, p, b2Vec2(-5.0f*t, t+p+p/10), 0.0f);
		payloadFixtureDef.shape = &payloadShape;
		payloadFixtureDef.density = 1.0f;
		payloadFixtureDef.filter.categoryBits = 0x0002;
		payloadFixtureDef.filter.maskBits = 0x0002;
		teeter->CreateFixture(&payloadFixtureDef);
	}
	
	
	teeterMeasure = t;
	payloadMeasure = p;
	bias = 1.0f;
	
	scale = 4.0f/teeterMeasure;
	state = TEETER_STATE_RESTING;
	
	for (int i=0; i<30; i++) {
		speeds.push_back(0.0f);
	}
}

void Teeter::setFocus(float centerX) {
	this->centerX = centerX;
}

void Teeter::start() {
	state = TEETER_STATE_STARTED;
	joint->EnableLimit(false);
	//	centerBlob = blob;
}



void Teeter::update() {
	
	
	switch (state) {
		case TEETER_STATE_RESTING:
			
			break;
			
		case TEETER_STATE_STARTED: {
			
			float32 speed = joint->GetJointSpeed();
			if (fabs(speed) * 180.0/b2_pi > 2) {
				state = TEETER_STATE_UNBALLANCED;
				
				
				maxSpeed = fabs(speed);
				
				for (vector<float32>::iterator iter=speeds.begin();iter!=speeds.end();iter++) {
					*iter = speed; 
				}
				siter = speeds.begin();
			}
			
		}	break;
			
		case TEETER_STATE_UNBALLANCED: {
			(*siter) = joint->GetJointSpeed();
			siter++;
			if (siter==speeds.end()) {
				siter=speeds.begin();
			}
			
			maxSpeed = fabs(speeds.front());
			for (vector<float32>::iterator iter=speeds.begin()+1;iter!=speeds.end();iter++) {
				maxSpeed = fmax(maxSpeed,fabs(*iter)); 
			}
			
			
			if (maxSpeed * 180.0 /  b2_pi < 1) {
				state = TEETER_STATE_BALLANCED;
				float32 angle = joint->GetJointAngle();
				joint->SetLimits(angle-b2_pi/72.0f,angle+b2_pi/72.0f); 
				joint->EnableLimit(true);
			}
		}	break;	
			
		default:
			break;
	}
	
}

void Teeter::updateBlob(ofxCvBlob& blob) {
	this->blob = blob;
	
	float32 center = blob.centroid.x-centerX;
	if (center > -15 && center < 15) {
		bias = 1.5+center/10;
		displace(bias);
	}
}

void Teeter::draw() {
	const b2Transform& xf = teeter->GetTransform();
	
				
	b2PolygonShape* poly = (b2PolygonShape*)teeterFixture->GetShape();
	int32 vertexCount = poly->m_vertexCount;
	b2Assert(vertexCount <= b2_maxPolygonVertices);
	b2Vec2 vertices[b2_maxPolygonVertices];
	
	for (int32 i = 0; i < vertexCount; ++i)
	{
		vertices[i] = b2Mul(xf, poly->m_vertices[i]);
	}
	
	ofSetColor(0x000000);
	glBegin(GL_TRIANGLE_FAN);
	for (int32 i = 0; i < vertexCount; ++i)
	{
		glVertex2f(vertices[i].x, vertices[i].y);
	}
	glEnd();
	
	b2Vec2 vertex;
	
	glBegin(GL_TRIANGLE_FAN);
	vertex = b2Mul(xf,b2Vec2(-0.2f,0.0f));
	glVertex2f(vertex.x, vertex.y);
	vertex = b2Mul(xf,b2Vec2(0.2f,0.0f));
	glVertex2f(vertex.x, vertex.y);
	vertex = b2Mul(xf,b2Vec2(0.0f,teeterMeasure));
	glVertex2f(vertex.x, vertex.y);
	
	glEnd();
	
	
//	ofSetColor(0x000000);
//	glBegin(GL_LINE_LOOP);
//	for (int32 i = 0; i < vertexCount; ++i)
//	{
//		glVertex2f(vertices[i].x, vertices[i].y);
//	}
//	glEnd();
	
	
	poly = (b2PolygonShape*)fixture->GetShape();
	vertexCount = poly->m_vertexCount;
	b2Assert(vertexCount <= b2_maxPolygonVertices);
	vertices[b2_maxPolygonVertices];
	
	for (int32 i = 0; i < vertexCount; ++i)
	{
		vertices[i] = b2Mul(xf, poly->m_vertices[i]);
	}
	
	ofPushMatrix();
	
	if (state==TEETER_STATE_RESTING) {
		ofTranslate((vertices[0].x+vertices[1].x)/2, position.y, 0);
		
		float32 blobFactor = 0.05 / scale; // to cancel current teeter scaling
		ofScale(blobFactor, -blobFactor,1.0f);
		ofTranslate(-centerX, -(blob.boundingRect.y+blob.boundingRect.height), 0);
		
	} else {
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
			ofSetColor(0x0000FF);
			break;
		case TEETER_STATE_TOUCHING:
			ofSetColor(0xFF0000);
			break;
		case TEETER_STATE_BALLANCED:
			ofSetColor(0x00FF00);
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
	ofRect( blob.boundingRect.x, blob.boundingRect.y,  blob.boundingRect.width, blob.boundingRect.height );
	
	ofPopStyle();
	ofPopMatrix();
	//m_debugDraw->DrawSolidPolygon(vertices, vertexCount, color);
	
}

void Teeter::displace(float32 bias) {
	this->bias = bias;
	float32 t = teeterMeasure;
	float32 p = payloadMeasure;
	
	teeter->DestroyFixture(fixture);
	
	
	b2PolygonShape payloadShape;
	//payloadShape.SetAsBox(2*p/3, p, *bias)*5.0f*t, p+p/10), 0.0f)
	payloadShape.SetAsBox(2*p/3.0f, p*bias, b2Vec2(5.0f*t, t+p*bias+p/10), 0.0f);
		
	b2FixtureDef payloadFixtureDef;
	payloadFixtureDef.shape = &payloadShape;
	payloadFixtureDef.density = 1.0f;
	payloadFixtureDef.filter.categoryBits = 0x0004;
	payloadFixtureDef.filter.maskBits = 0x0004;
	fixture = teeter->CreateFixture(&payloadFixtureDef);
	
	teeter->SetAwake(true);
	
	
	
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

b2Body *Teeter::getBody() {		
	return teeter;
}

void Teeter::log(ostringstream& ss) {
	ss << "state: " << getState() << ", speed: " << joint->GetJointSpeed()*180.0/b2_pi	<< ", maxSpeed: " << maxSpeed*180.0/b2_pi	<< endl;
}