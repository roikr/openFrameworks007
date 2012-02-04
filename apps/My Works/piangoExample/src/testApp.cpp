#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofBackground(34, 34, 34);

	// 2 output channels,
	// 0 input channels
	// 22050 samples per second
	// 512 samples per buffer
	// 4 num buffers (latency)
	
	int bufferSize		= 512;
	sampleRate 			= 44100;
	volume				= 1.0f;
	bPlay 				= false;

	lAudio.assign(bufferSize, 0.0);
	rAudio.assign(bufferSize, 0.0);
	
	//soundStream.listDevices();
	
	//if you want to set the device id to be different than the default
	//soundStream.setDeviceID(1); 	//note some devices are input only and some are output only 

	soundStream.setup(this, 2, 0, sampleRate, bufferSize, 4);

	ofSetFrameRate(60);
    
    audio.load(ofToDataPath("Piango.caf"), bufferSize);
    
    audio.play();
}


//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

	ofSetColor(225);
	ofDrawBitmapString("AUDIO OUTPUT EXAMPLE", 32, 32);
	ofDrawBitmapString("press 's' to unpause the audio\npress 'e' to pause the audio", 31, 92);
	
	ofNoFill();
	
	// draw the left channel:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 150, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("Left Channel", 4, 18);
		
		ofSetLineWidth(1);	
		ofRect(0, 0, 900, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
					
			ofBeginShape();
			for (int i = 0; i < lAudio.size(); i++){
				float x =  ofMap(i, 0, lAudio.size(), 0, 900, true);
				ofVertex(x, 100 -lAudio[i]*180.0f);
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();

	// draw the right channel:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 350, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("Right Channel", 4, 18);
		
		ofSetLineWidth(1);	
		ofRect(0, 0, 900, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
					
			ofBeginShape();
			for (int i = 0; i < rAudio.size(); i++){
				float x =  ofMap(i, 0, rAudio.size(), 0, 900, true);
				ofVertex(x, 100 -rAudio[i]*180.0f);
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();
	
		
	ofSetColor(225);
	string reportString = "volume: ("+ofToString(volume, 2)+") modify with -/+ keys\npan: ("+ofToString(pan, 2)+") modify with mouse x";
	
	ofDrawBitmapString(reportString, 32, 579);

}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	if (key == '-' || key == '_' ){
		volume -= 0.05;
		volume = MAX(volume, 0);
	} else if (key == '+' || key == '=' ){
		volume += 0.05;
		volume = MIN(volume, 1);
	}
	
	if( key == 's' ){
		soundStream.start();
	}
	
	if( key == 'e' ){
		soundStream.stop();
	}
	
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	int width = ofGetWidth();
	pan = (float)x / (float)width;
	float height = (float)ofGetHeight();
	float heightPct = ((height-y) / height);
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	int width = ofGetWidth();
	pan = (float)x / (float)width;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	bPlay = true;
}


//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	bPlay = false;
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::audioOut(float * output, int bufferSize, int nChannels){
	//pan = 0.5f;
	
    if (bPlay) {
        audio.mixChannel(output, 0, nChannels);
        audio.mixChannel(output, 1, nChannels);
        audio.postProcess();
    } else {
        for (int i = 0; i < bufferSize; i++){
            output[i*nChannels    ] =  0;
            output[i*nChannels + 1]=  0;
        }
    }
       
    float leftScale = 1 - pan;
	float rightScale = pan;
    
    
	for (int i = 0; i < bufferSize; i++){
        output[i*nChannels    ] *=  volume * leftScale;
        output[i*nChannels + 1] *=  volume * rightScale;
        lAudio[i] = output[i*nChannels    ];
        rAudio[i] = output[i*nChannels + 1];
    }

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
