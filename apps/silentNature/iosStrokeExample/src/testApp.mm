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
    ofSetColor(50);
    
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
    for (vector<ofVec2f>::iterator iter=stroke.getCurve().begin();iter!=stroke.getCurve().end();iter++) {
        ofRectangle rect;
        rect.setFromCenter(*iter, 1, 1);
        ofRect(rect);
    }
    
}

//--------------------------------------------------------------
void testApp::exit(){

}



//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    stroke.clear(5);
    stroke.touch(ofVec2f(touch.x,touch.y));
   
   
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
    
    stroke.touch(ofVec2f(touch.x,touch.y));
    
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){

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

