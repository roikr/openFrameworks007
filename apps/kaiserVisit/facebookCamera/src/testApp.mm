#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
//	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
//	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
//	ofxiPhoneAlerts.addListener(this);
	
	//If you want a landscape oreintation 
	iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_LEFT);
	
	ofBackground(127,127,127);
    ofxRegisterStillCameraNotification(this);
    cam.preview();

    
    ofxRegisterFacebookNotification(this);
    string strs[] = {"publish_actions", "user_photos"};
    fb.setup(vector<string>(strs,strs+2));
    if (!fb.getIsLoggedIn()) {
        fb.login();
    }
}

//--------------------------------------------------------------
void testApp::update(){
    cam.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	if (cam.getIsPlaying() && cam.getIsFrameVisible()) {
        float width = ofGetWidth(); 
        float height = width/cam.getWidth()*cam.getHeight();
        cam.draw(ofRectangle(0,0,width,height), ofRectangle(0,0,1,1));
    } 
}

//--------------------------------------------------------------
void testApp::exit(){
    fb.exit();
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
    if (fb.getIsLoggedIn()) {
        cam.snap();
    } 
}

//--------------------------------------------------------------
void testApp::lostFocus(){

}

//--------------------------------------------------------------
void testApp::gotFocus(){
    fb.gotFocus();
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

void testApp::launchedWithURL(string url) {
    fb.launchedWithURL(url);
    
}

void testApp::pictureTaken(ofPixels &pixels) {
    ofImage image;
    image.setFromPixels(pixels);
    cout << "pictureTaken: " << image.getWidth() << "\t" << image.getHeight() << endl;
    fb.postImage(image);
}

void testApp::facebookEvent(ofxFBEventArgs &args) {
    cout << "facebookEvent: " << args.message << endl;
}
