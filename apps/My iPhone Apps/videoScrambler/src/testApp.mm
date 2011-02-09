#include "testApp.h"
#include "ofMainExt.h"

#include "ofxXmlSettings.h"

#import "Box2DAccel.h"



//--------------------------------------------------------------
void testApp::setup(){	
	//ofDisableDataPath();
	// register touch events
	ofxRegisterMultitouch(this);
	
		
	//ofxCopyFile(ofToResourcesPath("IMG_0121.MOV"), ofToDataPath("IMG_0121.MOV"));
	//ofxCopyFile(ofToResourcesPath("can.m4v"), ofToDataPath("can.m4v"));
	//ofxCopyFile(ofToResourcesPath("IZABO_On_my_way.mp4"), ofToDataPath("IZABO_On_my_way.mp4"));
	
	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
	Box2DAccel *accel = [[[Box2DAccel alloc] init] retain];
	accel.app = this;

 
	
	ofBackground(0,0,0);
		
	
	ofSetFrameRate(60);
	
	int bufferSize = 256;
	int sampleRate = 44100;
	
	ofSoundStreamSetup(2, 0, this, sampleRate, bufferSize, 2);	

	streamer.setup(ofToResourcesPath("IZABO_On_my_way.mp4"));
	//streamer.setup("http:////www.roikr.com//can.m4v");
	//streamer.setup(ofToDataPath("IMG_0121.MOV"));
	bStarted = false;
	
	tilesPerRow = 4;
	tileSize = ofGetHeight()/tilesPerRow;
	
	coordinator.setup(ofGetWidth(), ofGetHeight(), ofPoint((ofGetWidth()-ofGetHeight())/2,ofGetHeight()), ofGetHeight()/4);
	
	b2BodyDef bodyDef;
	m_groundBody = world.CreateBody(&bodyDef);
	
	//world.SetDebugDraw(&m_debugDraw);
	
	//uint32 flags = b2DebugDraw::e_shapeBit | b2DebugDraw::e_jointBit | b2DebugDraw::e_centerOfMassBit ;
	//flags += settings->drawJoints			* b2DebugDraw::e_jointBit;
	//	flags += settings->drawAABBs			* b2DebugDraw::e_aabbBit;
	//	flags += settings->drawPairs			* b2DebugDraw::e_pairBit;
	//	flags += settings->drawCOMs				* b2DebugDraw::e_centerOfMassBit;
	//m_debugDraw.SetFlags(flags);
	
	velocityIterations = 5;
	positionIterations = 2;
	timeStep = 1.0f/60.0; // framerate
	
}

void testApp::fillVideoTile(videoTile &t,ofRectangle rect,ofRectangle videoRect) {
	float aspectRatio = videoRect.width/videoRect.height;
	
	t.rect = rect;
	t.tex.x = (t.rect.x-videoRect.x)/videoRect.height/aspectRatio;
	t.tex.y = (t.rect.y-videoRect.y)/videoRect.height;
	t.tex.width = t.rect.width/videoRect.width;
	t.tex.height = t.rect.height/videoRect.height;
		
}


