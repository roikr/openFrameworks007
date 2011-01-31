#include "testApp.h"
#include "ofxXmlSettings.h"
#include "ofxSndFile.h"
#include "ofxOsc.h"


//--------------------------------------------------------------
void testApp::setup(){	 
	
	bufferSize = 256;
	sampleRate = 44100;
	
	ofxXmlSettings xml;
	bool bLoaded = xml.loadFile("channels.xml");
	assert(bLoaded);
	
	receiver = NULL;
	bool bSender = xml.getAttribute("channels", "sender", 0);
	port = xml.getAttribute("channels", "port", 0);
	
		
	xml.pushTag("channels");
	
	if (bSender) {
		for (int i=0; i<xml.getNumTags("client"); i++) {
			ofxOscSender *sender = new ofxOscSender;
			sender->setup(xml.getAttribute("client","address","",i), port);
			senders.push_back(sender);
		}
	} else {
		receiver = new ofxOscReceiver;
		receiver->setup(port);
	}

	
	
	for (int i=0; i<xml.getNumTags("audio"); i++) {
		ofxSndFile *sound = new ofxSndFile;

		string filename = xml.getAttribute("audio", "filename", "", i);
		sound->load(ofToDataPath(filename), bufferSize);
		
		sounds.push_back(sound);
	}
	
	video.loadMovie(xml.getAttribute("video", "filename", ""));
	video.play();
	video.setPaused(true);
	video.setLoopState(OF_LOOP_NONE);
	
	xml.popTag();
		
	ofSoundStreamListDevices(); 
	
		
	ofSoundStreamSetup(sounds.size(),0,this);
	
		
	ofSetFrameRate(60);
		
	ofBackground(255,255,255);
	
	bPlaying = false;
}

void testApp::play() {
	video.setPaused(true);
	video.firstFrame();
	video.setPaused(false);
	for (vector<ofxSndFile*>::iterator iter = sounds.begin(); iter!=sounds.end(); iter++) {
		(*iter)->play();
	}
	
	bPlaying = true;
	
}

//--------------------------------------------------------------
void testApp::update(){	
	
	if (receiver) {
		while( receiver->hasWaitingMessages() )
		{
			// get the next message
			ofxOscMessage m;
			receiver->getNextMessage( &m );
			
			// check for mouse moved message
			if ( m.getAddress() == "/play" )
			{
				
				cout << "play" << endl;
				play();
				
			}
		}
		
	} else {
		if (!bPlaying) {
			play();
			ofxOscMessage m;
			m.setAddress( "/play" );
			for (vector<ofxOscSender*>::iterator iter=senders.begin(); iter!=senders.end(); iter++) {
				(*iter)->sendMessage( m );
			}
		} 
				
	}
	
	if (bPlaying && video.getIsMovieDone()) {
		cout << "movie is done" << endl;
		bPlaying = false;
	}

	
	

	
	
	
	video.update();
	
}

//--------------------------------------------------------------
void testApp::draw(){
	
	video.draw(0, 0);
		
}

void testApp::exit() {
	
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
void testApp::mousePressed(int x, int y, int button){
	video.setPaused(true);
	video.setPosition(0.95);
	video.setPaused(false);
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}



//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::audioRequested 	(float * output, int bufferSize, int nChannels){
		
	int i=0;
	for (vector<ofxSndFile*>::iterator iter = sounds.begin(); iter!=sounds.end(); iter++,i++) {
		(*iter)->audioRequested(output,i,bufferSize,nChannels);
		
	
	} 
	
}

