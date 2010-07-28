#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofBackground(255,255,255);
	ofSetWindowTitle("osc example");
	
	// open an outgoing connection to HOST:SEND_PORT
	ofSetFrameRate(60);
	bSmooth = false;
	
	cout << "listening for osc messages on port " << RECEIVE_PORT << "\n";
	receiver.setup( RECEIVE_PORT );
	bGotHost = false;
	
	
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
	
	
	
	// display instructions
	string buf;
	buf = "sending osc messages to" + string( HOST ) + ofToString( SEND_PORT );
	ofDrawBitmapString( buf, 10, 20 );
	
	
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	if ( key =='a' || key == 'A' )
	{
		ofxOscMessage m;
		m.setAddress( "/test" );
		m.addIntArg( 1 );
		m.addFloatArg( 3.5f );
		m.addStringArg( "hello" );
		m.addFloatArg( ofGetElapsedTimef() );
		sender.sendMessage( m );
	}
	
	if (key == 's'){
		bSmooth = !bSmooth;
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	ofPoint p(x,y); 
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
void testApp::mousePressed(int x, int y, int button){
	ofPoint p(x,y);
	curve.push_back(p);
	
	detector.setup();
	detector.addPoint(p);
	
	currentCorner = 0;
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	curve.clear();

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

