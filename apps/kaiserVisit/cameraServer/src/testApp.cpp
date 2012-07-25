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
        
        //xml.getAttribute("settings", "root", "");
        xml.pushTag("settings");
        receiver.setup(xml.getAttribute("receiver", "port", 10000));
        sound.loadSound(xml.getAttribute("trigger", "sound", ""));
        delay = xml.getAttribute("trigger", "delay", 3000);
        
        server = ofxHTTPServer::getServer(); // get the instance of the server
        server->setServerRoot("photos");		 // folder with files to be served
        server->start(xml.getAttribute("server", "port", 8888));
    }
    
	
    
    
    
    
    
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
        image.saveImage("photos/"+ss.str());
        
        ofxOscMessage m;
        m.setAddress("/add");
        m.addStringArg(ss.str());
        for (map<string,ofxOscSender*>::iterator iter=senders.begin(); iter!=senders.end(); iter++) {
            iter->second->sendMessage(m);
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
			ofFile file(ofToDataPath("photos/"+m.getArgAsString(0)));
            if (file.exists()) {
                file.remove();
            }
            
		}  else if ( m.getAddress() == "/list" ) {
            
            ofxOscSender *sender;
            
            string host = m.getRemoteIp();
            if (senders.find(host)==senders.end()) {
                
                int port = m.getArgAsInt32(0);
                sender = new ofxOscSender();
                sender->setup(host,port);
                
                senders[host]=sender;
                
                cout <<"new client: " << host << "\t" << port << endl;
            } else {
                sender = senders[host];
                cout <<"existing client: " << host << endl;
            }
            
            
            
                        
            ofDirectory dir(ofToDataPath("photos"));
            dir.listDir();
            for (int i=0;i<dir.size();i++) {
                ofxOscMessage m;
                m.setAddress("/add");
                m.addStringArg(dir.getName(i));
                sender->sendMessage(m);
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

void testApp::exit() {
    for (map<string,ofxOscSender*>::iterator iter=senders.begin(); iter!=senders.end(); iter++) {
        delete iter->second;
    }
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

