#include "testApp.h"
#include "easing.h"
enum {
	MOUSE_MODE_IDLE,
	MOUSE_MODE_PLANE,
	MOUSE_MODE_ROI
};

enum {
	
	DISPLAY_MODE_GAME,
	DISPLAY_MODE_DEBUG_GAME,
	DISPLAY_MODE_CALIB,
	DISPLAY_MODE_POINT_CLOUD
};


	

void testApp::createTeeters() {
	
	if (!teeters.empty()) {
		for (vector<Teeter*>::iterator iter=teeters.begin(); iter!=teeters.end(); iter++) {
			delete (*iter);
		}
		teeters.clear();
	}
	
	float32 m = 4.0f;
	Teeter *teeter = new Teeter(&m_world,m,b2Vec2(0.0f,0.0f),ground,false,SCALING_FACTOR*m);
	teeters.push_back(teeter);
	
	for (int i=0; i<NUM_TEETERS-2; i++) {
		m*=SCALING_FACTOR;
		teeter = new Teeter(&m_world,m,teeter->getNextPosition(),teeter->getTeeterBody(),false,SCALING_FACTOR*m);
		teeters.push_back(teeter);
	}
	m*=SCALING_FACTOR;
	teeter = new Teeter(&m_world,m,teeter->getNextPosition(),teeter->getTeeterBody(),true,0.0f);
	teeters.push_back(teeter);
	
	
	citer = teeters.end()-1;
	
	bTrans = false;
	bJump = false;
	bReset = false;
	(*citer)->setFocus(b2Vec2(rect.x+rect.width/2,rect.y+rect.height));
}


//--------------------------------------------------------------
void testApp::setupPhysics() {
	
	m_world.SetContactListener(this);
	m_world.SetDebugDraw(&m_debugDraw);
	
	uint32 flags = b2DebugDraw::e_shapeBit | b2DebugDraw::e_jointBit | b2DebugDraw::e_centerOfMassBit ;
	
	//	flags += settings->drawJoints			* b2DebugDraw::e_jointBit;
	//	flags += settings->drawAABBs			* b2DebugDraw::e_aabbBit;
	//	flags += settings->drawPairs			* b2DebugDraw::e_pairBit;
	//	flags += settings->drawCOMs				* b2DebugDraw::e_centerOfMassBit;
	m_debugDraw.SetFlags(flags);
	
	
	b2BodyDef bd;
	//bd.type = b2_dynamicBody;
	ground = m_world.CreateBody(&bd);
	
	b2PolygonShape groundShape;
	groundShape.SetAsEdge(b2Vec2(-400.0f, 0.0f), b2Vec2(400.0f, 0.0f));
	groundFixture=ground->CreateFixture(&groundShape,1.0f);
	
	float32 nextt = 4.0f;
	float32 nextp = nextt/2.0f;
	b2Vec2 p1(-nextp/2.0f,0.0f);
	b2Vec2 p2(nextp/2.0f,0.0f);
	b2Vec2 p3(0.0f, 1.5*nextt-(nextp/10.f));
	
	b2Vec2 vertices[3];
	
	vertices[0] = p1;
	vertices[1] = p2;
	vertices[2] = p3;
	
	b2PolygonShape baseShape;
	baseShape.Set(vertices, 3);
	baseFixture = ground->CreateFixture(&baseShape,1.0f);
	
	
	
	ofSetFrameRate(60);
	
	velocityIterations = 6;
	positionIterations = 2;
	timeStep = 1.0f/60.0f;
		
	coordinator.setup(ofGetWidth(), ofGetHeight(), ofPoint(ofGetWidth()/2,ofGetHeight()), 20);
	
}

//--------------------------------------------------------------
void testApp::setup() {
	//kinect.init(true);  //shows infrared image
	kinect.init();
	kinect.setVerbose(true);
	kinect.open();

	colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	
	ofSetFrameRate(60);

	// zero the tilt on startup
	angle = 0 ;
//	kinect.setCameraTiltAngle(angle);
	
	// start from the front
	pointCloudRotationY = 180;
	
	
	
	mode = MOUSE_MODE_IDLE;
	displayMode = DISPLAY_MODE_GAME;
	
	
	coordinator.setup(ofGetWidth(), ofGetHeight(), ofPoint(ofGetWidth()/2,ofGetHeight()), 50);
	
	segmentator.load();
	segmentator.getROI(rect);
	
	setupPhysics();
	createTeeters();
	bShowCursor = false;
	ofHideCursor();
	ofBackground(255, 255, 255);
	
}

