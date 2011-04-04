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
	
	nextPosition = b2Vec2(0.4*-5.0f*t, p/10);
	
	b2BodyDef teeterDef;
	teeterDef.type = b2_dynamicBody;
	teeterDef.position =  position+b2Vec2(0.0f, 1.5f*t); // raise the teeter
	teeter = m_world->CreateBody(&teeterDef);
	
	b2PolygonShape teeterShape;
	//teeterShape.SetAsEdge(b2Vec2(-t*5.0f, 0.0f), b2Vec2(t*5.0f, 0.0f));
	teeterShape.SetAsBox(t*5.0f, p/10, b2Vec2(0.0f, 0.0f),0.0f);
	
	

	teeterFixture = teeter->CreateFixture(&teeterShape,1.0f);
	
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
	
	
	
	playerShape.SetAsBox(p, 3*p, b2Vec2(5.0f*t-p, 3*p+p/10), 0.0f);
	playerFixture = 0;
	
	leafImage = 0;
	
	this->bLeaf = bLeaf;
	
	
	if (bLeaf) {
		
		payloadShape.SetAsBox(p, p, b2Vec2(-5.0f*t+p, p+p/10), 0.0f);
		leafImage = new ofImage();
		leafImage->loadImage("weight.png");
	} else {
		float32 nextp = nextt/2.0f;
		
		
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
	
	
	
	cbiter = blobs.end();
	bBlob = false;
	
	loopState = LOOP_IDLE;
	
}

Teeter::~Teeter() {
	if (joint) {
		//m_world->DestroyJoint(joint);
		teeter->DestroyFixture(teeterFixture);
		teeter->DestroyFixture(payloadFixture);
		if (playerFixture) {
			teeter->DestroyFixture(playerFixture);
		}
		
		m_world->DestroyBody(teeter);
	} else {
		teeter->DestroyFixture(teeterFixture);
		payload->DestroyFixture(payloadFixture);
		if (playerFixture) {
			player->DestroyFixture(playerFixture);
		}
		m_world->DestroyBody(teeter);
		m_world->DestroyBody(payload);
		m_world->DestroyBody(player);
	}

	
	
}

b2Vec2 Teeter::getNextPosition() {
	return teeter->GetWorldPoint(nextPosition);
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
				joint->SetLimits(-10.0/180.0*b2_pi,10.0/180.0*b2_pi); 
				
				joint->EnableMotor(false);
				
				loopState = LOOP_BACKWARD;
				
				loopFrame = ofGetFrameNum();
				lbiter = cbiter;
			}
		}	break;	
			
		case TEETER_STATE_BROKEN:
			if (loopState == LOOP_IDLE) {
				break;
			}
			
		case TEETER_STATE_BALLANCED:
		
		{
			
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


void Teeter::setFocus(ofRectangle &rect) {
	state = TEETER_STATE_RESTING;
	joint->SetLimits(-18.0/180.0*b2_pi,18.0/180.0*b2_pi); 
	this->rect = rect;
}

void Teeter::start() {
	state = TEETER_STATE_STARTED;
	playerFixture = teeter->CreateFixture(&playerShape,1.0f/3);
	joint->EnableMotor(true);
	joint->SetMaxMotorTorque(10000);
	//	centerBlob = blob;
	
	for (vector<float32>::iterator iter=speeds.begin();iter!=speeds.end();iter++) {
		*iter = 0.0f; 
	}
}

void Teeter::noBlob() {
	bBlob = false;
}

void Teeter::updateBlob(ofxCvBlob& blob) {
	
	bBlob = true;
	switch (state) {
		case TEETER_STATE_RESTING:
			

			
			if (blobs.empty()) {
				blobs.push_back(new ofxCvBlob);
				cbiter=blobs.begin();
			}
			(**cbiter) = blob;
			
			


			break;
			
		case TEETER_STATE_BROKEN:
		case TEETER_STATE_STARTED:
		case TEETER_STATE_UNBALLANCED: {
			
			if (blobs.size()<NUM_BLOBS) {
				blobs.push_back(new ofxCvBlob);
				cbiter=blobs.end()-1;
			} else if (cbiter==blobs.end()) {
				cbiter=blobs.begin();
			} else {
				cbiter++;
				if (cbiter==blobs.end()) {
					cbiter=blobs.begin();
				}
			}
			
			(**cbiter) = blob;
			
			int n=2;
			for( int j=n; j<blob.nPts-n; j++ ) {
				for (int i=1; i<=n; i++) {
					(*cbiter)->pts[j]+=blob.pts[j-i]+blob.pts[j+i];
				}
				(*cbiter)->pts[j] *= (1.0f/(2.0*n+1));
				
			}
			
			if (state!=TEETER_STATE_BROKEN) {
									
				joint->SetMotorSpeed(-( (*cbiter)->centroid.x-(rect.x+rect.width/2))/10.0f*b2_pi/10.0f);
					
				
				
			}
			
			
		} break;
	}
	
}


void Teeter::DrawShape(b2Fixture* fixture, const b2Transform& xf)
{
	
	b2Color color(0.0f, 0.0f, 0.0f);
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
	glColor3f(color.r, color.g,  color.b);
	//glColor3f(0.5f * color.r, 0.5f * color.g, 0.5f * color.b);
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
			if (!bLeaf) {
				DrawShape(payloadFixture, payload->GetTransform());
			}
			
//			if (playerFixture) {
//				DrawShape(playerFixture, player->GetTransform());
//			}
			break;
		default:
			DrawShape(teeterFixture, teeter->GetTransform());
			if (!bLeaf) {
				DrawShape(payloadFixture, teeter->GetTransform());
			}
//			if (playerFixture) {
//				DrawShape(playerFixture, teeter->GetTransform());
//			}
			break;
	}
	
	
	if (bLeaf) {
		ofPushMatrix();
		
		
		translateFixture(payloadFixture, ( state==TEETER_STATE_BROKEN ? payload : teeter)->GetTransform());
		
		float32 scaleFactor = 0.08f/scale ; 
		ofScale(scaleFactor, -scaleFactor,1.0f);
		ofTranslate(-leafImage->width/2, -leafImage->height, 0.0f);
		leafImage->draw(0,0);
//		ofDisableAlphaBlending();
		ofPopMatrix();
		
	}
	
	
	
}

