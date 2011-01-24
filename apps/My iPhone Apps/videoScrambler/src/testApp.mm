#include "testApp.h"
#include "ofMainExt.h"

#include "ofxXmlSettings.h"


//--------------------------------------------------------------
void testApp::setup(){	
	ofDisableDataPath();
	// register touch events
	ofxRegisterMultitouch(this);
		
	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);

	
	ofBackground(0,0,0);
		
	
	ofSetFrameRate(30);
	
	int bufferSize = 256;
	int sampleRate = 44100;
	
	ofSoundStreamSetup(2, 0, this, sampleRate, bufferSize, 2);	

	streamer.setup(ofToResourcesPath("IMG_0121.MOV"));
	
		
	
	pos = ofPoint(0,0);
	
}

//--------------------------------------------------------------
void testApp::update(){
	//image.update();
	streamer.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	ofPushMatrix();
	ofTranslate(pos.x, pos.y, 0);
	ofScale(0.5, 0.5, 1);
	
	streamer.draw();
	ofPopMatrix();
	
	
	
	
	ofSetColor(255, 255, 255);
	
	float sfps = streamer.getFrameRate();
	std::ostringstream ss;
	
	ss << sfps << " " << ofGetFrameRate() ;
	
	
	ofDrawBitmapString(ss.str(), 20, 20);
	

	
	//ofSetFrameRate(sfps);
	
		
	
	//ofDisableAlphaBlending();
	
	//--------------------------- rectangles
//	ofFill();
//	for (int i = 0; i < 200; i++){
//		ofSetColor((int)ofRandom(0,255),(int)ofRandom(0,255),(int)ofRandom(0,255));
//		ofRect(ofRandom(250,350),ofRandom(250,350),ofRandom(10,20),ofRandom(10,20));
//	}
//	ofSetColor(0x000000);
//	ofDrawBitmapString("rectangles", 275,500);
//
//
//	pincher.draw();

}



void testApp::touchDown(int x, int y, int id) {
	down = ofPoint(x,y);
	
}



void testApp::touchMoved(int x, int y, int id) {
	pos+=ofPoint(x,y)-down;
	down = ofPoint(x,y);
		
		
}

void testApp::touchUp(int x, int y, int id) {
	
	
		
	}

void testApp::touchDoubleTap(int x, int y, int id) {

}


void testApp::audioRequested( float * output, int bufferSize, int nChannels ) {
	
	streamer.audioRequested(output, bufferSize, nChannels);
	//printf("bufferSize: %i\n",bufferSize);
//	
//	memset(lAudio, 0, bufferSize*sizeof(float));
//	memset(rAudio, 0, bufferSize*sizeof(float));
//	
//	
//	for (int i = 0; i < bufferSize; i++){
//		output[i*nChannels] = lAudio[i];// * gain;
//		output[i*nChannels + 1] = rAudio[i];// * gain;
//	}
	
}



