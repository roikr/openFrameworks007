/*
 *  Teeter.h
 *  teeterTotter
 *
 *  Created by Roee Kremer on 3/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "Box2D.h"
#include "ofxOpenCv.h"

class Teeter  {
public:
	Teeter(b2World *m_world,float32 t,b2Body *parent,b2Vec2 position,bool bLeaf = false);
	void draw();
	
	void update(int32 m_stepCount);
	
	
	void updateBlob(ofxCvBlob& blob);
	void setCenter();
	void displace();
	
	float32 bias;
	b2RevoluteJoint *joint;
	int32 stepCount;
	b2Body * teeter;
	bool bDown;
	b2Vec2 position;
	float32 teeterMeasure;

private:
	
	
	float32 payloadMeasure;
	
	b2Fixture *teeterFixture;
	b2Fixture *fixture;
	
	
	
	
	
	
	ofxCvBlob blob;
	ofxCvBlob centerBlob;
	
	
};