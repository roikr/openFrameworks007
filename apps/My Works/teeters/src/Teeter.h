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

enum {
	TEETER_STATE_RESTED,
	TEETER_STATE_STARTED,
	TEETER_STATE_UNBALLANCED,
	TEETER_STATE_BALLANCED,
	TEETER_STATE_BROKEN

};

class Teeter  {
public:
	Teeter(b2World *m_world,float32 t,b2Body *parent,b2Vec2 position,bool bLeaf = false);
	void draw();
	void log(ostringstream& ss);
	void update();
	
	void start();

	void updateBlob(ofxCvBlob& blob);
	void setCenter();
	void displace(float32 bias);
	b2RevoluteJoint * getJoint();

	void setState(int state);
	int getState();
	
	b2Body * teeter;
	
	b2Vec2 position;
	float32 scale;
	float32 teeterMeasure;
	
	
	
private:
	
	
	int state;
	
	vector<float32> speeds;
	vector<float32>::iterator siter;
	float32 maxSpeed;

	float32 bias;
	b2RevoluteJoint *joint;
	
	float32 payloadMeasure;
	
	b2Fixture *teeterFixture;
	b2Fixture *fixture;
	
	
	ofxCvBlob blob;
	ofxCvBlob centerBlob;
	
};