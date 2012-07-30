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
//	iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
    
    vector<pair<int, int> > modes = extView.listModes();
    
	
    ss << "External Screen: " << endl;
    
    for (vector<pair<int, int> >::iterator iter=modes.begin(); iter!=modes.end(); iter++) {
        ss << "Screen mode: " << iter->first << "x" << iter->second   << endl;
        
        if (iter->first == 1024 && iter->second == 768) {
            extView.setupMode(distance(modes.begin(), iter));
        }
        
    }
    

        
	ofBackground(127,127,127);
    
    
}

//--------------------------------------------------------------
void testApp::update(){
    if (extView.getIsActive()) {
        extView.begin();
        ofDrawBitmapString(ss.str(), 0,0);
        extView.end();
    }

}

//--------------------------------------------------------------
void testApp::draw(){
	ofDrawBitmapString(ss.str(), 0,0);
}

//--------------------------------------------------------------
void testApp::exit(){

}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){

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

