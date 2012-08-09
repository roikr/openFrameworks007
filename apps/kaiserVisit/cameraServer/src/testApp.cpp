#include "testApp.h"
#include "ofxXmlSettings.h"

#define EXTENSION "jpg"



//--------------------------------------------------------------
void testApp::setup(){
	
	
	
	
    
    ofxXmlSettings xml;
    if (xml.loadFile("settings.xml")) {
        camWidth = xml.getAttribute("settings", "width", 1280);
        camHeight = xml.getAttribute("settings", "width", 720);
        vidGrabber.setVerbose(true);
        vidGrabber.initGrabber(camWidth,camHeight);
        //xml.getAttribute("settings", "root", "");
        xml.pushTag("settings");
        receiver.setup(xml.getAttribute("receiver", "port", 10000));
        sound.loadSound(xml.getAttribute("trigger", "sound", ""));
        delay = xml.getAttribute("trigger", "delay", 3000);
        
        server = ofxHTTPServer::getServer(); // get the instance of the server
        server->setServerRoot("");		 // folder with files to be served
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
        
        float width = 1024.0;
        float height = 768.0;
        float scale = min((float)vidGrabber.getWidth()/width,(float)vidGrabber.getHeight()/height);
        int newWidth = floor(width*scale);
        int newHeight = floor(height*scale);
        cout << newWidth << "\t" << newHeight << endl;
        
        stringstream ss;
        ss << "PHOTO_" << ofGetHours() << "_" << ofGetMinutes() << "_" << ofGetSeconds();
        cout << ss.str() << endl;
        
        image.crop(0.5*(image.getWidth()-newWidth), 0.5*(image.getHeight()-newHeight), newWidth, newHeight);
        image.saveImage("photos/"+ss.str()+"."+EXTENSION);
        image.resize(120, 90);
        image.saveImage("thumbs/"+ss.str()+"_THUMB."+EXTENSION);
        image.update();
        
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
		if ( m.getAddress() == "/delete" ) {
            string name = m.getArgAsString(0);
			ofFile file(ofToDataPath("photos/"+name+"."+EXTENSION));
            if (file.exists()) {
                file.remove();
            }
            
            file = ofFile(ofToDataPath("thumbs/"+name+"_THUMB."+EXTENSION));
            if (file.exists()) {
                file.remove();
            }
            
            m.clear();
            m.setAddress("/remove");
            m.addStringArg(name);
            for (map<string,ofxOscSender*>::iterator iter=senders.begin(); iter!=senders.end(); iter++) {
                iter->second->sendMessage(m);
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
                m.addStringArg(ofSplitString(dir.getName(i), ".").front());
                sender->sendMessage(m);
            }
            
		}
			
    }
}
        
    

//--------------------------------------------------------------
void testApp::draw(){
	ofSetHexColor(0xffffff);
	vidGrabber.draw(0,0);
    if (image.getTextureReference().bAllocated()) {
        image.draw(40,40);
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

