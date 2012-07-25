#include "testApp.h"
#include "ofxXmlSettings.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	camWidth 		= 320;	// try to grab at this size. 
	camHeight 		= 240;
	
	vidGrabber.setVerbose(true);
	vidGrabber.initGrabber(camWidth,camHeight);
    
    ofxXmlSettings xml;
    if (xml.loadFile("settings.xml")) {
        root = xml.getAttribute("settings", "root", "");
        xml.pushTag("settings");
        receiver.setup(xml.getAttribute("receiver", "port", 10001));
        sound.loadSound(xml.getAttribute("trigger", "sound", ""));
        delay = xml.getAttribute("trigger", "delay", 3000);
    }
    
	cout << root << endl;
    
    
    bTrigger = false;
   //	videoInverted 	= new unsigned char[camWidth*camHeight*3];
//	videoTexture.allocate(camWidth,camHeight, GL_RGB);	
}


//--------------------------------------------------------------
void testApp::update(){
	
	ofBackground(100,100,100);
	
	vidGrabber.grabFrame();
    
    if (bTrigger && ofGetElapsedTimeMillis()>delayTimer ) {
        bTrigger = false;
        image.setFromPixels(vidGrabber.getPixelsRef());
        stringstream ss;
        ss << "PHOTO_" << ofGetHours() << "_" << ofGetMinutes() << "_" << ofGetSeconds() << ".png";
        cout << ss.str() << endl;
        image.saveImage(root+"/"+ss.str());
        
        for (vector<ofxOscSender>::iterator iter=senders.begin(); iter!=senders.end(); iter++) {
            ofxOscMessage m;
            m.setAddress("/add");
            m.addStringArg(ss.str());
            iter->sendMessage(m);
        }
        
    }
	
//	if (vidGrabber.isFrameNew()){
//		int totalPixels = camWidth*camHeight*3;
//		unsigned char * pixels = vidGrabber.getPixels();
//		for (int i = 0; i < totalPixels; i++){
//			videoInverted[i] = 255 - pixels[i];
//		}
//		videoTexture.loadData(videoInverted, camWidth,camHeight, GL_RGB);
//	}

    // check for waiting messages
	while( receiver.hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );
        
		// check for mouse moved message
		if ( m.getAddress() == "/remove" ) {
			ofFile file(root+"/"+m.getArgAsString(0));
            if (file.exists()) {
                file.remove();
            }
            
		}  else if ( m.getAddress() == "/list" ) {
            
            string host = m.getRemoteIp();
            if (sendersMap.find(host)==sendersMap.end()) {
                sendersMap[host]=senders.size();
                ofxOscSender s;
                
                senders.push_back(s);
                
                
                int port = m.getArgAsInt32(0);
                senders.back().setup(host,port);
                
                cout <<host << "\t" << port << endl;
            }
            
            int senderNum = sendersMap[host];
            cout << senderNum << endl;
            ofxOscSender &sender = senders[senderNum];
            
            ofxOscMessage m;
                        
            ofDirectory dir(root);
            dir.listDir();
            for (int i=0;i<dir.size();i++) {
                m.clear();
                m.setAddress("/add");
                m.addStringArg(dir.getName(i));
                sender.sendMessage(m);
            }
            
		}
			
    }
}
        
    

//--------------------------------------------------------------
void testApp::draw(){
	ofSetHexColor(0xffffff);
	vidGrabber.draw(20,20);
    if (image.getTextureReference().bAllocated()) {
        image.draw(40,40,camWidth/3,camHeight/3);
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
	
	if (key == 's' || key == 'S'){
		vidGrabber.videoSettings();
	}
    
    if (key == ' ' && !bTrigger) {
        delayTimer = ofGetElapsedTimeMillis()+3000;
        bTrigger = true;
        sound.play();        
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
