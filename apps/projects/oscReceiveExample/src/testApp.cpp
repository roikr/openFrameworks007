#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
    receiver.setup(PORT);
    
	mouseX = 0;
	mouseY = 0;
	mouseButtonState = "";

	ofBackground( 30, 30, 130 );
    
    current_msg_string = 0;
    
    ofAddListener(receiver.gotOscMessage, this, &testApp::gotOscMessage);
    
}

//--------------------------------------------------------------
void testApp::update(){
    // hide old messages
	for ( int i=0; i<NUM_MSG_STRINGS; i++ )
	{
		if ( timers[i] < ofGetElapsedTimef() )
			msg_strings[i] = "";
	}
}


//--------------------------------------------------------------
void testApp::draw(){

	string buf;
	buf = "listening for osc messages on port" + ofToString( PORT );
	ofDrawBitmapString( buf, 10, 20 );

	// draw mouse state
	buf = "mouse: " + ofToString( mouseX, 4) +  " " + ofToString( mouseY, 4 );
	ofDrawBitmapString( buf, 430, 20 );
	ofDrawBitmapString( mouseButtonState, 580, 20 );

	for ( int i=0; i<NUM_MSG_STRINGS; i++ )
	{
		ofDrawBitmapString( msg_strings[i], 10, 40+15*i );
	}



}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    mouseX = x;
	mouseY = y;
	mouseButtonState = ofToString( button ) ;;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

void testApp::gotOscMessage(ofxOscMessage &m) {
    // unrecognized message: display on the bottom of the screen
    string msg_string;
    msg_string = m.getAddress();
    msg_string += ": ";
    for ( int i=0; i<m.getNumArgs(); i++ )
    {
        // get the argument type
        msg_string += m.getArgTypeName( i );
        msg_string += ":";
        // display the argument - make sure we get the right type
        if( m.getArgType( i ) == OFXOSC_TYPE_INT32 )
            msg_string += ofToString( m.getArgAsInt32( i ) );
        else if( m.getArgType( i ) == OFXOSC_TYPE_FLOAT )
            msg_string += ofToString( m.getArgAsFloat( i ) );
        else if( m.getArgType( i ) == OFXOSC_TYPE_STRING )
            msg_string += m.getArgAsString( i );
        else
            msg_string += "unknown";
        
        // add to the list of strings to display
        msg_strings[current_msg_string] = msg_string;
        timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
        current_msg_string = ( current_msg_string + 1 ) % NUM_MSG_STRINGS;
        // clear the next line
        msg_strings[current_msg_string] = "";
    }
}