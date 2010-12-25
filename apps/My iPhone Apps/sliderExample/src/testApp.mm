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
	
	ofBackground(0,0,0);
	sliderPrefs prefs;
	prefs.pages.push_back(ofPoint(0,0));
	prefs.pages.push_back(ofPoint(320,0));
	prefs.pages.push_back(ofPoint(480,0));
	
	slider.setup( 1.0,prefs);
}

//--------------------------------------------------------------
void testApp::update(){
	slider.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	slider.transform();
	//for (int i=0; i<3; i++) {
//		ofSetColor((i==0)*255, (i==1)*255, (i==2)*255);
//		ofFill();
//		ofRect(i*ofGetWidth()+10, 10, ofGetWidth()-20, ofGetHeight()-20);
//
//	}
		
	ofFill();
	ofSetColor(255, 0, 0);
	ofRect(0+10, 10, 320-20, ofGetHeight()-20);
	ofSetColor(0, 255, 0);
	ofRect(320+10, 10, 160-20, ofGetHeight()-20);
	ofSetColor(0, 0, 255);
	ofRect(480+10, 10, 320-20, ofGetHeight()-20);
	slider.draw();
	
}

//--------------------------------------------------------------
void testApp::exit(){

}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	slider.touchDown(touch.x, touch.y, touch.id);
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
	slider.touchMoved(touch.x, touch.y, touch.id);
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
	slider.touchUp(touch.x, touch.y, touch.id);
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
	slider.touchDoubleTap(touch.x, touch.y, touch.id);
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

