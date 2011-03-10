#include "testApp.h"

enum {
	TEETER_MODE_IDLE,
	TEETER_MODE_PLANE,
	TEETER_MODE_ROI
};

enum {
	DISPLAY_MODE_CALIB,
	DISPLAY_MODE_GAME,
	DISPLAY_MODE_POINT_CLOUD
};


//--------------------------------------------------------------
void testApp::setupPhysics() {
	
	m_world.SetDebugDraw(&m_debugDraw);
	
	uint32 flags = b2DebugDraw::e_shapeBit | b2DebugDraw::e_jointBit | b2DebugDraw::e_centerOfMassBit ;
	
	//flags += settings->drawJoints			* b2DebugDraw::e_jointBit;
	//	flags += settings->drawAABBs			* b2DebugDraw::e_aabbBit;
	//	flags += settings->drawPairs			* b2DebugDraw::e_pairBit;
	//	flags += settings->drawCOMs				* b2DebugDraw::e_centerOfMassBit;
	m_debugDraw.SetFlags(flags);
	
	
	b2PolygonShape shape;
	shape.SetAsEdge(b2Vec2(-40.0f, 0.0f), b2Vec2(40.0f, 0.0f));
	
	b2BodyDef bd;
	m_ground = m_world.CreateBody(&bd);
	m_ground->CreateFixture(&shape, 0.0f);
	
	
	teeter.setup(m_world, m_ground, 5);
	
	
	velocityIterations = 6;
	positionIterations = 2;
	timeStep = 1.0f/60.0f;
	m_bias = 1;
	
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
	angle = -30;
	kinect.setCameraTiltAngle(angle);
	
	// start from the front
	pointCloudRotationY = 180;
	
	drawPC = false;
	
	mode = TEETER_MODE_IDLE;
	displayMode = DISPLAY_MODE_CALIB;
	
	
	coordinator.setup(ofGetWidth(), ofGetHeight(), ofPoint(ofGetWidth()/2,ofGetHeight()), 50);
	
	setupPhysics();
	
	segmentator.load();
	
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
				teeter.updateBlob(blob);
			}
		}
		
		
		
	}
	
	
	
	m_world.Step(timeStep, velocityIterations, positionIterations);
	m_world.ClearForces();
	
	
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
		}	break;

		case DISPLAY_MODE_GAME: {
			
			ofEnableSmoothing();
			coordinator.pushTransform();
			m_world.DrawDebugData();
			teeter.draw();
			coordinator.popTransform();
			
			ofDisableSmoothing();
			
			ofSetColor(255, 255, 255);
			std::ostringstream ss;
			
			ss << ofGetFrameRate();
			
			
			ofDrawBitmapString(ss.str(), 20, 20);
			
		} break;
	
			
		default:
			
			break;
	}
			

			

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




//--------------------------------------------------------------
void testApp::keyPressed (int key) {
	switch (key) {
				break;
		case'p':
			drawPC = !drawPC;
			break;
	
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
			mode = TEETER_MODE_PLANE;
			segmentator.clear();
			break;
		case 'k':
			segmentator.save();
			break;
			
		case 'r':
			mode = TEETER_MODE_ROI;
			break;
			
		case 'm':
			displayMode = (displayMode+1) % 3;
			break;

		case OF_KEY_LEFT: 
			m_bias-=0.1;
			teeter.displace(m_bias);
			
			break;
			
		case OF_KEY_RIGHT: 
			m_bias+=0.1;
			teeter.displace(m_bias);
			break;
			
		case 's':
			teeter.setCenter();
			
			break;

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
		case TEETER_MODE_PLANE: {
			ofxVec3f vec = kinect.getWorldCoordinateFor(kx*kinect.getWidth(), ky*kinect.getHeight());
			segmentator.addVector(vec);
			
			if (segmentator.getIsReady()) {
				mode=TEETER_MODE_IDLE;
				
			}
		} break;
			
		case TEETER_MODE_ROI:
			mouseDown = ofPoint(x,y);
						
			break;
			
		default:
			break;
	}
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{
	if (mode == TEETER_MODE_ROI) {
		ofRectangle rect;
		
		rect.x = (mouseDown.x-10.0)/400.0*kinect.getWidth();
		rect.y = (mouseDown.y-10.0)/300.0*kinect.getHeight();
		rect.width = (x-mouseDown.x)/400.0*kinect.getWidth();
		rect.height = (y-mouseDown.y)/300.0*kinect.getHeight();
		segmentator.setROI(rect);
		//grayImage.setROI(rect);
		mode =TEETER_MODE_IDLE;
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
	
