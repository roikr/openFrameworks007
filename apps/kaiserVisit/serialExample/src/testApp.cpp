#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){	 

	ofSetVerticalSync(true);

	
	ofBackground(255,255,255);	
	ofSetLogLevel(OF_LOG_NOTICE);

	
			
	serial.listDevices();
	vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
	
	//----------------------------------- note:
	// < this should be set
	// to whatever com port
	// your serial device is
	// connected to.
	// (ie, COM4 on a pc, /dev/tty.... on linux, /dev/tty... on a mac)
	// arduino users check in arduino app....
		
	//serial.setup(0, 9600); //open the first device

	//serial.setup("COM4");  						  // windows example
	//serial.setup("/dev/tty.usbserial-A4001JEC",9600); // mac osx example
	
    
    bSerial = false;
    serialRetry = ofGetElapsedTimeMillis();
    
   
}

//--------------------------------------------------------------
void testApp::update(){
    
    if (!bSerial && ofGetElapsedTimeMillis()>serialRetry) {
        bSerial = serial.setup("/dev/tty.usbmodemfd121", 9600);			  //linux example
        serialRetry = ofGetElapsedTimeMillis()+10000;
    }
	
	if (bSerial) {
        int byte = serial.readByte();
        switch (byte) {
            case OF_SERIAL_ERROR:
                bSerial = false;
               
                break;
            case OF_SERIAL_NO_DATA:
                cout << "no data" << endl;
                break;
            default:
                cout << byte << endl;
                break;
        }
   
    }
    
    
    
   
}

//--------------------------------------------------------------
void testApp::draw(){

	

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

