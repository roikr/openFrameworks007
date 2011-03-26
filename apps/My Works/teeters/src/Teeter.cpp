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
	teeterDef.position =  position+b2Vec2(0.0f,t);;
	teeter = m_world->CreateBody(&teeterDef);
	
	b2PolygonShape teeterShape;
	//teeterShape.SetAsEdge(b2Vec2(-t*5.0f, 0.0f), b2Vec2(t*5.0f, 0.0f));
	teeterShape.SetAsBox(t*5.0f, p/10, b2Vec2(0.0f, 0.0f),0.0f);
	
	b2FixtureDef teeterFixtureDef;
	teeterFixtureDef.shape = &teeterShape;
	teeterFixtureDef.density = 1.0f; // thin teeter
	teeterFixtureDef.filter.categoryBits = 0x0002;
	teeterFixtureDef.filter.maskBits = 0x0002;
	//teeterFixtureDef.isSensor = true;
	teeterFixture = teeter->CreateFixture(&teeterFixtureDef);
	
	b2RevoluteJointDef jointDef;
	
	jointDef.Initialize(parent,teeter,position); 
	
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
	
	b2Vec2 payloadPosition(0.5f*5.0f*t, 0.0f);
	
	payloadShape.SetAsBox(2*p/3.0f, p, payloadPosition+b2Vec2(0, p+p/10), 0.0f);
	payloadFixtureDef.shape = &payloadShape;
	payloadFixtureDef.density = 1.0f;
	payloadFixtureDef.filter.categoryBits = 0x0002;
	payloadFixtureDef.filter.maskBits = 0x0002;
	fixture = teeter->CreateFixture(&payloadFixtureDef);
	
	if (bLeaf) {
		payloadShape.SetAsBox(2*p/3.0f, p, -payloadPosition+b2Vec2(0, p+p/10 ), 0.0f);
		payloadFixtureDef.shape = &payloadShape;
		payloadFixtureDef.density = 1.0f;
		payloadFixtureDef.filter.categoryBits = 0x0002;
		payloadFixtureDef.filter.maskBits = 0x0002;
		teeter->CreateFixture(&payloadFixtureDef);
	}
	
	
	teeterMeasure = t;
	payloadMeasure = p;
	bias = 1.0f;
	bDown = false;
	stepCount = 0;
	scale = 4.0f/teeterMeasure;

}


void Teeter::displace() {
	
	float32 t = teeterMeasure;
	float32 p = payloadMeasure;
	
	teeter->DestroyFixture(fixture);
	
	
	b2PolygonShape payloadShape;
	//payloadShape.SetAsBox(2*p/3, p, *bias)*5.0f*t, p+p/10), 0.0f);
	b2Vec2 payloadPosition(0.5f*5.0f*t, 0.0f);
	payloadShape.SetAsBox(2*p/3.0f, p*bias, payloadPosition+b2Vec2(0, p*bias+p/10), 0.0f);
	
	b2FixtureDef payloadFixtureDef;
	payloadFixtureDef.shape = &payloadShape;
	payloadFixtureDef.density = 1.0f;
	payloadFixtureDef.filter.categoryBits = 0x0004;
	payloadFixtureDef.filter.maskBits = 0x0004;
	fixture = teeter->CreateFixture(&payloadFixtureDef);
	
	teeter->SetAwake(true);
	
}


void Teeter::update(int32 m_stepCount) {
	if (!joint->IsLimitEnabled() && !bDown) {
		if (fabs(joint->GetJointSpeed())>b2_epsilon) {
			
			stepCount = m_stepCount;
		}
		
		if (m_stepCount - stepCount > 60) {
			cout << "no rotation" << endl;
			float32 angle = joint->GetJointAngle();
			joint->SetLimits(angle-b2_pi/72.0f,angle+b2_pi/72.0f); // -b2_pi/72.0f
			joint->EnableLimit(true);
		}
		
	}
}

void Teeter::updateBlob(ofxCvBlob& blob) {
	this->blob = blob;
	bias = (blob.centroid.x-centerBlob.centroid.x)/10;
	displace();
}

void Teeter::setCenter() {
	centerBlob = blob;
}


void Teeter::draw() {
	const b2Transform& xf = teeter->GetTransform();
	
	b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
	int32 vertexCount = poly->m_vertexCount;
	b2Assert(vertexCount <= b2_maxPolygonVertices);
	b2Vec2 vertices[b2_maxPolygonVertices];
	
	for (int32 i = 0; i < vertexCount; ++i)
	{
		vertices[i] = b2Mul(xf, poly->m_vertices[i]);
	}
	
	ofPushMatrix();
	ofTranslate((vertices[0].x+vertices[1].x)/2, vertices[0].y, 0);
	float degree = atan2(vertices[1].y-vertices[0].y, vertices[1].x-vertices[0].x) / M_PI * 180;
	ofRotate(degree);

	
	
	
	ofScale(0.02f, -0.02f,1.0f);
	
	ofTranslate(-(centerBlob.boundingRect.x+centerBlob.boundingRect.width/2), -(blob.boundingRect.y+blob.boundingRect.height), 0);
	ofPushStyle();
	
	
	
	
	ofSetColor(0xFFFFFF);
	
	
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