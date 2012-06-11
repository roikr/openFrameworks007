#include "testApp.h"

#define VECTOR_SIZE 200




//--------------------------------------------------------------
void testApp::setup(){
    ofBackground(0);
    ofSetVerticalSync(true);
       
    // 0 output channels, 
	// 2 input channels
	// 44100 samples per second
	// 256 samples per buffer
	// 4 num buffers (latency)
	
	soundStream.listDevices();
	
	//if you want to set a different device id 
	soundStream.setDeviceID(3); //bear in mind the device id corresponds to all audio devices, including  input-only and output-only devices.
	
    
	int bufferSize = 256;
    
    for (int i=0;i<NUM_CHANNELS;i++) {
        channel c;
        channels.push_back(c);
        vector<channel>::iterator iter=channels.begin()+i;
        iter->input.assign(bufferSize,0.0);
        iter->peakVec.assign(VECTOR_SIZE,0.0);
        iter->limiterVec.assign(VECTOR_SIZE,0.0);
        iter->envelopeVec.assign(VECTOR_SIZE,0.0);
        iter->peak = 0.0;
        iter->envelope = 0.0;
        
        iter->thresholds.assign(2, 0.0);
        iter->thresholds[THRESHOLD_MIN] = 0.01;
        iter->thresholds[THRESHOLD_MAX] = 0.1;
    }
	
	
    
    bufferCounter	= 0;
    drawCounter		= 0;
    float sampleRate = 44100;
    
    float attackMs= 10;
    float releaseMs = 500;
    ga = pow( 0.01, 1.0 / ( attackMs * sampleRate * 0.001 ) ); 
    gr = pow( 0.01, 1.0 / ( releaseMs * sampleRate * 0.001 ) ); 
    
	soundStream.setup(this, 0, NUM_CHANNELS, sampleRate, bufferSize, 4);
    
    bDebug = true;
    //ofToggleFullscreen();
    ofSetFrameRate(30);
    ofEnableAlphaBlending();
    
    ofNoFill();
    
    curThresh = THRESHOLD_MIN;
    
}