void testApp::setupTiles() {
	
	float aspectRatio = streamer.getWidth()/streamer.getHeight();
	
	videoRect.width = ofGetHeight()*aspectRatio;
	videoRect.height = ofGetHeight();
	videoRect.x = (ofGetWidth() - videoRect.width)/2;
	videoRect.y = 0;
//	
//	tile.tex.x = 0;
//	tile.tex.y = 0;
//	tile.tex.width = 1;
//	tile.tex.height = 1;
	
	ofRectangle margin;
	margin.x = videoRect.x;
	margin.y = 0;
	margin.width = (videoRect.width-videoRect.height)/2;
	margin.height = videoRect.height;
	fillVideoTile(leftMargin, margin, videoRect);
	margin.x+=videoRect.width-margin.width;
	fillVideoTile(rightMargin, margin, videoRect);
	
	
	offset = leftMargin.rect.x+leftMargin.rect.width;
	
	{
		vector<b2Vec2 > points;
		points.push_back(b2Vec2(-0.05,-0.05));
		points.push_back(b2Vec2(-0.05,4.05));
		points.push_back(b2Vec2(4.05,4.05));
		points.push_back(b2Vec2(4.05,-0.05));
		
		for (vector<b2Vec2>::iterator iter = points.begin();iter!=points.end();iter++) {
			vector<b2Vec2>::iterator niter= iter+1;
			if (niter==points.end()) {
				niter=points.begin();
			}
			
			b2Body* edge = NULL;
			{
				b2BodyDef bd;
				edge = world.CreateBody(&bd);
				
				
				
				b2PolygonShape shape;
				
				shape.SetAsEdge(*iter,*niter);
				
				b2FixtureDef fd;
				fd.shape = &shape;
				
				
				edge->CreateFixture(&fd);
			}
		}
	}
		
		
	for (int i=0; i<tilesPerRow*tilesPerRow-1	; i++) {
		
		videoTile t;
		ofRectangle rect;
		rect.x = (i%tilesPerRow)*tileSize+leftMargin.rect.x+leftMargin.rect.width;
		rect.y = (int)(i/tilesPerRow)*tileSize;
		rect.width = tileSize;
		rect.height = tileSize;
		
		
		
		ofRectangle * tex= new ofRectangle;
		
		float videoAspectRatio = videoRect.width/videoRect.height;
		
		tex->x = (rect.x-videoRect.x)/videoRect.height/videoAspectRatio;
		tex->y = (rect.y-videoRect.y)/videoRect.height;
		tex->width = rect.width/videoRect.width;
		tex->height = rect.height/videoRect.height;
		
		
		float shapeSize = 1.0;
		
		b2BodyDef bd;
		bd.type = b2_dynamicBody;
		bd.position.Set((i%4+0.5)*shapeSize, (3-int(i/4)+0.5)*shapeSize);
		bd.userData = tex;
		b2Body* body = world.CreateBody(&bd);
		
		b2PolygonShape shape;
		shape.SetAsBox(shapeSize/2.0f, shapeSize/2.0f);
		
		b2FixtureDef fd;
		fd.shape = &shape;
		fd.friction = 0.5f;
		fd.density = 1.0f;
		fd.restitution = 0.0f;
		body->CreateFixture(&fd);
			
		
			
		
		
		
	}
	
	
	
//	current=tiles.end();
		
	
	
	

	
	//tile.tex.x = (float)pos.x/(float)ofGetWidth();
	//tile.tex.y = (float)pos.y/(float)ofGetHeight();
}


//--------------------------------------------------------------
void testApp::update(){
	//image.update();
	streamer.update();
	
	if (!bStarted && streamer.getIsStreaming()) {
		bStarted = true;
		setupTiles();
	}
	
	
	
	world.Step( timeStep, velocityIterations, positionIterations); //
	world.ClearForces();
	
}

void testApp::setGravity(float x,float y) {
	
	//printf("x = %f   y = %f \n",x,y  );
	float tVectorLength=sqrt(x*x+y*y);
	float newGravityX=9.81f*x/tVectorLength;
	float newGravityY=9.81f*y/tVectorLength;
	world.SetGravity(b2Vec2(-newGravityY,newGravityX));
	// TODO: the flip is because of the iDevice rotation
	
	
}

//--------------------------------------------------------------
void testApp::draw(){
	
	if (!streamer.getIsStreaming()) {
		return;
	}
	
	//ofPushMatrix();
	//ofTranslate(pos.x, pos.y, 0);
	//ofScale(0.5, 0.5, 1);
	
	ofSetColor(255, 255, 255);
	
	streamer.draw(leftMargin.rect,leftMargin.tex);
	streamer.draw(rightMargin.rect,rightMargin.tex);
	
	
//	for(vector<videoTile>::iterator iter = tiles.begin();iter!=tiles.end();iter++) {
//		streamer.draw(iter->rect,iter->tex);
//	}
	//streamer.draw(videoRect,tile.tex);
	//ofPopMatrix();
	
	coordinator.pushTransform();
	//ofFill();
	//ofRect(0, 0, 8, 8);
	
	//world.DrawDebugData();
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, streamer.getTexture());
	glEnable(GL_TEXTURE_2D);
	
	for (b2Body* b = world.GetBodyList(); b; b = b->GetNext())
	{
		const b2Transform& xf = b->GetTransform();
		for (b2Fixture* fixture = b->GetFixtureList(); fixture; fixture = fixture->GetNext())
		{
			ofRectangle *vt = (ofRectangle*)b->GetUserData();
			if (vt)
			{
				
				b2Color color = b2Color(0.5f, 0.5f, 0.3f);
				
				switch (fixture->GetType())
				{
					case b2Shape::e_circle:
					
					break;
						
					case b2Shape::e_polygon:
					{
						b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
						int32 vertexCount = poly->m_vertexCount;
						b2Assert(vertexCount <= b2_maxPolygonVertices);
						
						
						ofRectangle rect;
						rect.x = b2Mul(xf, poly->m_vertices[0]).x;
						rect.y = b2Mul(xf, poly->m_vertices[0]).y;
						rect.width = b2Mul(xf, poly->m_vertices[2]).x-rect.x;
						rect.height = b2Mul(xf, poly->m_vertices[2]).y -rect.y;
						
						ofRectangle tex =*vt;
						
						
						GLfloat spriteTexcoords[] = {
							tex.x+tex.width,tex.y,   
							tex.x+tex.width,tex.y+tex.height,
							tex.x,tex.y,   
							tex.x,tex.y+tex.height,};
						
						//	float w =videoReader.width*u;//ofGetWidth()/2;
						//	float h =videoReader.height*v;//(float)ofGetWidth()/(float)width*(float)height/2;
						
						GLfloat spriteVertices[] =  {
							rect.x+rect.width,rect.y+rect.height,0,   
							rect.x+rect.width,rect.y,0,   
							rect.x,rect.y+rect.height,0, 
							rect.x,rect.y,0};
						
						
						glVertexPointer(3, GL_FLOAT, 0, spriteVertices);
						glTexCoordPointer(2, GL_FLOAT, 0, spriteTexcoords);	
						
						
						
						glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
						
						
						
						
						
						//GLfloat spriteTexcoords[] = {
//							tex.x,tex.y+tex.height,
//							tex.x+tex.width,tex.y+tex.height,
//							tex.x+tex.width,tex.y,   
//							tex.x,tex.y,   
//							};
//						
//												
//						
//						glVertexPointer(2, GL_FLOAT, 0, vertices);
//						glTexCoordPointer(2, GL_FLOAT, 0, spriteTexcoords);	
//						//glColor4f(color.r, color.g, color.b,0.5f);
//						glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
//						//glColor4f(color.r, color.g, color.b,1);
//						glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
						
					}
						break;
				}
			}
			
		}
	}
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
		
	coordinator.popTransform();
	
	
	ofSetColor(255, 0, 0);
	
	char str[256];
	sprintf(str,"%d\n%d\n%2.2f\n%2.2f\n", streamer.getElapsedFrame(),streamer.getCurrentFrame(),streamer.getFrameRate(),ofGetFrameRate());
	//std::ostringstream ss;
	//ss << streamer.getFrameRate() << "\n" << streamer.getCurrentFrame() << "\n" << ofGetFrameRate() << "\n" ;
	
