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
	
	for (int i=0; i<xml.getNumTags("video"); i++) {
		videoPlayer *video = new videoPlayer;
		
		string filename = xml.getAttribute("video", "filename", "", i);
		video->pos = ofPoint(xml.getAttribute("video", "x", 0, i),xml.getAttribute("video", "y", 0, i));		
		video->player.loadMovie(xml.getAttribute("video", "filename", ""));
		video->player.play();
		video->player.setPaused(true);
		video->player.setLoopState(OF_LOOP_NONE);
		
		videos.push_back(video);
	}
	
	
	
	xml.popTag();
		
	
	
		
	ofSetFrameRate(60);
		
	ofBackground(255,255,255);
	
	bPlaying = false;

	ofSoundStreamListDevices(); 
	
		
	ofSoundStreamSetup(sounds.size(),0,this,sampleRate,256, 4);
}

void testApp::play() {
	
	for (vector<videoPlayer*>::iterator viter = videos.begin(); viter!=videos.end(); viter++) {
		(*viter)->player.setPaused(true);
		(*viter)->player.firstFrame();
		(*viter)->player.setPaused(false);
	}
	for (vector<ofxSndFile*>::iterator siter = sounds.begin(); siter!=sounds.end(); siter++) {
		(*siter)->play();
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
		
	} else if (!bPlaying) {
		play();
		
		ofxOscMessage m;
		m.setAddress( "/play" );
		for (vector<ofxOscSender*>::iterator iter=senders.begin(); iter!=senders.end(); iter++) {
			(*iter)->sendMessage( m );
		}
		
	} 
				
	
	if (bPlaying) {
		bool bDone = true;
		for (vector<videoPlayer*>::iterator viter = videos.begin(); viter!=videos.end(); viter++) {
			bDone = bDone && (*viter)->player.getIsMovieDone();
			
			(*viter)->player.update();
		}
		
		if (bDone ) {  
			cout << "movie is done" << endl;
			bPlaying = false;
		}
	}
	
	
	
	
	
	

	
	

	
	
	

	
}

//--------------------------------------------------------------
void testApp::draw(){
	for (vector<videoPlayer*>::iterator viter = videos.begin(); viter!=videos.end(); viter++) {
		(*viter)->player.draw((*viter)->pos.x, (*viter)->pos.y);
	}
}

void testApp::exit() {
	for (vector<videoPlayer*>::iterator viter = videos.begin(); viter!=videos.end(); viter++) {
		(*viter)->player.stop();
		delete (*viter);
		*viter = 0;
	}
	
	for (vector<ofxSndFile*>::iterator siter = sounds.begin(); siter!=sounds.end(); siter++) {
		(*siter)->exit();
		delete (*siter);
		*siter = 0;
	}
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	if	(key='t') {
		ofToggleFullscreen();	
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){ 
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	for (vector<videoPlayer*>::iterator viter = videos.begin(); viter!=videos.end(); viter++) {
		(*viter)->player.setPaused(true);
		(*viter)->player.setPosition(0.95);
		(*viter)->player.setPaused(false);
	}
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