//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){
    
    
    if (bDebug) {
        
        for (int i=0;i<NUM_CHANNELS;i++) {
            
            ofPushMatrix();
            ofTranslate(10+(VECTOR_SIZE+10)*i, 0,0);
        
        channel &ch = channels[i];
        /*
        // draw the left channel:
        ofPushStyle();
            ofPushMatrix();
            ofTranslate(32, 170, 0);
            
            ofSetColor(245, 58, 135);
            ofRect(0, 0, 256*2, 200);
                    
            ofBeginShape();
            for (int i = 0; i < channels.front().input.size(); i++){
                ofVertex(i*2, 100 -channels.front().input[i]*180.0f);
            }
            ofEndShape(false);
            
            ofPopMatrix();
        ofPopStyle();
        */
        
        ofPushStyle();
            ofPushMatrix();
            ofTranslate(0, 10, 0);
            
            ofSetColor(150);
            float y = 20*log(ch.thresholds[THRESHOLD_MIN])/log(10);
            ofLine(0, -2*y, ch.peakVec.size(), -2*y);
            ofSetColor(250);
            y = 20*log(ch.thresholds[THRESHOLD_MAX])/log(10);
            ofLine(0, -2*y, ch.peakVec.size(), -2*y);
            
            
            ofSetColor(245, 58, 135);
            
            ofRect(0, 0, ch.peakVec.size(), 200);
            
            ofBeginShape();
            for (int i = 0; i < ch.peakVec.size(); i++){
                float x = 20*log(ch.peakVec[(bufferCounter+i)%ch.peakVec.size()])/log(10);
                
                ofVertex(i, -2*x);
            }
            ofEndShape(false);
        
            
            
        
            
            ofPopMatrix();
        ofPopStyle();
        
        ofPushStyle();
            ofPushMatrix();
            ofTranslate(0, 220, 0);
            
            ofSetColor(245, 58, 135);
            
            ofRect(0, 0, ch.peakVec.size(), 200);
            
            ofBeginShape();
            for (int i = 0; i < ch.peakVec.size(); i++){
                float x = ch.peakVec[(bufferCounter+i)%ch.peakVec.size()];
                float y =  ofMap(x, ch.thresholds[THRESHOLD_MIN], ch.thresholds[THRESHOLD_MAX], 0.0f, 1.0f,true);
                ofVertex(i, 200*(1-y));
            }
            ofEndShape(false);
            
            ofPopMatrix();
        ofPopStyle();
        
        ofPushStyle();
            ofPushMatrix();
            ofTranslate(0, 430, 0);
            
            ofSetColor(245, 58, 135);
           
            ofRect(0, 0, ch.envelopeVec.size(), 200);
            
            ofBeginShape();
            for (int i = 0; i < ch.envelopeVec.size(); i++){
                float x = ch.envelopeVec[(bufferCounter+i)%ch.envelopeVec.size()];
                float y = ofMap(x, ch.thresholds[THRESHOLD_MIN], ch.thresholds[THRESHOLD_MAX], 0.0f, 1.0f,true);
                ofVertex(i,200*(1-y) );
            }
            ofEndShape(false);
            
            ofPopMatrix();
        ofPopStyle();
        
        ofPushStyle();
            ofPushMatrix();
            ofTranslate(0, 640, 0);
            
            ofSetColor(245, 58, 135);
            
            ofRect(0, 0, ch.limiterVec.size(), 200);
            
            ofBeginShape();
            for (int i = 0; i < ch.limiterVec.size(); i++){
                
                float x = ch.limiterVec[(bufferCounter+i)%ch.limiterVec.size()];
                float y = ofMap(x, ch.thresholds[THRESHOLD_MIN], ch.thresholds[THRESHOLD_MAX], 0.0f, 1.0f,true);
                ofVertex(i,200*(1-y) );
                
            }
            ofEndShape(false);
            
            ofPopMatrix();
        ofPopStyle();
        
            ofPopMatrix();
        
//        ofPushStyle();
//        ofPushMatrix();
//        ofTranslate(10, 420, 0);
//        
//        ofSetColor(245, 58, 135);
//        
//        ofRect(0, 0, ch.power.size()*2, 200);
//        
//        ofBeginShape();
//        for (int i = 0; i < ch.power.size(); i++){
//            ofVertex(i*2, 100-ch.power[(bufferCounter+i)%ch.power.size()]);
//        }
//        ofEndShape(false);
//        
//        ofPopMatrix();
//        ofPopStyle();
        
        
        for (vector<float>::iterator iter=ch.thresholds.begin(); iter!=ch.thresholds.end(); iter++) {
            int d=distance(ch.thresholds.begin(), iter);
            ofSetColor(255*((d+1) & 0x4), 255*((d+1) & 0x2), 255*((d+1) & 0x1));
            
            switch (d) {
                case THRESHOLD_MIN:
                    
                    break;
                    
                default:
                    break;
            }
            
        }

        }
            
        
        drawCounter++;
        
        ofSetColor(225);
        string reportString = "buffers received: "+ofToString(bufferCounter)+"\ndraw routines called: "+ofToString(drawCounter)+"\nticks: " + ofToString(soundStream.getTickCount());
        ofDrawBitmapString(reportString, 32, 589);
    }
    
}


//--------------------------------------------------------------
void testApp::audioIn(float * input, int bufferSize, int nChannels){	
	
	

   
    for (int j=0; j<nChannels; j++) {
        channel &c = channels[j];
        c.peak = 0;
        float limiter = 0;
        
        for(int i=0;i<bufferSize;i++)
        {
            
                
            c.input[i] = input[nChannels*i+j];
            
            float EnvIn = abs(c.input[i]);
            
            if(c.envelope < EnvIn)
            {
                c.envelope =ga *(c.envelope - EnvIn) + EnvIn;
                
            }
            else
            {
                c.envelope =gr *(c.envelope - EnvIn) + EnvIn;
            }
            
            c.peak = max(c.peak,abs(input[nChannels*i+j]));
            
            if (c.envelope>c.thresholds[THRESHOLD_MAX]) {
                limiter = max(limiter,c.thresholds[THRESHOLD_MAX]*abs(input[nChannels*i+j])/c.envelope);
            } else {
                limiter = max(limiter,abs(input[nChannels*i+j]));
            }
        }
        
        c.peakVec[bufferCounter % c.peakVec.size()] = c.peak;
        c.limiterVec[bufferCounter % c.limiterVec.size()] = limiter;
        c.envelopeVec[bufferCounter % c.envelopeVec.size()] = c.envelope;
       

    
    }
        
        
        
	bufferCounter++;
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key) {
        case 'f':
            ofToggleFullscreen();
            break;
        case 'd':
            bDebug = !bDebug;
            break;
        case '1':
        case '2':
            curThresh = (key-'1') % THRESHOLD_NUM;
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    switch (curThresh) {
        case THRESHOLD_MIN:
        case THRESHOLD_MAX: {
            channel &c=channels[(x / (VECTOR_SIZE+10)) % NUM_CHANNELS];
            c.thresholds[curThresh] = exp((10-y)*log(10)/20/2);
        } break;
            
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