//--------------------------------------------------------------
void testApp::update() {
	//ofBackground(100, 100, 100);
	
	kinect.update();
	if(kinect.isFrameNew())	// there is a new frame and we are connected
	{

		if (segmentator.getIsReady()) {
			grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
			unsigned char * pix = grayImage.getPixels();
			int numPixels = grayImage.getWidth() * grayImage.getHeight();
			
			for(int i = 0; i < grayImage.getHeight(); i++){
				for(int j = 0; j < grayImage.getWidth(); j++){
					int c = i*(int)grayImage.getWidth()+j;
					ofPoint pnt=ofPoint(j,i);
					if (segmentator.getIsPointInside(pnt)) {
						ofxVec3f vec = kinect.getWorldCoordinateFor(j, i);
						if (segmentator.getIsVectorInside(vec)){
							continue;
						}
					}
					pix[c] = 0;
				}
			}
			
			//update the cv image
//			grayImage.erode_3x3();
//			grayImage.dilate_3x3();
//			grayImage.erode_3x3();
//			grayImage.dilate_3x3();
			grayImage.flagImageChanged();
			
			// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
			// also, find holes is set to true so we will get interior contours as well....
			contourFinder.findContours(grayImage, MIN_BLOB_AREA, (kinect.width*kinect.height)/2, 1, false);
			
			if (contourFinder.blobs.empty() ) {
				switch ((*citer)->getState() ) {
					case TEETER_STATE_RESTING:
						(*citer)->noBlob();
						centerTimer = ofGetElapsedTimeMillis();
						break;

					case TEETER_STATE_BALLANCED:
						if (!bReset) {
							next();
						}
						
						break;
						
					case TEETER_STATE_STARTED:
					case TEETER_STATE_UNBALLANCED:
						(*citer)->noBlob();
						if	(ofGetElapsedTimeMillis()-outTimer>OUT_DELAY) {
							leave();
						}
						break;
					default:						
						(*citer)->noBlob();
						break;
				}

			} else{
				ofxCvBlob &blob = contourFinder.blobs.front();
				(*citer)->updateBlob(blob);
				
				switch ((*citer)->getState()){
					case TEETER_STATE_RESTING:
						if (fabs(blob.centroid.x-(rect.x+rect.width/2)) > 30) {
							centerTimer = ofGetElapsedTimeMillis();
						} else {
							if (ofGetElapsedTimeMillis() - centerTimer>CENTER_DELAY) {
								startBlobY =blob.boundingRect.y+blob.boundingRect.height ;
								(*citer)->start(startBlobY);
							}
						}
						break;

					case TEETER_STATE_STARTED:
					case TEETER_STATE_UNBALLANCED:
						outTimer = ofGetElapsedTimeMillis();
						if (!bJump) {
							bJump =  startBlobY - (blob.boundingRect.y+blob.boundingRect.height) > 20;
						} else {
							if (startBlobY - (blob.boundingRect.y+blob.boundingRect.height) < 10) {
								jump();
								bJump = false;
							}
						}
						break;

				}

				
			} 

		}
		
		
		
	}
	
	
	
	m_world.Step(timeStep, velocityIterations, positionIterations);
	m_world.ClearForces();
	

	
		
	for (vector<Teeter*>::iterator iter=citer; iter!=teeters.end(); iter++) {
		(*iter)->update();
	}
	
	
	if (bTrans) {
		float t = (float)(ofGetElapsedTimeMillis() - animStart)/1000.0;
		if (t >= 1) {
			bTrans = false;
		} else {
			
			b2Vec2 npos;
			float32 nscale;
			(*citer)->getTransform(npos,nscale);
			scale = easeInOutQuad(t,scale,nscale);
			position.x = easeInOutQuad(t,position.x,npos.x);
			position.y = easeInOutQuad(t,position.y,npos.y);
		}
		
	}
	
	if (bReset) {
		if (ofGetElapsedTimeMillis()-resetTimer>RESET_DELAY) {
			createTeeters();
		}
		
		
		
	}
}


void testApp::DrawShape(b2Fixture* fixture, const b2Transform& xf)
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
	glEnd();
	
}


