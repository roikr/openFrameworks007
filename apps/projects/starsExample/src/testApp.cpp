#include "testApp.h"

#define MAX_SIZE 50
#define MAX_DEPTH 32
#define VELOCITY -2.0
#define NUM_GROUPS 5
#define NUM_ENV 200


void randomStar(star &s,bool bInit) {
        
    s.pos.x = ofRandom(-MAX_SIZE, MAX_SIZE);
    s.pos.y = ofRandom(-MAX_SIZE, MAX_SIZE);
    s.pos.z = bInit ? ofRandom(1,MAX_DEPTH) : MAX_DEPTH;
    s.startTime = bInit ? ofGetElapsedTimeMillis()-(s.pos.z-MAX_DEPTH)/(VELOCITY/1000) : ofGetElapsedTimeMillis();
    s.group = rand() % NUM_GROUPS;
    s.randomPhase = ofRandom(M_PI*2);
}

//--------------------------------------------------------------
void testApp::setup(){
    ofBackground(0);
    ofSetVerticalSync(true);
    stars.assign(512, star());
    
    
    for (vector<star>::iterator iter=stars.begin(); iter!=stars.end(); iter++) {
        randomStar(*iter,true);
        
    }
    
    // 0 output channels, 
	// 2 input channels
	// 44100 samples per second
	// 256 samples per buffer
	// 4 num buffers (latency)
	
	soundStream.listDevices();
	
	//if you want to set a different device id 
	//soundStream.setDeviceID(0); //bear in mind the device id corresponds to all audio devices, including  input-only and output-only devices.
	
	int bufferSize = 256;
    
    for (int i=0;i<NUM_CHANNELS;i++) {
        channel c;
        channels.push_back(c);
        channels.back().input.assign(bufferSize,0.0);
        channels.back().envelope.assign(NUM_ENV,0.0);
        channels.back().power.assign(NUM_ENV,-100);
        channels.back().amplitude = 0.0;
    }
	
	
    
    bufferCounter	= 0;
    drawCounter		= 0;
    float sampleRate = 44100;
    
    float attackMs= 10;
    float releaseMs = 50;
    ga = pow( 0.01, 1.0 / ( attackMs * sampleRate * 0.001 ) ); 
    gr = pow( 0.01, 1.0 / ( releaseMs * sampleRate * 0.001 ) ); 
    
	soundStream.setup(this, 0, NUM_CHANNELS, sampleRate, bufferSize, 4);
    
    bDebug = false;
    ofToggleFullscreen();
    ofSetFrameRate(30);
    ofEnableAlphaBlending();
}


//--------------------------------------------------------------
void testApp::update(){
    for (vector<star>::iterator iter=stars.begin(); iter!=stars.end(); iter++) {
        iter->pos.z = MAX_DEPTH+(ofGetElapsedTimeMillis()-iter->startTime)*VELOCITY/1000;
        
        float k=128/iter->pos.z;
        float x = iter->pos.x*k + ofGetWidth()/2;
        float y = iter->pos.y*k + ofGetHeight()/2;
        
        if  (iter->pos.z<=0 || ofDist(ofGetWidth()/2, ofGetHeight()/2, x, y)>ofGetHeight()/2) {
            randomStar(*iter,false);
        }
        
        
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    ofFill();
    
    
    
    for (vector<star>::iterator iter=stars.begin(); iter!=stars.end(); iter++) {
       
        float k=128/iter->pos.z;
        
        float x = iter->pos.x*k+ofGetWidth()/2;
        float y = iter->pos.y*k+ofGetHeight()/2;
        float size = (1-iter->pos.z/MAX_DEPTH) * 5;
        //float color = 
        
        ofColor color;
        //float dist = iter->chan ? lEnv[(bufferCounter-1)%lEnv.size()] : rEnv[(bufferCounter-1)%rEnv.size()];
        //float hue = iter->chan ? 0 : 160;
        //float sat = ofMap(dist, -50, 0, 0, 255,true);
        float brt = (1-iter->pos.z/MAX_DEPTH)*255;
        
        color.setHsb(0.0,0.0,brt,255); // 
        //color.setBrightness(brt);
        ofSetColor(color.r,color.g,color.b);
        
        switch (iter->group) {
            case 0: {
                float alpha = ofRandom(2*M_PI);
                float r = ofClamp(channels[0].amplitude*100,0,1)*size;
                x+=r*cos(alpha);
                y+=r*sin(alpha);
                
                ofCircle(x, y, size/2);
            }   break;
               
            case 1: {
                float r = ofClamp(channels[0].amplitude*100,0,1)*size;
                float N = 3;
                float lambda = M_PI/2;//(N-1)/N*M_PI/2;
                float a = 3;
                float b = 4;
                float k = 0.008;
                
                x+=r*sin(k*a*ofGetElapsedTimeMillis()+lambda+iter->randomPhase);
                y+=r*sin(k*b*ofGetElapsedTimeMillis()+iter->randomPhase);
            
                ofCircle(x, y, size/2);
            } break;
            case 2: {
                int length = ofClamp(channels[0].amplitude*100,0,1)*20;
                
                
                for (int i = 0; i < length; i++){
                    
                    float f = 20;
                    float z = MAX_DEPTH+(ofGetElapsedTimeMillis()-f*(length-i)-iter->startTime)*VELOCITY/1000;
                    
                    float k=128.0/z;
                    
                    ofSetColor(color.r,color.g,color.b,255*i/length); //ofMap(i,0,length,0,255));//255*i/envSize);
                    ofCircle(iter->pos.x*k + ofGetWidth()/2, iter->pos.y*k + ofGetHeight()/2, size/2);
                }
                
                ofSetColor(color.r,color.g,color.b);
                ofCircle(x, y, size/2);
                
                // ofRect(x, y, size,size);
               
            }  break;
            default: 
                ofCircle(x, y, size/2);
                break;
                
                
        }
       
        
    }
    
    ofNoFill();
    
    if (bDebug) {
	
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
        
        
        // draw the right channel:
        ofPushStyle();
            ofPushMatrix();
            ofTranslate(32, 370, 0);
        
            ofSetColor(245, 58, 135);
            int envSize = channels.front().power.size();
           ofRect(0, 0, envSize*2, 200);
            
            ofBeginShape();
                for (int i = 0; i < envSize; i++){
                    ofVertex(i*2, 100-channels.front().power[(bufferCounter+i)%envSize]);
                }
            ofEndShape(false);
            
            ofPopMatrix();
        ofPopStyle();

        
            
        
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
        for(int i=0;i<bufferSize;i++)
        {
            
                
            c.input[i] = input[nChannels*i+j];
            
            float EnvIn = abs(c.input[i]);
            
            if(c.amplitude < EnvIn)
            {
                c.amplitude *= ga;
                c.amplitude += (1-ga)*EnvIn;
            }
            else
            {
                c.amplitude *= gr;
                c.amplitude += (1-gr)*EnvIn;
            }
        }
        
        c.envelope[bufferCounter % c.envelope.size()] = c.amplitude;
        c.power[bufferCounter % c.power.size()] = 20.0*log(c.amplitude)/log(10.0f);

    
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

