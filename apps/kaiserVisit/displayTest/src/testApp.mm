#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
    
    image.loadImage("1280_800_test.png",2048);
	
	//If you want a landscape oreintation 
	iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_LEFT);
    
    vector<pair<int, int> > modes = extView.listModes();
    
	
    ss << "External Screen: " << endl;
    
    
    if (!modes.empty()) {
        vector<pair<int, int> >::iterator iter;
        vector<pair<int, int> >::iterator maxIter = modes.begin();
        
        for (iter=modes.begin(); iter!=modes.end(); iter++) {
            ss << "Screen mode: " << iter->first << "x" << iter->second   << endl;
            
            if (iter->first == 1280 && iter->second == 800) {
                break;
            }
            
            if (iter->first * iter->second > maxIter->first * maxIter->second ) {
                maxIter = iter;
            }
        }
        
        if (iter==modes.end()) {
            iter = maxIter;
        }
        
        extView.setupMode(distance(modes.begin(), iter));
        ss << "Selected mode: " << iter->first << "x" << iter->second   << endl;
    }
    

        
	ofBackground(127,127,127);
    
    
}

//--------------------------------------------------------------
void testApp::update(){
    if (extView.getIsActive()) {
        extView.begin();
        image.draw();
        ofDrawBitmapString(ss.str(), 0,0);
        extView.end();
    }

}

//--------------------------------------------------------------
void testApp::draw(){
    image.draw();
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

