#include "testApp.h"



void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
	ofxiPhoneSetOrientation( OF_ORIENTATION_90_LEFT );	
	// initialize the accelerometer
	ofxAccelerometer.setup();	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
    
    nav.setup();
    glFlush();
    
    vector<pair<int, int> > modes = extView.listModes();
    
    for (vector<pair<int, int> >::iterator iter=modes.begin(); iter!=modes.end(); iter++) {
        cout << iter->first << "x" << iter->second << endl;
        if (iter->first == 1280 && iter->second == 1024) {
            extView.setupMode(distance(modes.begin(), iter));
            break;
        }
        
    }
    
}


void testApp::update() {
   
    nav.update();
    
    if (extView.getIsActive()) {
        extView.begin();
        nav.draw2nd();
        extView.end();
    }

     
}


void testApp::draw() {
    nav.draw();
	
//	glColor4f(1,1,1,1);
//	ofDrawBitmapString("fps: " + ofToString( ofGetFrameRate(), 1 ),  10, ofGetHeight() - 10 );	
}

void testApp::exit() {
    nav.exit();
}

void testApp::touchDown(ofTouchEventArgs &touch){

	nav.touchDown(touch);
	
}


void testApp::touchMoved(ofTouchEventArgs &touch){
	nav.touchMoved(touch);

}


void testApp::touchUp(ofTouchEventArgs &touch){
	nav.touchUp(touch);

}


void testApp::touchDoubleTap(ofTouchEventArgs &touch){
	nav.touchDoubleTap(touch);
}


void testApp::touchCancelled(ofTouchEventArgs& args){

}


void testApp::deviceOrientationChanged(int newOrientation){
	//not sure whiy this is inverted in horizontal modes, disabling for now
	//ofxiPhoneSetOrientation( (ofOrientation)newOrientation);
	//cam.setScreenSize(ofGetWidth(), ofGetHeight());
};
