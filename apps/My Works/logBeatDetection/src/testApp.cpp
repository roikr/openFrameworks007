#include "testApp.h"

#define FFT_SIZE 512
#define SUBBANDS_NUMBER 32
#define SUBBANDS_FIRST 1
#define MEAN_ENERGY_DELAY 1.0f
#define MEAN_FACTOR 1000


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

	lAudio.assign(bufferSize, 0.0);
	
	
	//soundStream.listDevices();
	
	//if you want to set the device id to be different than the default
	//soundStream.setDeviceID(1); 	//note some devices are input only and some are output only 

	soundStream.setup(this, 2, 0, sampleRate, bufferSize, 4);
    
    fft.setup(FFT_SIZE);
    smoothed.assign(fft.mFFTLength, 0.0);
    power.assign(fft.mFFTLength, 0.0);
    
    beatDecay = 0.8f;
    
    float N = fft.mFFTLength;
    float n = SUBBANDS_NUMBER;
    float w1 = SUBBANDS_FIRST;
    float a = 2*(N-n*w1)/n/(n-1);
    float b = (n*(n+1)*w1-2*N)/n/(n-1);
    
       
    for (float i=0;i<n;i++) { // we keep subband first bean
        //int w = floor(SUBBANDS_A * (i+1) + SUBBANDS_B);
        int first = floor(i*b + a * i*(i+1)/2.0);
        int next = floor((i+1)*b + a * (i+1)*(i+2)/2.0);
        
        
        band s;
        s.first = first;
        s.width = next-first;
        s.energy = 0;
        s.mean = 0;
        
        s.history.assign(MEAN_ENERGY_DELAY* sampleRate / N, 0.0);
        s.beat = 0;
        s.counter = 0;
        
        cout << i << " " << s.first << " " << s.width << endl;
        
        bands.push_back(s);
    }
    
    cout << "energy beans: " << MEAN_ENERGY_DELAY* sampleRate / fft.numFrames << endl;
   
    counter = 0;

	ofSetFrameRate(60);
    
    audio.load(ofToDataPath("tropit.caf"), bufferSize);
    
    audio.play();
}


//--------------------------------------------------------------
void testApp::update(){

}

#define GAIN 100.0f

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
		ofDrawBitmapString("FFT", 4, 18);
		
		ofSetLineWidth(1);	
		ofRect(0, 0, 900, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
					
			ofBeginShape();
			for (int i = 0; i < power.size(); i++){
				float x =  ofMap(i, 0, power.size(), 0, 900, true);
				ofVertex(x, 200 -power[i]-128);
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();
	
    ofPushStyle();
        ofPushMatrix();
            ofTranslate(32, 600,0);
            
            // draw the fft resutls:
            ofFill();
            ofEnableAlphaBlending();
            ofSetColor(255,255,255,100);
            ofRect(0,0,900,100);
            
            
            
            
            float width = (float)(900) / smoothed.size();
    
            ofSetColor(0,255,0,150);
            
            
            for (vector<band>::iterator iter=bands.begin(); iter!=bands.end();iter++) {
                ofRect(width*iter->first,100,width*(iter->width),-(iter->beat  * 100)); 
            }
    
            ofSetColor(255,255,255,50);
    
            for (vector<band>::iterator iter=bands.begin(); iter!=bands.end();iter++) {
                ofRect(width*iter->first,100,width*(iter->width),-(iter->mean * 200 * MEAN_FACTOR * 50)); 
            }
    
            ofSetColor(255,0,0,150);
            
            
            for (vector<band>::iterator iter=bands.begin(); iter!=bands.end();iter++) {
               ofRect(width*iter->first,100,width*(iter->width),-(iter->energy * 200 * 200)); 
            }
            
            
            ofSetColor(0,0,255,50);
            
            for (int i = 0; i < smoothed.size()/4; i++){
                // (we use negative height here, because we want to flip them
                // because the top corner is 0,0)
                ofRect(i*width,100,width,-(smoothed[i] * 200 * 50));
            }
    
            ofDisableAlphaBlending();
        ofPopMatrix();
	ofPopStyle();
	

}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	
	
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
       
    
    
    
	for (int i = 0; i < bufferSize; i++){
        lAudio[i] = output[i*nChannels    ];
        
    }

    
    fft.process(lAudio.data(),bufferSize);
    
    if (fft.bReady) {
        
        for (int i = 0; i < fft.mFFTLength; i++){
            power[i]		= fft.power[i];
            
            // let the smoothed calue sink to zero:
            smoothed[i] *= 0.96f;
            
            // take the max, either the smoothed or the incoming:
            if (smoothed[i] <  fft.amplitude[i]) 
                smoothed[i] =  fft.amplitude[i];

        }
        
        /*
        for (int j = 0; j < 16; j++){
            bands[j] = 0;
            for (int i = 0; i < 32; i++){
                bands[j] += fft.amplitude[32*j+i];
            }
        }
        */
        
        
        for (vector<band>::iterator iter=bands.begin(); iter!=bands.end();iter++) {
            iter->energy = 0;
            for (int i = 0; i < iter->width; i++){
                iter->energy +=  fft.amplitude[iter->first+i];
            }
            iter->energy/=iter->width;
            iter->history[counter % iter->history.size()] = iter->energy/iter->history.size();
            
            iter->mean = 0;
            for (vector<float>::iterator eiter=iter->history.begin(); eiter!=iter->history.end(); eiter++) {
                iter->mean+=(*eiter);
            }
            iter->mean/=iter->history.size();
            
            if (counter>=iter->history.size() &&  iter->energy >= MEAN_FACTOR * iter->mean) {
                iter->counter++;
                iter->beat = 1.0;
            } else {
                iter->beat*=beatDecay;
            }
        }
        
        counter++;
        
        
    }
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
