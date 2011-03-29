#include "testApp.h"
#include "easing.h"
enum {
	MOUSE_MODE_IDLE,
	MOUSE_MODE_PLANE,
	MOUSE_MODE_ROI
};

enum {
	DISPLAY_MODE_CALIB,
	DISPLAY_MODE_GAME,
	DISPLAY_MODE_POINT_CLOUD
};

enum {
	BLOB_STATE_INACTIVE,
	BLOB_STATE_ACTIVE,
	BLOB_STATE_CENTERED
};



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
	
	
	b2Body* ground = NULL;
	{
		
		
		b2BodyDef bd;
		//bd.type = b2_dynamicBody;
		ground = m_world.CreateBody(&bd);
		
		b2PolygonShape groundShape;
		groundShape.SetAsEdge(b2Vec2(-40.0f, 0.0f), b2Vec2(40.0f, 0.0f));
		
		b2FixtureDef groundFixtureDef;
		groundFixtureDef.shape=&groundShape;
		groundFixtureDef.filter.categoryBits = 0x0007;
		groundFixtureDef.filter.maskBits = 0x0007;
		m_groundFixture = ground->CreateFixture(&groundFixtureDef);
		
		
		
	}
	
	
	Teeter *teeter = new Teeter(&m_world,4.0f,ground,b2Vec2(0.0f,0.0f));
	teeters.push_back(teeter);
	
	teeter = new Teeter(&m_world,3.0f,teeter->getBody(),b2Vec2(-10.0f,4.0f));
	teeters.push_back(teeter);
	
	teeter = new Teeter(&m_world,2.0f,teeter->getBody(),b2Vec2(-15.0f,7.0f),true);
	teeters.push_back(teeter);
	
	current = teeters.end()-1;
	
	ofSetFrameRate(60);
	
	velocityIterations = 6;
	positionIterations = 2;
	timeStep = 1.0f/60.0f;
	m_stepCount = 0;
	
	coordinator.setup(ofGetWidth(), ofGetHeight(), ofPoint(ofGetWidth()/2,ofGetHeight()), 20);
	
	
	bTrans = false;
	
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
	angle = -10;
	kinect.setCameraTiltAngle(angle);
	
	// start from the front
	pointCloudRotationY = 180;
	
	
	
	mode = MOUSE_MODE_IDLE;
	displayMode = DISPLAY_MODE_CALIB;
	
	
	coordinator.setup(ofGetWidth(), ofGetHeight(), ofPoint(ofGetWidth()/2,ofGetHeight()), 50);
	
	setupPhysics();
	segmentator.load();
	ofRectangle rect;
	segmentator.getROI(rect);
	(*current)->setFocus(rect.x+rect.width/2);
	
	
	
	ofBackground(0, 0, 0);
	
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
			grayImage.erode_3x3();
			grayImage.dilate_3x3();
			grayImage.flagImageChanged();
			
			// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
			// also, find holes is set to true so we will get interior contours as well....
			contourFinder.findContours(grayImage, 1000, (kinect.width*kinect.height)/2, 1000, false);
			
			if (!contourFinder.blobs.empty() && displayMode == DISPLAY_MODE_GAME) {
				ofxCvBlob &blob = contourFinder.blobs.front();
				(*current)->updateBlob(blob);
			}
		}
		
		
		
	}
	
	
	
	m_world.Step(timeStep, velocityIterations, positionIterations);
	
	if (timeStep > 0.0f)
	{
		++m_stepCount;
	}
	
	(*current)->update();
	
	
	m_world.ClearForces();
	
	if (bTrans) {
		float t = (float)(ofGetElapsedTimeMillis() - animStart)/1000.0;
		if (t >= 1) {
			bTrans = false;
		} else {
			
			b2Vec2 npos;
			float32 nscale;
			(*current)->getTransform(npos,nscale);
			scale = easeInOutQuad(t,scale,nscale);
			position.x = easeInOutQuad(t,position.x,npos.x);
			position.y = easeInOutQuad(t,position.y,npos.y);
		}
		
	}
}



