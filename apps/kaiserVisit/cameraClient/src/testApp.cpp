#include "testApp.h"
#include "ofxXmlSettings.h"

//--------------------------------------------------------------
void testApp::setup(){
	
    ofRegisterURLNotification(this);
	
    ofxXmlSettings xml;
    if (xml.loadFile("settings.xml")) {
        url = xml.getAttribute("settings", "url", "",0);
        xml.pushTag("settings");
        sender.setup(xml.getAttribute("sender", "host", ""),xml.getAttribute("sender", "port", 10001));
        int port = xml.getAttribute("receiver", "port", 10000);
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
    }
//	videoTexture.draw(20+camWidth,20,camWidth,camHeight);
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	
	// in fullscreen mode, on a pc at least, the 
	// first time video settings the come up
	// they come up *under* the fullscreen window
	// use alt-tab to navigate to the settings
	// window. we are working on a fix for this...
    
    if (key>='1' && key<='9'  && key-'1'<images.size()) {
        ofLoadURLAsync(url+'/'+images[key-'1']);
    }
	
	if ((key=='r' || key=='R') && !images.empty()) {
        ofxOscMessage m;
        m.setAddress("/remove");
        m.addStringArg(images[0]);
        sender.sendMessage(m);
        images.erase(images.begin());
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

void testApp::urlResponse(ofHttpResponse &response) {
    if (response.status == 200) {
        image.loadImage(response.data);
    }
}
