/*
 *  Teeter.cpp
 *  teeterTotter
 *
 *  Created by Roee Kremer on 3/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Teeter.h"


void Teeter::setup(b2World &m_world,b2Body* body,float32 distance) {
	
	m_distance = distance;
	
	b2BodyDef teeterDef;
	teeterDef.type = b2_dynamicBody;
	teeterDef.position.Set(0.0f,1.0f);
	m_teeter = m_world.CreateBody(&teeterDef);
	
	b2PolygonShape teeterShape;
	teeterShape.SetAsEdge(b2Vec2(-m_distance, 0.0f), b2Vec2(m_distance, 0.0f));
	
	b2FixtureDef teeterFixtureDef;
	teeterFixtureDef.shape = &teeterShape;
	teeterFixtureDef.density = 1.0f;
	teeterFixtureDef.friction = 1.0f;
	m_teeter->CreateFixture(&teeterFixtureDef);
	
	
	b2RevoluteJointDef jointDef;
	jointDef.collideConnected = true;
	jointDef.Initialize(body,m_teeter,b2Vec2(0.0f,1.0f)); // teeter->GetWorldCenter()
	
	m_world.CreateJoint(&jointDef);
	
	b2PolygonShape shape;
	shape.SetAsBox(0.25f, 1.0f, b2Vec2(-0.8*m_distance, 1.0f), 0.0f);
	m_fixture1 = m_teeter->CreateFixture(&shape, 5.0f);
	
	
	shape.SetAsBox(0.25f, 1.0f, b2Vec2(0.8*m_distance, 1.0f), 0.0f);
	m_fixture2 = m_teeter->CreateFixture(&shape, 2.5f);
	m_figure = m_teeter->CreateFixture(&shape, 2.5f);
	
	
	
}

void Teeter::displace(float32 bias) {

	
	m_teeter->DestroyFixture(m_fixture2);
	b2PolygonShape shape;
	shape.SetAsBox(0.25f, 1.0f, b2Vec2((0.8+0.2*bias)*m_distance, 1.0f), 0.0f);
	m_fixture2 = m_teeter->CreateFixture(&shape, 2.5f);
	m_teeter->SetAwake(true);
}

void Teeter::updateBlob(ofxCvBlob& blob) {
	this->blob = blob;
	displace((blob.centroid.x-centerBlob.centroid.x)/10);
}

void Teeter::setCenter() {
	centerBlob = blob;
}


void Teeter::draw() {
	const b2Transform& xf = m_teeter->GetTransform();
	
	b2PolygonShape* poly = (b2PolygonShape*)m_figure->GetShape();
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