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
	prefs.direction = SLIDER_VERTICAL;
	prefs.pages.push_back(ofPoint(0,0));
	prefs.pages.push_back(ofPoint(0,400));
	prefs.pages.push_back(ofPoint(0,600));
	
	slider.setup(1.5,prefs);
	slider.setPage(1);
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
	ofRect(0+10, 10, 400-20, 400-20);
	ofSetColor(0, 255, 0);
	ofRect(400+10, 10, 200-20, 200-20);
	ofRect(10, 400+10, 200-20, 200-20);
	ofSetColor(0, 0, 255);
	ofRect(600+10, 10, 300-20, 300-20);
	ofRect(10, 600+10, 300-20, 300-20);
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

