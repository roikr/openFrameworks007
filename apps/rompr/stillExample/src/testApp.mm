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
	//iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_LEFT);
	
	ofBackground(127,127,127);
    
    ofSetFrameRate(60);
}

//--------------------------------------------------------------
void testApp::update(){
    cam.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	float aspectRatio = cam.getWidth()/cam.getHeight();
    float width = ofGetHeight()*aspectRatio;
    ofRectangle rect((ofGetWidth() - width)/2,0,width,ofGetHeight());
    
    if (cam.getIsPlaying()) {
        cam.draw(rect, ofRectangle(0,0,1,1));
    } else {
        if (cam.image.isAllocated()) {
            cam.image.draw(0, 0);
        }
    }
    
	
    glColor4f(1,1,1,1);
	ofDrawBitmapString("of fps: " + ofToString( ofGetFrameRate(), 1 ),  10, ofGetHeight() - 50 );
	ofDrawBitmapString("video fps: " + ofToString( cam.getFrameRate(), 1 ),  10, ofGetHeight() - 30 );
    ofDrawBitmapString(ofToString( cam.getWidth())+" "+ofToString(cam.getHeight()),  10, ofGetHeight() - 10 );
}

//--------------------------------------------------------------
void testApp::exit(){

}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
    if (cam.getIsPlaying()) {
        cam.snap();
    }
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
    
    if (cam.getIsPlaying()) {
        cam.stop();
    } else {
        cam.preview();
    }
   
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

