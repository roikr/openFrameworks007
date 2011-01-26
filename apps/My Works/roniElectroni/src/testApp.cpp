#include "testApp.h"
#include "ofxXmlSettings.h"
#include "ofxSndFile.h"
#include "ofxSoundStream.h"

//--------------------------------------------------------------
void testApp::setup(){	 
	
	bufferSize = 256;
	sampleRate = 44100;
	
	ofxXmlSettings xml;
	bool bLoaded = xml.loadFile("channels.xml");
	assert(bLoaded);
	xml.pushTag("channels");
	for (int i=0; i<xml.getNumTags("audio"); i++) {
		ofxSndFile *sound = new ofxSndFile;

		string filename = xml.getAttribute("audio", "filename", "", i);
		sound->load(ofToDataPath(filename), bufferSize);
		
		sounds.push_back(sound);
	}
	
	video.loadMovie(xml.getAttribute("video", "filename", ""));
	
	xml.popTag();
	
	
	for (vector<ofxSndFile*>::iterator iter = sounds.begin(); iter!=sounds.end(); iter++) {
		
		(*iter)->play();
	}
	
	video.play();
	
	//ofxSoundStreamListDevices(); 
	
	//ofxSoundStreamSetup(8,0,this, sampleRate,256, 4);
	
	ofxSoundStreamSetup(2,sounds.size(),0,this);
	
	
	
	
	ofSetFrameRate(60);
		
	ofBackground(255,255,255);
}

//--------------------------------------------------------------
void testApp::update(){	
	
	
	
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