//--------------------------------------------------------------
void testApp::draw() {
	ofSetColor(255, 255, 255);
	ofRectangle roi;
	segmentator.getROI(roi);
	
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
			<< "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl;
			//<< "diff: " << m_position-m_center << ", m_position: " << m_position <<  ", m_center: " << m_center << ", m_bias: " << m_bias << endl;
			ofDrawBitmapString(reportStream.str(),20,666);

			
		}	break;

		case DISPLAY_MODE_GAME: {
			
			ofEnableSmoothing();
			//	ofPushMatrix();
			//	ofTranslate(ofGetWidth()/2, ofGetHeight(), 0);
			//	float scale = 1;
			//	ofScale(scale, -scale, 1.0);
			coordinator.pushTransform();
			
			if (bTrans) {
				ofScale(scale, scale, 1.0f);
				ofTranslate(-position.x, -position.y, 0.0f);
			} else {
				(*current)->transform();
				
			}
			
			
			
		
			
			m_world.DrawDebugData();
			for (vector<Teeter*>::iterator iter=current; iter!=teeters.end(); iter++) {
				
								
				
				(*iter)->draw();
			}
			
			coordinator.popTransform();
			//	ofPopMatrix();
			ofDisableSmoothing();
			
			ofSetColor(0, 0, 0);
			std::ostringstream ss;
			
			ss << ofGetFrameRate() << " " << m_stepCount << endl;
			(*current)->log(ss);
			
			ofDrawBitmapString(ss.str(), 20, 20);
			
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
		if ((*current)->getJoint() == fixtureA->GetUserData() || (*current)->getJoint() == fixtureB->GetUserData() ) {
			(*current)->setState(TEETER_STATE_TOUCHING);
			cout << "contact" << endl;
		}
	}
	
}

void testApp::EndContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
	
	
	if ( (fixtureA->GetUserData() && fixtureB->GetUserData()) || fixtureA == m_groundFixture || fixtureB == m_groundFixture)
	{
		if ((*current)->getJoint() == fixtureA->GetUserData() || (*current)->getJoint() == fixtureB->GetUserData() ) {
			(*current)->setState(TEETER_STATE_UNBALLANCED);
			cout << "no contact" << endl;
		}
		
	}
	
}


void testApp::nextTeeter() {
	if (current!=teeters.begin()) {
		(*current)->getTransform(position,scale);
		current--;
		ofRectangle rect;
		segmentator.getROI(rect);
		(*current)->setFocus(rect.x+rect.width/2);
		animStart = ofGetElapsedTimeMillis();
		bTrans = true;
	}
}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
	
	if (key=='m') {
		displayMode = (displayMode+1) % 3;
		
		switch (displayMode) {
			case DISPLAY_MODE_GAME:
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
				
			} break;
			
			
				
		case DISPLAY_MODE_GAME:
			switch (key) {
									
				case '1':
					current = teeters.begin();
					break;
				case '2':
					current = teeters.begin()+1;
					break;
				case '3':
					current = teeters.begin()+2;
					break;
					
				case 's':
					(*current)->start();
					
					break;
					
				case 'n':
					nextTeeter();
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
		ofRectangle rect;
		
		rect.x = (mouseDown.x-10.0)/400.0*kinect.getWidth();
		rect.y = (mouseDown.y-10.0)/300.0*kinect.getHeight();
		rect.width = (x-mouseDown.x)/400.0*kinect.getWidth();
		rect.height = (y-mouseDown.y)/300.0*kinect.getHeight();
		segmentator.setROI(rect);
		//grayImage.setROI(rect);
		mode =MOUSE_MODE_IDLE;
	}
	
	
	
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}



//--------------------------------------------------------------
void testApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
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
	
