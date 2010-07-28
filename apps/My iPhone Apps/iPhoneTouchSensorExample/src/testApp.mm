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
	
	receiver.setup( RECEIVE_PORT );
	bGotHost = false;
	bSmooth = true;

}

//--------------------------------------------------------------
void testApp::update(){
	while( receiver.hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );
		
		// check for mouse moved message
		if ( m.getAddress() == "/host" )
		{
			string host = m.getArgAsString(0);
			cout << "host: " << host << endl;
			sender.setup( host, SEND_PORT );
			bGotHost = true;
			
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	if (curve.size()>1) {
		
		if (bSmooth){
			ofEnableSmoothing();
		}
		
		
		ofSetColor(0xFF0000);
		
		
		
		for (vector<ofPoint>::iterator iter = curve.begin(); iter!= curve.end()-1 ; iter++){
			ofPoint p1=*iter;
			ofPoint p2=*(iter+1);
			ofLine(p1.x,p1.y,p2.x, p2.y);
		}
		
		if (bSmooth){
			ofDisableSmoothing();
		}
		
		
	}
	
}

//--------------------------------------------------------------
void testApp::exit(){

}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	if (touch.id != 0) 
		return;
	
	ofPoint p(touch.x,touch.y);
	curve.push_back(p);
	
	detector.setup();
	detector.addPoint(p);
	
	currentCorner = 0;
	
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
	if (touch.id != 0) 
		return;
	
	ofPoint p(touch.x,touch.y); 
	curve.push_back(p);
	
	detector.addPoint(p);
	detector.update();
	
	if (!bGotHost) {
		return;
	}
	ofxOscMessage m1;
	m1.setAddress( "/length" );
	m1.addIntArg(detector.getTotalPathLength());
	sender.sendMessage( m1 );
	
	
	
	if (detector.getNumCorners() > currentCorner) {
		float length = detector.getPathLength(0, detector.getCornerIndex(currentCorner));
		cout << "corner length: " << length << endl;
		ofxOscMessage m;
		m.setAddress( "/corner" );
		m.addIntArg(length);
		
		sender.sendMessage( m );
		currentCorner = detector.getNumCorners();
	}

}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
	if (touch.id != 0) 
		return;
	
	curve.clear();
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

