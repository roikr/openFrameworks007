#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){	 

	ofSetVerticalSync(true);

	
	ofBackground(255,255,255);	
	ofSetLogLevel(OF_LOG_NOTICE);

	
			
	serial = 0;
    pin = 0;
	
	serialTimer = ofGetElapsedTimeMillis();
    
	    
   
}

//--------------------------------------------------------------
void testApp::update(){
	
    string s;
    if (!serial) {
        cout << "\nconnecting" << endl;
//        serial = new ofxSerial("/dev/tty.usbmodemfd121", 9600);
        serial = new ofxSerial("/dev/tty.usbserial-A6006kiO", 9600);
        serialTimer = ofGetElapsedTimeMillis()+5000;
    } else {
        if (serial->readUntil(s, 'e')) {
            cout << s ;
            serialTimer = ofGetElapsedTimeMillis()+5000;
            pin = s[s.length()-2]-'0';
        } else {
            cout << ".";
            if (ofGetElapsedTimeMillis()>serialTimer) {
                serial = 0;
            }
        }
        
        
    } 
    
	
    
//    if (nRead) {
//        
//        cout << byte << endl;
//    }
    
    
    
    
   
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(255*pin);
	

}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
    if (serial) {
        const char s = key;
        serial->writeBytes(&s, 1);
    }
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