//--------------------------------------------------------------
void testApp::draw() {
	ofSetColor(255, 255, 255);
	
	
	switch (displayMode) {
		case DISPLAY_MODE_POINT_CLOUD:
			ofPushMatrix();
			ofTranslate(420, 320);
			// we need a proper camera class
			drawPointCloud();
			ofPopMatrix();
			break;
		case DISPLAY_MODE_CALIB: {
			kinect.drawDepth(10, 10, 400, 300);
			kinect.draw(420, 10, 400, 300);
			
			
			grayImage.draw(10, 320, 400, 300);
			contourFinder.draw(10,320, 400, 300);
			
			ofNoFill();
			ofRect(rect.x*400.0/kinect.getWidth()+10,rect.y*300/kinect.getHeight()+10,rect.width*400.0/kinect.getWidth(),rect.height*300/kinect.getHeight());
			
			
			
			//grayImage.drawROI(10, 320, 400, 300);
			
			//contourFinder.draw(10+roi.x*400/kinect.getWidth(), 320+roi.y*300/kinect.getHeight(), 400, 300);
			
			ofSetColor(255, 255, 255);
			stringstream reportStream;
			reportStream << "accel is: " << ofToString(kinect.getMksAccel().x, 2) << " / "
			<< ofToString(kinect.getMksAccel().y, 2) << " / " 
			<< ofToString(kinect.getMksAccel().z, 2) << endl
			<< "press p to switch between images and point cloud, rotate the point cloud with the mouse" << endl
			<<"num blobs found " << contourFinder.nBlobs
			<< ", fps: " << ofGetFrameRate() << endl
			<< "press c to close the connection and o to open it again, connection is: " << kinect.isConnected() << endl
			<< "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
			<< "press f to enter floor segmentation mode (4 CW clicks)" << endl
			<< "press r to enter 2D segmentation mode (drag rectangle)" << endl
			<< "press k to save segmentation data" << endl;
			
			//<< "diff: " << m_position-m_center << ", m_position: " << m_position <<  ", m_center: " << m_center << ", m_bias: " << m_bias << endl;
			
			if (!contourFinder.blobs.empty()) {
				ofxCvBlob &b = contourFinder.blobs.front();
				reportStream << "area: " << b.area << endl;
			} 
			
			ofDrawBitmapString(reportStream.str(),20,600);

			
		}	break;

		case DISPLAY_MODE_GAME:
		case DISPLAY_MODE_DEBUG_GAME:
		{
			
			
			//	ofPushMatrix();
			//	ofTranslate(ofGetWidth()/2, ofGetHeight(), 0);
			//	float scale = 1;
			//	ofScale(scale, -scale, 1.0);
			coordinator.pushTransform();
			
			if (bTrans) {
				ofScale(scale, scale, 1.0f);
				ofTranslate(-position.x, -position.y, 0.0f);
			} else {
				(*citer)->transform();
				
			}
			
			
			
			glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);	
			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
			glEnable(GL_LINE_SMOOTH);
			
			//why do we need this?
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			DrawShape(baseFixture,ground->GetTransform());
			
			for (vector<Teeter*>::iterator iter=teeters.begin(); iter!=teeters.end(); iter++) {
				(*iter)->draw();
			}
			
			
			
			for (vector<Teeter*>::iterator iter=citer; iter!=teeters.end(); iter++) {
				(*iter)->drawPlayer();
			}
			
			glPopAttrib();
			
			//m_world.DrawDebugData();
			
			coordinator.popTransform();
			//	ofPopMatrix();
			
			
			if (displayMode == DISPLAY_MODE_DEBUG_GAME) {
				ofSetColor(0, 0, 0);
				std::ostringstream ss;
				
				ss << ofGetFrameRate() <<  endl;
				(*citer)->debug(ss);
				
				ofDrawBitmapString(ss.str(), 20, 20);
			}
			
			
		} break;
	
			
		default:
			
			break;
	}
			
}

void testApp::drawPointCloud() {
	ofScale(400, 400, 400);
	int w = 640;
	int h = 480;
	ofRotateY(pointCloudRotationY);
	float* distancePixels = kinect.getDistancePixels();
	glBegin(GL_POINTS);
	int step = 2;
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			ofPoint cur = kinect.getWorldCoordinateFor(x, y);
			ofColor color = kinect.getCalibratedColorAt(x,y);
			glColor3ub((unsigned char)color.r,(unsigned char)color.g,(unsigned char)color.b);
			glVertex3f(cur.x, cur.y, cur.z);
		}
	}
	glEnd();
}




// Implement contact listener.
void testApp::BeginContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
	
	
	if (fixtureA->GetUserData() && fixtureB->GetUserData())
	{
		//if ((*citer)->getJoint() == fixtureA->GetUserData() || (*citer)->getJoint() == fixtureB->GetUserData() ) {
		if ( (fixtureA->GetUserData() && fixtureB->GetUserData()) || fixtureA == groundFixture || fixtureB == groundFixture) {

			cout << "contact" << endl;
		}
	}
	
}

void testApp::EndContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
	
	
	if ( (fixtureA->GetUserData() && fixtureB->GetUserData()) || fixtureA == groundFixture || fixtureB == groundFixture)
	{
		if ((*citer)->getJoint() == fixtureA->GetUserData() || (*citer)->getJoint() == fixtureB->GetUserData() ) {

			cout << "no contact" << endl;
		}
		
	}
	
}

void testApp::reset() {
	bReset = true;	
	resetTimer = ofGetElapsedTimeMillis();
}

