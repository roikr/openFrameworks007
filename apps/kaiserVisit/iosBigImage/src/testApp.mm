#include "testApp.h"



void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
	ofxiPhoneSetOrientation( OF_ORIENTATION_90_LEFT );	
	// initialize the accelerometer
	ofxAccelerometer.setup();	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
    
    image.loadImage(ofToDataPath("I1"),1024,5359,3792);
//    image.loadImage("I2.png",1024,2019,1639);
    
    float minZoom = (float)ofGetWidth()/(float)image.getWidth();
    //	cam.setZoom(0.125f);
    cam.setZoom(minZoom);
	cam.setMinZoom(minZoom);
	cam.setMaxZoom(2.0f);
	cam.setScreenSize( ofGetWidth(), ofGetHeight() );
    ofVec2f limit = 0.5*ofVec2f(image.getWidth(),image.getHeight());
    cam.setViewportConstrain( -limit, limit); //limit browseable area, in world units
	
    
    
       
    
}


void testApp::update() {
    cam.update();
    

     
}


void testApp::draw() {
    
    
	cam.apply(); //put all our drawing under the ofxPanZoom effect
    
    
    
    ofTranslate(-0.5*ofVec2f(image.getWidth(),image.getHeight()));
    image.draw();
    
    
    
    
	
	cam.reset();	//back to normal ofSetupScreen() projection
	
    
    
	cam.drawDebug(); //see info on ofxPanZoom status
	
	glColor4f(1,1,1,1);
	ofDrawBitmapString("fps: " + ofToString( ofGetFrameRate(), 1 ),  10, ofGetHeight() - 10 );	
}

void testApp::exit() {
    
}

void testApp::touchDown(ofTouchEventArgs &touch){

	cam.touchDown(touch);
	
}


void testApp::touchMoved(ofTouchEventArgs &touch){
	cam.touchMoved(touch);

}


void testApp::touchUp(ofTouchEventArgs &touch){
	cam.touchUp(touch);

}


void testApp::touchDoubleTap(ofTouchEventArgs &touch){
	cam.touchDoubleTap(touch);
}


void testApp::touchCancelled(ofTouchEventArgs& args){

}


void testApp::deviceOrientationChanged(int newOrientation){
	//not sure whiy this is inverted in horizontal modes, disabling for now
	//ofxiPhoneSetOrientation( (ofOrientation)newOrientation);
	//cam.setScreenSize(ofGetWidth(), ofGetHeight());
};
