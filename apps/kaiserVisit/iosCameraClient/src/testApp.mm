#include "testApp.h"
#include "ofxXmlSettings.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofBackground( 100, 100, 100 );

	ofRegisterURLNotification(this);
	
    ofxXmlSettings xml;
    if (xml.loadFile("settings.xml")) {
        url = xml.getAttribute("settings", "url", "",0);
        xml.pushTag("settings");
        sender.setup(xml.getAttribute("sender", "host", ""),xml.getAttribute("sender", "port", 10000));
        int port = xml.getAttribute("receiver", "port", 10001);
        receiver.setup(port);
        
        ofxOscMessage m;
        m.setAddress("/list");
        m.addIntArg(port);
        sender.sendMessage(m);
        
    }
    
	cout << url << endl;
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(100,100,100);
	
    // check for waiting messages
	while( receiver.hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );
        
		// check for mouse moved message
		if ( m.getAddress() == "/add" ) {
            images.push_back(m.getArgAsString(0));
            cout << images.back() << endl;
            
		} 			
    }

}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetHexColor(0xffffff);
    
    if (image.getTextureReference().bAllocated()) {
        image.draw(20,20);
    }}

//--------------------------------------------------------------
void testApp::exit(){

}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    
    if (!images.empty()) {
        ofLoadURLAsync(url+'/'+images[MIN(floor(touch.y*images.size()/ofGetHeight()),images.size()-1)]);
    }
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
	if (!images.empty()) {
        ofLoadURLAsync(url+'/'+images[MIN(floor(touch.y*images.size()/ofGetHeight()),images.size()-1)]);
    }
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
	
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
    if (!images.empty()) {
        ofxOscMessage m;
        m.setAddress("/remove");
        m.addStringArg(images[0]);
        sender.sendMessage(m);
        images.erase(images.begin());
    }
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

void testApp::urlResponse(ofHttpResponse &response) {
    if (response.status == 200) {
        image.loadImage(response.data);
    }
}

