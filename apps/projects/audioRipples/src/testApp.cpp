#include "testApp.h"

#include "ofxXmlSettings.h"

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
    
    ofxXmlSettings xml;
    xml.loadFile("settings.xml");
    xml.pushTag("settings");
    
    
    for (int i=0;i<xml.getNumTags("channel");i++) {
        xml.pushTag("channel",i);
        
        channel c;
        channels.push_back(c);
        vector<channel>::iterator iter=channels.begin()+i;
        iter->input.assign(bufferSize,0.0);
        iter->peakVec.assign(VECTOR_SIZE,0.0);
        iter->limiterVec.assign(VECTOR_SIZE,0.0);
        iter->envelopeVec.assign(VECTOR_SIZE,0.0);
        iter->peak = 0.0;
        iter->envelope = 0.0;
        
        
        
        iter->thresholds.assign(THRESHOLD_NUM, 0.01);
        for (int j=0;j<xml.getNumTags("threshold"); j++) {
            iter->thresholds[j] = xml.getValue("threshold", 0.01,j);
        }
        
        
        iter->bTriggered = false;
        
        xml.popTag();
    }
	xml.popTag();
    
    
	cout << channels.size();
    
    bufferCounter	= 0;
    drawCounter		= 0;
    float sampleRate = 44100;
    
    float attackMs= 10;
    float releaseMs = 500;
    ga = pow( 0.01, 1.0 / ( attackMs * sampleRate * 0.001 ) ); 
    gr = pow( 0.01, 1.0 / ( releaseMs * sampleRate * 0.001 ) ); 
    
	soundStream.setup(this, 0, channels.size(), sampleRate, bufferSize, 4);
    
    bDebug = true;
    //ofToggleFullscreen();
    ofSetFrameRate(30);
    ofEnableAlphaBlending();
    
    ofNoFill();
    
    curThresh = THRESHOLD_MIN;
    
    int width =ofGetWidth()/2;
    int height = ofGetHeight()/2;
    
    image.allocate(width, height, OF_IMAGE_COLOR);
    
    buffers.push_back(new int[width*height]);
    buffers.push_back(new int[width*height]);
    memset(buffers.front(), 0, width*height);
    memset(buffers.back(), 0, width*height);
    
    counter = 0;
    
    bSetThresh = false;
}


//--------------------------------------------------------------
void testApp::update(){
    
    for (vector<channel>::iterator iter=channels.begin(); iter!=channels.end(); iter++) {
        if (iter->bTriggered) {
            int ix = iter->pos.x * image.getWidth()/ofGetWidth();
            int iy = iter->pos.y *image.getHeight()/ofGetHeight();
            
            float y = ofMap(iter->envelope, iter->thresholds[THRESHOLD_MIN], iter->thresholds[THRESHOLD_MAX], 0.0f, 1.0f,true);
            
            
            buffers[counter % buffers.size()][iy*(int)image.getWidth()+ix]= 2*255*y; // waves scale factor
        }
    }
    
    int *buffer0= buffers[counter % buffers.size()];
    int *buffer1 = buffers[(counter+1) % buffers.size()];
    
    int width = image.getWidth();
    int height = image.getHeight();
    
    
    for (int i=width;i<width*(height-1);i++) {
        int pixel = (buffer0[i-1] + buffer0[i+1] + buffer0[i+width] + buffer0[i-width])/2-buffer1[i];
        pixel -= pixel >> 20;
        buffer1[i]  = pixel;
    }
    
    ofColor color;
    ofColor bg;
    bg.set(100);
    
    for (int y=0;y<image.height; y++) {
        for (int x=0; x<image.width; x++) {
            color.setHsb(150, 100, ofClamp(buffer1[y*width+x], 0, 255));
            image.getPixelsRef().setColor(x, y, bg+color);
        }
    }
    
    
    
    image.update();
    
    counter++;
    
}

