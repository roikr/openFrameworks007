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
	iPhoneSetOrientation(OFXIPHONE_ORIENTATION_PORTRAIT);
	
	ofBackground(127,127,127);
    
    
    ofFill();
    
    /* open an alert with an OK button */
    NSString *message = @"Only one Facebook upload per account is allowed. You can still send your photos by e-mail.";
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Oops." message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil];
    [alert show];
    [alert release];
    
    int sscale = [[UIScreen mainScreen] scale];
    ofRectangle rect(0,0,200,50) ;
    
    keyboard = new ofxiPhoneKeyboard(rect.x/sscale,rect.y/sscale,rect.width/sscale,rect.height/sscale);
	keyboard->setVisible(false);
    //	keyboard->setBgColor(255, 255, 255, 0);
	keyboard->setFontColor(255,255,255, 255);
	keyboard->setFontSize(52);
    //    keyboard->setText("roikr75@gmail.com");
    [keyboard->getTextField() setKeyboardType:UIKeyboardTypeEmailAddress];
    bKeyboard = false;

}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
    ofSetColor(180, 0, 0);
    ofRect(0, 0, 48, 32);
    ofSetColor(180);
	ofCircle(50, 160, 25);
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
    if (bKeyboard) {    
        keyboard->setVisible(false);
        cout << keyboard->getText();
    } else {
        keyboard->setText("");
        keyboard->setVisible(true);
        keyboard->openKeyboard();
    }
    bKeyboard =!bKeyboard;
    
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

