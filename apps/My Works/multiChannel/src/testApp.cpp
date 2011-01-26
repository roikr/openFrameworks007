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
	
	font.loadFont("Sudbury_Basin_3D.ttf", 32);
	
	ofxSoundStreamListDevices(); 
	
	//ofxSoundStreamSetup(8,0,this, sampleRate,256, 4);
	ofxSoundStreamSetup(2,sounds.size(),0,this);
	ofSetFrameRate(60);
		
}

//--------------------------------------------------------------
void testApp::update(){	
	
	ofBackground(255,255,255);
	
	// update the sound playing system:
	ofSoundUpdate();
	
}

//--------------------------------------------------------------
void testApp::draw(){
	
	char tempStr[255];
	
	// draw the background colors:
	float widthDiv = ofGetWidth() / (float)sounds.size();
	ofSetColor(0xeeeeee);
	ofRect(0,0,widthDiv,ofGetHeight()); 
	ofSetColor(0xffffff);
	ofRect(widthDiv,0,widthDiv,ofGetHeight()); 
	ofSetColor(0xdddddd);
	ofRect(widthDiv*2,0,widthDiv,ofGetHeight()); 
	
	
	int i=0;

	for (vector<ofxSndFile*>::iterator iter = sounds.begin(); iter!=sounds.end(); iter++) {
		if ((*iter)->getIsPlaying()) ofSetColor(0xFF0000);
		else ofSetColor(0x000000);	
		stringstream ss;
		ss << i;
		font.drawString(ss.str(),widthDiv*i+50,50);
		
		ofSetColor(0x000000);
		//sprintf(tempStr, "click to play\npct done: %f\nspeed: %f\npan: %f", (*iter)->getPosition(),  (*iter)->getSpeed(), (*iter)->getPan());
		//ofDrawBitmapString(tempStr, widthDiv*i+50,ofGetHeight()-50);
		i++;
	}
	

		
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
	float widthStep = ofGetWidth() / (float)sounds.size();
	int i=0;
	for (vector<ofxSndFile*>::iterator iter = sounds.begin(); iter!=sounds.end(); iter++) {
		if (x>=i*widthStep && x <(i+1)*widthStep) {
			float pct = (x-(i*widthStep)) / widthStep;
			(*iter)->play();
			//(*iter)->setSpeed( 0.1f + ((float)(ofGetHeight() - y) / (float)ofGetHeight())*10);
			//(*iter)->setPan(pct);
			
		}
		i++;
	
	}
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	// continuously control the speed of the beat sample via drag, 
	// when in the "beat" region:
	//float widthStep = ofGetWidth() / 3.0f;
//	if (x >= widthStep && x < widthStep*2){
//		beats.setSpeed( 0.5f + ((float)(ofGetHeight() - y) / (float)ofGetHeight())*1.0f);
//	} 

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