//--------------------------------------------------------------
void testApp::draw(){
    
    image.draw(0, 0, ofGetWidth(), ofGetHeight());
    
    
    if (bDebug) {
        
        for (vector<channel>::iterator iter=channels.begin();iter!=channels.end();iter++) {
            
            ofPushMatrix();
            ofTranslate(10+(VECTOR_SIZE+10)*distance(channels.begin(),iter), 0,0);
        
        
            ofPushStyle();
            ofPushMatrix();
            ofTranslate(0, 10, 0);
            
            ofSetColor(iter->bTriggered ? 255 : 50);
            ofRect(0, 0, iter->peakVec.size(), 200);
        
            ofSetColor(245, 58, 135);
            
            for (int i=0; i<THRESHOLD_NUM; i++) {
                float y = 20*log(iter->thresholds[i])/log(10);
                ofLine(0, -2*y, iter->peakVec.size(), -2*y);
                ofDrawBitmapString(ofToString(i+1), i*VECTOR_SIZE/THRESHOLD_NUM,-2*y-2);

            }
            
            ofBeginShape();
            for (int i = 0; i < iter->peakVec.size(); i++){
                float x = 20*log(iter->peakVec[(bufferCounter+i)%iter->peakVec.size()])/log(10);
                
                ofVertex(i, -2*x);
            }
            ofEndShape(false);
            
            ofPopMatrix();
            ofPopStyle();
            
            ofPushStyle();
            ofPushMatrix();
            ofTranslate(0, 220, 0);
        
            ofSetColor(iter->bTriggered ? 255 : 50);
            ofRect(0, 0, iter->peakVec.size(), 200);
        
            ofSetColor(245, 58, 135);
            
            
            for (int i=0; i<THRESHOLD_NUM; i++) {
                float y =  ofMap(iter->thresholds[i], iter->thresholds[THRESHOLD_MIN], iter->thresholds[THRESHOLD_MAX], 0.0f, 1.0f,true);
                ofLine(0, 200*(1-y), iter->peakVec.size(), 200*(1-y));
                ofDrawBitmapString(ofToString(i+1), 0,200*(1-y)-2);
            }
            
            
            ofBeginShape();
            for (int i = 0; i < iter->peakVec.size(); i++){
                float x = iter->peakVec[(bufferCounter+i)%iter->peakVec.size()];
                float y =  ofMap(x, iter->thresholds[THRESHOLD_MIN], iter->thresholds[THRESHOLD_MAX], 0.0f, 1.0f,true);
                ofVertex(i, 200*(1-y));
            }
            ofEndShape(false);
            
            ofPopMatrix();
            ofPopStyle();
            
            ofPushStyle();
            ofPushMatrix();
            ofTranslate(0, 430, 0);
        
            ofSetColor(iter->bTriggered ? 255 : 50);
            ofRect(0, 0, iter->peakVec.size(), 200);
            
            ofSetColor(245, 58, 135);
            
            
            for (int i=0; i<THRESHOLD_NUM; i++) {
                float y =  ofMap(iter->thresholds[i], iter->thresholds[THRESHOLD_MIN], iter->thresholds[THRESHOLD_MAX], 0.0f, 1.0f,true);
                ofLine(0, 200*(1-y), iter->peakVec.size(), 200*(1-y));
                ofDrawBitmapString(ofToString(i+1), 0,200*(1-y)-2);
            }
            
    
    
            ofBeginShape();
            for (int i = 0; i < iter->envelopeVec.size(); i++){
                float x = iter->envelopeVec[(bufferCounter+i)%iter->envelopeVec.size()];
                float y = ofMap(x, iter->thresholds[THRESHOLD_MIN], iter->thresholds[THRESHOLD_MAX], 0.0f, 1.0f,true);
                ofVertex(i,200*(1-y) );
            }
            ofEndShape(false);
            
            ofPopMatrix();
            ofPopStyle();
            
            ofPushStyle();
            ofPushMatrix();
            ofTranslate(0, 640, 0);
        
        
            
            ofSetColor(iter->bTriggered ? 255 : 50);
            ofRect(0, 0, iter->peakVec.size(), 200);
            
            ofSetColor(245, 58, 135);
        
            for (int i=0; i<THRESHOLD_NUM; i++) {
                float y =  ofMap(iter->thresholds[i], iter->thresholds[THRESHOLD_MIN], iter->thresholds[THRESHOLD_MAX], 0.0f, 1.0f,true);
                ofLine(0, 200*(1-y), iter->peakVec.size(), 200*(1-y));
                ofDrawBitmapString(ofToString(i+1), 0,200*(1-y)-2);
            }
            
                           
            ofBeginShape();
            for (int i = 0; i < iter->limiterVec.size(); i++){
                
                float x = iter->limiterVec[(bufferCounter+i)%iter->limiterVec.size()];
                float y = ofMap(x, iter->thresholds[THRESHOLD_MIN], iter->thresholds[THRESHOLD_MAX], 0.0f, 1.0f,true);
                ofVertex(i,200*(1-y) );
                
            }
            ofEndShape(false);
            
            ofPopMatrix();
            ofPopStyle();
        
            ofPopMatrix();
        
            
            //ofSetColor(255*((d+1) & 0x4), 255*((d+1) & 0x2), 255*((d+1) & 0x1));
            
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
        
        bool bTriggeredOn = c.bTriggered;
        bool bTriggeredOff = !c.bTriggered;
        
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
            
            if (!bTriggeredOn && c.envelope>c.thresholds[THRESHOLD_TRIGGER_ON]) {
                bTriggeredOn = true;
                c.bTriggered = true;
                c.pos=ofVec3f(ofRandom(ofGetWidth()),ofRandom(ofGetHeight()),0);
            } 
            
            if (!bTriggeredOff && c.envelope<c.thresholds[THRESHOLD_TRIGGER_OFF]) {
                bTriggeredOff = true;
                c.bTriggered = false;
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
    
    bSetThresh = false;
    
    switch (key) {
        case 'f':
            ofToggleFullscreen();
            break;
        case 'd':
            bDebug = !bDebug;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
            curThresh = (key-'1') % THRESHOLD_NUM;
            bSetThresh = true;
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
    
    if (bSetThresh) {
        channel &c=channels[(x / (VECTOR_SIZE+10)) % channels.size()];
        c.thresholds[curThresh] = exp((10-y)*log(10)/20/2);

        
        ofxXmlSettings xml;
        xml.addTag("settings");
        xml.pushTag("settings");
        for (vector<channel>::iterator citer=channels.begin();citer!=channels.end();citer++) {
            xml.addTag("channel");
            xml.pushTag("channel",distance(channels.begin(), citer));
            for (vector<float>::iterator titer=citer->thresholds.begin();titer!=citer->thresholds.end();titer++) {
                xml.addTag("threshold");
                xml.setValue("threshold", *titer,distance(citer->thresholds.begin(), titer));
            }
            xml.popTag();
        }
        xml.popTag();
        
        xml.saveFile("settings.xml");
        
    }
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
    if (!bSetThresh) {
        int ix = x * image.getWidth()/ofGetWidth();
        int iy = y*image.getHeight()/ofGetHeight();
        buffers[counter % buffers.size()][iy*(int)image.getWidth()+ix]= 1500;
    }
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

