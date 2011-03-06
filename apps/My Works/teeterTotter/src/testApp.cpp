#include "testApp.h"


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
	angle = -20;
	kinect.setCameraTiltAngle(angle);
	
	// start from the front
	pointCloudRotationY = 180;
	
	drawPC = false;
	
	mode = TEETER_MODE_IDLE;
	
}

//--------------------------------------------------------------
void testApp::update() {
	ofBackground(100, 100, 100);
	
	kinect.update();
	if(kinect.isFrameNew())	// there is a new frame and we are connected
	{

		if (!normals.empty()) {
			grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
			
			
			unsigned char * pix = grayImage.getPixels();
			int numPixels = grayImage.getWidth() * grayImage.getHeight();
			
			
			
			for(int i = 0; i < grayImage.getHeight(); i++){
				for(int j = 0; j < grayImage.getWidth(); j++){
					int c = i*(int)grayImage.getWidth()+j;
					ofxVec3f pos = kinect.getWorldCoordinateFor(j, i);

					int k; 
					for (k=0; k<5; k++) {
						if (pos.dot(normals[k]) > points[k % 4].dot(normals[k] - 0.01) ) {
							break;
						}
					}
					pix[c] =  k==5 ? pix[c] : 0;
					
										
					
				}
			}
			
			//update the cv image
			grayImage.flagImageChanged();
			
			// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
			// also, find holes is set to true so we will get interior contours as well....
			//contourFinder.findContours(grayImage, 1000, (kinect.width*kinect.height)/2, 1000, false);
			
		}
		
		
		
		/*
		//we do two thresholds - one for the far plane and one for the near plane
		//we then do a cvAnd to get the pixels which are a union of the two thresholds.	
		if( bThreshWithOpenCV ){
			grayThreshFar = grayImage;
			grayThresh = grayImage;
			grayThresh.threshold(nearThreshold, true);
			grayThreshFar.threshold(farThreshold);
			cvAnd(grayThresh.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
		}else{
		
			//or we do it ourselves - show people how they can work with the pixels
		
			unsigned char * pix = grayImage.getPixels();
			int numPixels = grayImage.getWidth() * grayImage.getHeight();

			for(int i = 0; i < numPixels; i++){
				if( pix[i] < nearThreshold && pix[i] > farThreshold ){
					pix[i] = 255;
				}else{
					pix[i] = 0;
				}
			}
		}

		//update the cv image
		grayImage.flagImageChanged();
	
		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
    	// also, find holes is set to true so we will get interior contours as well....
    	contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);
		 */
	}
}

void outVector(stringstream &reportStream,ofxVec3f vec) {
	reportStream << "(" << vec.x << "," << vec.y << "," << vec.z << ")";
}

//--------------------------------------------------------------
void testApp::draw() {
	ofSetColor(255, 255, 255);
	if(drawPC){
		ofPushMatrix();
		ofTranslate(420, 320);
		// we need a proper camera class
		drawPointCloud();
		ofPopMatrix();
	}else{
		kinect.drawDepth(10, 10, 400, 300);
		kinect.draw(420, 10, 400, 300);

		grayImage.draw(10, 320, 400, 300);
		//contourFinder.draw(10, 320, 400, 300);
		if (!contourFinder.blobs.empty()) {
			ofxCvBlob &blob = contourFinder.blobs.front();
			float x = 10;
			float y =320;
			float scalex = 400.0f/grayImage.getWidth();
			float scaley = 300.0f/grayImage.getHeight();
			
						
			ofPushStyle();
			// ---------------------------- draw the bounding rectangle
			ofSetColor(0xDD00CC);
			glPushMatrix();
			glTranslatef( x, y, 0.0 );
			glScalef( scalex, scaley, 0.0 );
			
			ofNoFill();
			ofRect( blob.boundingRect.x, blob.boundingRect.y,  blob.boundingRect.width, blob.boundingRect.height );
			
			
			// ---------------------------- draw the blobs
			ofSetColor(0x00FFFF);
			
			
			ofNoFill();
			ofBeginShape();
			for( int j=0; j<blob.nPts; j++ ) {
				ofVertex( blob.pts[j].x, blob.pts[j].y );
			}
			ofEndShape();
				
			
			glPopMatrix();
			ofPopStyle();
		}
		
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
	
	/*
	if (!normals.empty()) {
		reportStream << "points: ";
		outVector(reportStream, points[0]);
		reportStream << " ";
		outVector(reportStream, points[1]);
		reportStream << " ";
		outVector(reportStream, points[2]);
		reportStream << " ";
		outVector(reportStream, points[3]);
		reportStream << endl << "normals: ";
		outVector(reportStream, normals[0]);
	}
	 */
	
	
				
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
			points.clear();
			normals.clear();
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
		case TEETER_MODE_PLANE:
			points.push_back(kinect.getWorldCoordinateFor(kx*kinect.getWidth(), ky*kinect.getHeight()));
			
			if (points.size()>=4) {
				mode=TEETER_MODE_IDLE;
				ofxVec3f floorNormal = -(points[1]-points[0]).cross(points[2]-points[0]);
				floorNormal.normalize();
				
				
				for (int i=0; i<4; i++) {
					ofxVec3f normal = -(points[(i+1) % 4]-points[i]).cross(floorNormal);
					normal.normalize();
					normals.push_back(normal);
				}
							
				normals.push_back(-floorNormal);
				//if (m_normal[2] < 0) {
//					m_normal = -m_normal;
//				}
				
				//m_ground_plane = Plane(normal, p0);
			}
			break;
		default:
			break;
	}
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}

