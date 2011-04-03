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
	TEETER_STATE_LOCKED,
	TEETER_STATE_RESTING,
	TEETER_STATE_STARTED,
	TEETER_STATE_UNBALLANCED,
	TEETER_STATE_TOUCHING,
	TEETER_STATE_BALLANCED,
	TEETER_STATE_BROKEN
	

};

class Teeter  {
public:
	Teeter(b2World *m_world,float32 t,b2Vec2 position,b2Body *parent,bool bLeaf,float32 nextt);

	void log(ostringstream& ss);
	void update();
	
	void setFocus(float centerX);
	void start();

	void draw();
	void drawPayload();
	
	void DrawShape(b2Fixture* fixture, const b2Transform& xf);
	void updateBlob(ofxCvBlob& blob);
	void displace(float32 bias);
	void breakTeeter();
	void jump();
	b2RevoluteJoint * getJoint();
	

	void setState(int state);
	int getState();
	void getTransform(b2Vec2 &pos,float32 &scale);
	void transform();
	b2Body *getTeeterBody();
	
	
	b2Vec2 getNextPosition();
		
private:
	
	b2Vec2 nextPosition;
	
	void createPlayer(b2Body *body);
	
	b2World *m_world;
	
	b2Vec2 position;
	float32 scale;
	
	b2Body * teeter;
	b2Body * player;
	b2Body * payload;
	
	float32 t; //teeterMeasure;
	float32 p; // payloadMeasure;
	
	int state;
	
	vector<float32> speeds;
	vector<float32>::iterator siter;
	

	float32 bias;
	b2RevoluteJoint *joint;
	
	
	
	b2Fixture *teeterFixture;
	b2Fixture *playerFixture;
	b2Fixture *payloadFixture;
	
	vector<ofxCvBlob *> blobs; 
	vector<ofxCvBlob *>::iterator cbiter; // current blob iter
	
	
	float centerX;
	
	bool bLeaf;
	ofImage *leafImage;
	
	float32 minSpeed,maxSpeed,angle;
	
	b2Vec2 m_velocity;
	float32 m_angularVelocity;
	
	b2PolygonShape payloadShape;
};