void Teeter::translateFixture(b2Fixture* fixture, const b2Transform& xf) {
	b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
	int32 vertexCount = poly->m_vertexCount;
	b2Assert(vertexCount <= b2_maxPolygonVertices);
	b2Vec2 vertices[b2_maxPolygonVertices];		
	for (int32 i = 0; i < vertexCount; ++i)
	{
		vertices[i] = b2Mul(xf, poly->m_vertices[i]);
	}
	
	ofTranslate((vertices[0].x+vertices[1].x)/2, (vertices[0].y+vertices[1].y)/2, 0);
	
	ofRotate(atan2(vertices[1].y-vertices[0].y, vertices[1].x-vertices[0].x) / b2_pi * 180);

}

void Teeter::drawPlayer() {
	
	if (!bBlob && loopState==LOOP_IDLE) {
		return;
	}
	
	ofPushMatrix();
	
	
	
	if (state==TEETER_STATE_RESTING) {
		//b2Vec2(5.0f*t-2*p/3.0f, p+p/10)
		ofTranslate(position.x+5.0*t, position.y, 0);
		
				
	} else {
		if (!playerFixture) {
			return;
		}
		translateFixture(playerFixture, ( state==TEETER_STATE_BROKEN ? player : teeter)->GetTransform());
		//ofRotate(degree);
		
	}
	
	float32 blobFactor = 0.05 / scale; // to cancel current teeter scaling
	ofScale(blobFactor, -blobFactor,1.0f);
	//	ofTranslate(-(rect.x+rect.width/2), -(blob.boundingRect.y+blob.boundingRect.height), 0);
	ofTranslate(-(rect.x+rect.width/2), -(rect.y+rect.height), 0);
	
	
	ofPushStyle();
	
	switch (state) {
		case TEETER_STATE_RESTING:
			ofSetColor(0x808080);
			break;
		case TEETER_STATE_STARTED:
		case TEETER_STATE_UNBALLANCED:
		
			
			ofSetColor(0xFF0000);
			break;
		case TEETER_STATE_BALLANCED:
		case TEETER_STATE_BROKEN:
			ofSetColor(0x000000);
			break;
			
			
		default:
			break;
	}
	
	
	ofFill();
	ofBeginShape();
	for( int j=0; j< (*cbiter)->nPts; j++ ) {
		ofVertex( (*cbiter)->pts[j].x, (*cbiter)->pts[j].y );
	}
	ofEndShape();
	
	ofSetColor(0xFF0000);
	ofRect( (*cbiter)->centroid.x-2, (*cbiter)->centroid.y-2,  4, 4 );
	ofNoFill();
	//	ofRect( blob.boundingRect.x, blob.boundingRect.y,  blob.boundingRect.width, blob.boundingRect.height );
	
	ofPopStyle();
	ofPopMatrix();
	
}




void Teeter::displace(float32 bias) {
	
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
	playerFixture = body2->CreateFixture(&playerShape, 1.0f/3);

	
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

void Teeter::leave() {
	player->DestroyFixture(playerFixture);
	playerFixture = NULL;
}

void Teeter::jump(int multiplier) {
	float32 power = 500/exp(multiplier*log(SCALING_FACTOR));
	player->ApplyForce(b2Vec2(0,-power),player->GetWorldCenter());
	teeter->ApplyTorque(-power);
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


void Teeter::debug(ostringstream& ss) {
	ss << "state: " << getState() << endl;
	if (cbiter!=blobs.end()) {
		ss << "diff: " << (*cbiter)->centroid.x-(rect.x+rect.width/2)<<endl;
	}
	//ss << "centerTime: " << ofGetElapsedTimeMillis() - centerTime << endl;
	ss <<"minSpeed: " << minSpeed	<< ", maxSpeed: " << maxSpeed << ", angle: " << angle	<< endl; // ", speed: " << joint->GetJointSpeed()*180.0/b2_pi	<<
}