//	for (int i=0; i<4; i++) {
//		ss << rowCounters[i] << " " << columnCounters[i] << "\n";
//	}
	
	
	
	
//	if (current!=tiles.end()) {
//		ss << "touch" << "\n";
//	}
	
	ofDrawBitmapString(str, 0, 20);
	
	//ofSetFrameRate(sfps);
	
		
	
	//ofDisableAlphaBlending();
	
	//--------------------------- rectangles
//	ofFill();
//	for (int i = 0; i < 200; i++){
//		ofSetColor((int)ofRandom(0,255),(int)ofRandom(0,255),(int)ofRandom(0,255));
//		ofRect(ofRandom(250,350),ofRandom(250,350),ofRandom(10,20),ofRandom(10,20));
//	}
//	ofSetColor(0x000000);
//	ofDrawBitmapString("rectangles", 275,500);
//
//
//	pincher.draw();

}




bool testApp::getIsInside(ofRectangle &rect,ofPoint p) {
	
	return p.x>rect.x && p.x<rect.x+rect.width && p.y>rect.y && p.y<rect.y+rect.width;
	
	
}

bool testApp::getIsIntersecting(ofRectangle &r1,ofRectangle &r2) {
	return getIsInside(r1, ofPoint(r2.x,r2.y)) || getIsInside(r1, ofPoint(r2.x,r2.y+r2.height)) ||
	getIsInside(r1, ofPoint(r2.x+r2.width,r2.y)) || getIsInside(r1, ofPoint(r2.x+r2.width,r2.y+r2.height)) ||
	getIsInside(r1, ofPoint(r2.x+r2.width/2,r2.y)) || getIsInside(r1, ofPoint(r2.x+r2.width/2,r2.y+r2.height)) ||
	getIsInside(r1, ofPoint(r2.x,r2.y+r2.height/2)) || getIsInside(r1, ofPoint(r2.x+r2.width,r2.y+r2.height/2));
}


class QueryCallback : public b2QueryCallback
{
public:
	QueryCallback(const b2Vec2& point)
	{
		m_point = point;
		m_fixture = NULL;
	}
	
	bool ReportFixture(b2Fixture* fixture)
	{
		b2Body* body = fixture->GetBody();
		if (body->GetType() == b2_dynamicBody)
		{
			bool inside = fixture->TestPoint(m_point);
			if (inside)
			{
				m_fixture = fixture;
				
				// We are done, terminate the query.
				return false;
			}
		}
		
		// Continue the query.
		return true;
	}
	
	b2Vec2 m_point;
	b2Fixture* m_fixture;
};