void testApp::next() {
	if (citer!=teeters.begin()) {
		(*citer)->getTransform(position,scale);
		citer--;
		
		(*citer)->setFocus(b2Vec2(rect.x+rect.width/2,rect.y+rect.height));
		animStart = ofGetElapsedTimeMillis();
		bTrans = true;
	} else {
		reset();
	}

}

void testApp::jump() {
	
	for (vector<Teeter*>::iterator iter=citer; iter!=teeters.end(); iter++) {
		(*iter)->breakTeeter();
	}
	
	(*citer)->jump(distance(citer,teeters.end()));
	
	reset();
}

void testApp::leave() {
	for (vector<Teeter*>::iterator iter=citer; iter!=teeters.end(); iter++) {
		(*iter)->breakTeeter();
	}
	(*citer)->leave();
	
	reset();
}


//--------------------------------------------------------------
void testApp::keyPressed (int key) {
	
	if (key=='m') {
		displayMode = (displayMode+1) % 4;
		
		switch (displayMode) {
			case DISPLAY_MODE_GAME:
			case DISPLAY_MODE_DEBUG_GAME:
				ofBackground(255, 255, 255);
				break;
			default:
				ofBackground(0, 0, 0);
				break;
		}
		
		return;
	}
	
	switch (displayMode) {
		case DISPLAY_MODE_CALIB:
			switch (key) {

				case 'w':
					kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
					break;
				case 'o':
					kinect.setCameraTiltAngle(angle);	// go back to prev tilt
					kinect.open();
					break;
				case 'c':
					kinect.setCameraTiltAngle(0);		// zero the tilt
					kinect.close();
					break;
					
				case OF_KEY_UP:
					angle++;
					if(angle>30) angle=30;
					kinect.setCameraTiltAngle(angle);
					break;
					
				case OF_KEY_DOWN:
					angle--;
					if(angle<-30) angle=-30;
					kinect.setCameraTiltAngle(angle);
					break;
					
				case 'g':
					saveImage();
					break;
					
				case 'f':
					mode = MOUSE_MODE_PLANE;
					segmentator.clear();
					break;
				case 'k':
					segmentator.save();
					break;
					
				case 'r':
					mode = MOUSE_MODE_ROI;
					break;
					
				case 't':
					bShowCursor = !bShowCursor;
					if (bShowCursor) {
						ofShowCursor();
					} else{
						ofHideCursor();
					}

				
			} break;
			
			
				
		case DISPLAY_MODE_GAME:
		case DISPLAY_MODE_DEBUG_GAME:
			

			switch (key) {
									
				
				case 'n':
					next();
					break;
					
				case 't':
					ofToggleFullscreen();
					break;
					
				case 'l':
					leave();
					break;
				case 'j': 
					jump();
					break;
					
				case 'r':
					createTeeters();
					break;

									
			} break;

		
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y) {
	pointCloudRotationY = x;
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{
	if (mode == MOUSE_MODE_ROI) {
		ofRectangle rect;
		
		rect.x = (mouseDown.x-10.0)/400.0*kinect.getWidth();
		rect.y = (mouseDown.y-10.0)/300.0*kinect.getHeight();
		rect.width = (x-mouseDown.x)/400.0*kinect.getWidth();
		rect.height = (y-mouseDown.y)/300.0*kinect.getHeight();
		segmentator.setROI(rect);
		this->rect = rect;
	}
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
	float kx = float(x-10)/400;
	float ky = float(y-10)/300;
	
	
	
	switch (mode) {
		case MOUSE_MODE_PLANE: {
			ofxVec3f vec = kinect.getWorldCoordinateFor(kx*kinect.getWidth(), ky*kinect.getHeight());
			segmentator.addVector(vec);
			
			if (segmentator.getIsReady()) {
				mode=MOUSE_MODE_IDLE;
				
			}
		} break;
			
		case MOUSE_MODE_ROI:
			mouseDown = ofPoint(x,y);
						
			break;
			
		default:
			break;
	}
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{
		
	if (mode == MOUSE_MODE_ROI) {
		mode =MOUSE_MODE_IDLE;
	}
	
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}



//--------------------------------------------------------------
void testApp::exit() {
	//kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
}


void testApp::saveImage() {
	ofImage image;
	image.setFromPixels(kinect.getCalibratedRGBPixels(), kinect.getWidth(), kinect.getHeight(), OF_IMAGE_COLOR);
	image.setImageType(OF_IMAGE_COLOR_ALPHA);
	
	
	unsigned char * pix = kinect.getDepthPixels();
	int numPixels = kinect.getWidth() * kinect.getHeight();
	
	unsigned char * imgPix = image.getPixels();
	
	for(int i = 0; i < numPixels; i++){
		imgPix[i*4+3] = pix[i];
	}
	
	image.saveImage("grab.png");
}
	
