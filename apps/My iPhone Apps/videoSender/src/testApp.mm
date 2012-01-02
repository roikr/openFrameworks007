#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	//If you want a landscape oreintation 
	//iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
	ofBackground(127,127,127);
	
	ofAddListener(gkReceiveDataEvent, this,&testApp::gkReceiveData);
	
	game.startPicker();
	
	video.fps = 25;
	video.numIntroFrames = 4;
	video.numFrames = 25;
	video.sampleLength = 1000*video.numFrames/video.fps;
	video.widthFraction = 1.0;
	video.heightFraction = 1.0;
	video.textureWidth = 64;
	video.textureHeight = 64;
	video.bHorizontal = true;
	video.bFlipHoriznotal = false;
	
	grabber.setup(&video,FRONT_CAMERA);
	
	bCameraOffset = false;
	
	buffer.allocate(video.textureWidth*video.textureHeight*4);
	
}

//--------------------------------------------------------------
void testApp::update(){
	if  (grabber.getState() == CAMERA_READY) {
        grabber.startCamera();
    }
	
    grabber.update();
    
    if (!bCameraOffset  && grabber.getState() >= CAMERA_RUNNING) {
        bCameraOffset = true;
        ofPoint offset;
//        float scale = 0.75;
//        offset = ofPoint((grabber.getCameraWidth()-video.textureWidth/scale)/2,0);
		
		float scale = 0.125;
		offset = ofPoint(0,0);
		
		grabber.setTransform(offset,scale);
        grabber.startCapture();
        
		
	}
	
	if (bAutoGrab && ofGetElapsedTimeMillis()-lastGrab>grabInterval) {
		bGrabPixels = true;
		lastGrab = ofGetElapsedTimeMillis();
	}
	
}

//--------------------------------------------------------------
void testApp::draw(){
	grabber.render(); //ofPoint((grabber.getCameraWidth()-video.textureWidth)/2,(grabber.getCameraHeight()-video.textureHeight)/2));
	
	[ofxiPhoneGetGLView() startRender];
	
	
	glViewport(0, 0, ofGetWidth(), ofGetHeight());
	
	
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluOrtho2D (0, ofGetWidth(), 0, ofGetHeight());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); 
	glScalef(1.0, -1.0,1.0);
	glTranslatef(0, -ofGetHeight(), 0);
	//	
	ofBackground(0,0,0);	
	ofSetHexColor(0xffffff);
	
	ofPushMatrix();
	
	ofTranslate(ofGetWidth(), 0, 0);
	ofRotate(90);
	
	ofPushMatrix();
	grabber.drawCamera();
	ofPopMatrix();
	
		
	
	
	if (grabber.getState()>=CAMERA_CAPTURING) {
		ofPushMatrix();
		ofTranslate(480-64, 320-64, 0.0f);
		
		grabber.draw();
		
		ofPopMatrix();
	}
	
	
	if (bGrabPixels) {
		bGrabPixels = false;
		int width = video.textureWidth;
		int height =  video.textureHeight;
		
		
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer.getBinaryBuffer());
		
		game.sendData(buffer);
		
		/*
		 NSInteger myDataLength = width * height * 4;
		 GLubyte *buffer = (GLubyte *) malloc(myDataLength);
		 glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		 texture.loadData(buffer,width,height,GL_RGBA);
		 free(buffer);
		 */
	}
	
	
	
	ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::exit(){

}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	//bGrabPixels = true;
	bAutoGrab = true;
	lastGrab = ofGetElapsedTimeMillis();
	grabInterval = 250.0+(float)touch.x/(float)ofGetWidth()*1750.0;
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
	bAutoGrab = false;
}

//--------------------------------------------------------------
void testApp::lostFocus(){

}

//--------------------------------------------------------------
void testApp::gotFocus(){

}

//--------------------------------------------------------------
void testApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void testApp::deviceOrientationChanged(int newOrientation){

}


//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs& args){

}

void testApp::gkReceiveData(gkReceiveDataEventArgs &args) {
	cout << args.data.getText() << endl;
}

