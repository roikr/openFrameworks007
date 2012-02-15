#include "testApp.h"

#define FFT_SIZE 1024


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
	bPlay 				= false;

	input.assign(bufferSize, 0.0);
	
	
	//soundStream.listDevices();
	
	//if you want to set the device id to be different than the default
	//soundStream.setDeviceID(1); 	//note some devices are input only and some are output only 

	soundStream.setup(this, 2, 1, sampleRate, bufferSize, 4); // stereo output one channel input
    
    fft.setup(FFT_SIZE);
    smoothed.assign(fft.mFFTLength, 0.0);
    power.assign(fft.mFFTLength, 0.0);
    spectrum.assign(fft.mFFTLength, 0.0);
    spectralFlux.assign(bufferSize, 0.0);
    sfiter=spectralFlux.begin();
    
    
    beatDecay = 0.8f;
    
    counter = 0;

	ofSetFrameRate(60);
    
    audio.load(ofToDataPath("tropit.caf"), bufferSize);
    
    
    bInput = true;
}


//--------------------------------------------------------------
void testApp::update(){

}

#define GAIN 100.0f

//--------------------------------------------------------------
void testApp::draw(){

	ofSetColor(225);
	ofDrawBitmapString("AUDIO OUTPUT EXAMPLE", 32, 32);
//	ofDrawBitmapString("press 's' to unpause the audio\npress 'e' to pause the audio", 31, 92);
	
	ofNoFill();
	
	// draw the left channel + power:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 150, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("Left Channel + Power", 4, 18);
		
		ofSetLineWidth(1);	
		ofRect(0, 0, 900, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
					
			ofBeginShape();
			for (int i = 0; i < input.size(); i++){
				float x =  ofMap(i, 0, input.size(), 0, 900, true);
				ofVertex(x, 100 -input[i]*180.0f);
			}
			ofEndShape(false);
    
        
        ofSetLineWidth(1);
            ofBeginShape();
            for (int i = 0; i < power.size(); i++){
                float x =  ofMap(i, 0, power.size(), 0, 900, true);
                ofVertex(x, 200 -power[i]-128);
            }
            ofEndShape(false);
    
			
		ofPopMatrix();
	ofPopStyle();

    
    // draw the left channel + power:
	ofPushStyle();
        ofPushMatrix();
        ofTranslate(32, 350, 0);
        
        ofSetColor(225);
        ofDrawBitmapString("spectral flux", 4, 18);
        
        ofSetLineWidth(1);	
        ofRect(0, 0, 900, 200);
        
        ofSetColor(245, 58, 135);
    

        ofSetLineWidth(1);
    
            ofBeginShape();
    
            
            vector<float>::iterator iter = spectralFlux.begin() + (distance(spectralFlux.begin(), sfiter) + spectralFlux.size()) % spectralFlux.size();
    
            for (int i = 0; i < spectralFlux.size(); i++){
                float x =  ofMap(i, 0, spectralFlux.size(), 0, 900, true);
                ofVertex(x, 200 -(*iter) );
                
                iter++;
                if (iter == spectralFlux.end()) {
                    iter = spectralFlux.begin();
                }
            }
    
                        
    
            ofEndShape(false);
        
        
        ofPopMatrix();
	ofPopStyle();
	
	
    
	

}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	
	
	if( key == 't' ){
		if (bInput) {
            audio.play();
        } else {
            audio.stop();
        }
        
        bInput=!bInput;
	}
	
	
	
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	bPlay =! bPlay;
}


//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

void testApp::processAudio(float * input, int bufferSize) {
    fft.process(input,bufferSize);
    
    if (fft.bReady) {
        *sfiter = 0;
        for (int i = 0; i < fft.mFFTLength; i++){
            float x = fft.amplitude[i];
            (*sfiter)+=  x- spectrum[i];
            spectrum[i] = x;
        }
        
        sfiter++;
        if (sfiter==spectralFlux.end()) {
            sfiter=spectralFlux.begin();
        }
        
        for (int i = 0; i < fft.mFFTLength; i++){
            power[i]		= fft.power[i];
            
            // let the smoothed calue sink to zero:
            smoothed[i] *= 0.96f;
            
            // take the max, either the smoothed or the incoming:
            if (smoothed[i] <  fft.amplitude[i]) 
                smoothed[i] =  fft.amplitude[i];
            
        }
        
        
        
        
    }
}


void testApp::audioIn(float * input, int bufferSize, int nChannels) {
    if (bInput) {
        memcpy(this->input.data(), input, sizeof(float)*bufferSize);
        processAudio(input, bufferSize);
    }
}

//--------------------------------------------------------------
void testApp::audioOut(float * output, int bufferSize, int nChannels){
	//pan = 0.5f;
    if  (!bInput) {
        audio.mixChannel(output, 0, nChannels);
        audio.mixChannel(output, 1, nChannels);
        audio.postProcess();
        
        for (int i = 0; i < bufferSize; i++){
            input[i] = output[i*nChannels    ]; // copy just left channel
            
        }
        
        processAudio(input.data(), bufferSize);
	}
    
    
    
//    for (int i = 0; i < bufferSize; i++){
//        output[i*nChannels    ] =  0;
//        output[i*nChannels + 1]=  0;
//    }
        
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
