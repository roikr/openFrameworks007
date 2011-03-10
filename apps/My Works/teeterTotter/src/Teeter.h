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

class Teeter {
public:
	void setup(b2World &m_world,b2Body* body,float32 distance);
	void displace(float32 bias);
	void draw();
	
	
	void updateBlob(ofxCvBlob& blob);
	void setCenter();
private:
	
	b2Body* m_teeter;
	
	b2Fixture* m_fixture1;
	b2Fixture* m_fixture2;
	
	b2Fixture* m_figure;
	
	float32 m_distance;
	ofxCvBlob blob;
	ofxCvBlob centerBlob;
	
	float32 m_center;
	float32 m_position;


};