void testApp::touchDown(int x, int y, int id) {
	
	ofPoint pos = coordinator.screenSpaceToWorldSpace(ofPoint(x,y));
	b2Vec2 p;
	p.Set(pos.x,pos.y);
	m_mouseWorld = p;
	
	if (m_mouseJoint != NULL)
	{
		return;
	}
	
	// Make a small box.
	b2AABB aabb;
	b2Vec2 d;
	d.Set(0.001f, 0.001f);
	aabb.lowerBound = p - d;
	aabb.upperBound = p + d;
	
	// Query the world for overlapping shapes.
	QueryCallback callback(p);
	world.QueryAABB(&callback, aabb);
	
	if (callback.m_fixture)
	{
		b2Body* body = callback.m_fixture->GetBody();
		b2MouseJointDef md;
		md.bodyA =m_groundBody;
		md.bodyB = body;
		md.target = p;
#ifdef TARGET_FLOAT32_IS_FIXED
		md.maxForce = (body->GetMass() < 16.0)? 
		(1000.0f * body->GetMass()) : float32(16000.0);
#else
		md.maxForce = 1000.0f * body->GetMass();
#endif
		m_mouseJoint = (b2MouseJoint*)world.CreateJoint(&md);
		body->SetAwake(true);
	}
	
}

void testApp::touchMoved(int x, int y, int id) {
	ofPoint pos = coordinator.screenSpaceToWorldSpace(ofPoint(x,y));
	b2Vec2 p;
	p.Set(pos.x,pos.y);
	m_mouseWorld = p;
	
	if (m_mouseJoint)
	{
		m_mouseJoint->SetTarget(p);
	}
}

void testApp::touchUp(int x, int y, int id) {
	if (m_mouseJoint)
	{
		world.DestroyJoint(m_mouseJoint);
		m_mouseJoint = NULL;
	}
}

/*
void testApp::touchDown(int x, int y, int id) {
	
	if (!bStarted) {
		return;
	}
	
	down = ofPoint(x,y);
	
	
	for (current=tiles.begin(); current!=tiles.end(); current++) {
		if (x>=current->rect.x && x<current->rect.x+current->rect.width && y>=current->rect.y && y<current->rect.y+current->rect.height) {
			break;
		}
	}		
	
	
	//	if (current!=tiles.end()) {
	//		cout << distance(tiles.begin(), current) << ": " <<current->rect.x << " " << current->rect.y << endl;
	//	}
	
	
	
}


void testApp::touchMoved(int x, int y, int id) {
	
	if (!bStarted) {
		return;
	}
	ofPoint diff = ofPoint(x,y)-down;
		
	
	if (current!=tiles.end() ) {
		
		ofPoint pos(current->rect.x/tileSize,current->rect.y/tileSize);
		
		
		if (pos.y==(int)pos.y) {
			current->rect.x+=diff.x;
			
			for (vector<videoTile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
				
				if (current==iter ) {
					continue;
				}
				
				if (getIsIntersecting(current->rect, iter->rect)) {
					//printf("x: %i and %i are intersecting\n",distance(tiles.begin(), current),distance(tiles.begin(), iter));
					
			
					if (current->rect.x+current->rect.width > iter->rect.x && current->rect.x+current->rect.width < iter->rect.x+iter->rect.width) {
						current->rect.x=iter->rect.x-current->rect.width;
					}
					if (current->rect.x > iter->rect.x  && current->rect.x < iter->rect.x+iter->rect.width) {
						current->rect.x=iter->rect.x+iter->rect.width;
						
					}
				}
			} 
			current->rect.x=ofClamp(current->rect.x, offset, tileSize*(tilesPerRow-1)+offset);
		} 
		
		pos= ofPoint(current->rect.x/tileSize,current->rect.y/tileSize);
		
		if (pos.x==(int)pos.x ) {
		
		
			current->rect.y+=diff.y;
			
				
			for (vector<videoTile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
				
				if (current==iter ) {
					continue;
				}
				
				if (getIsIntersecting(current->rect, iter->rect)) {
				
					//printf("y: %i and %i are intersecting\n",distance(tiles.begin(), current),distance(tiles.begin(), iter));
					
					if (current->rect.y+current->rect.height > iter->rect.y && current->rect.y+current->rect.height < iter->rect.y+iter->rect.height) {
						current->rect.y=iter->rect.y-current->rect.height;
					}
					
					if (current->rect.y > iter->rect.y  && current->rect.y < iter->rect.y+iter->rect.height) {
						current->rect.y=iter->rect.y+iter->rect.height;
						
					}
				}
			} 
			
			current->rect.y=ofClamp(current->rect.y, 0, tileSize*(tilesPerRow-1));
			
		}
	}
	
	down = ofPoint(x,y);
		
}

void testApp::touchUp(int x, int y, int id) {
	if (!bStarted) {
		return;
	}
	current = tiles.end();
	
		
}
*/
void testApp::touchDoubleTap(int x, int y, int id) {

}


void testApp::audioRequested( float * output, int bufferSize, int nChannels ) {
	
	streamer.audioRequested(output, bufferSize, nChannels);

}



