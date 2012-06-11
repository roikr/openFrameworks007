#include "testApp.h"

#define MAX_SIZE 50.0f
#define MAX_DEPTH 32.0f
#define VELOCITY -2.0
#define NUM_GROUPS 5
#define NUM_ENV 200
#define PROPAGATION_VELOCITY 0.5f // pixels per ms

void randomStar(star &s,bool bInit) {
        
    s.pos.x = ofRandom(-MAX_SIZE, MAX_SIZE);
    s.pos.y = ofRandom(-MAX_SIZE, MAX_SIZE);
    s.pos.z = bInit ? ofRandom(1,MAX_DEPTH) : MAX_DEPTH;
    s.startTime = bInit ? ofGetElapsedTimef()-(s.pos.z-MAX_DEPTH)/VELOCITY : ofGetElapsedTimef();
    s.group = 0;//rand() % NUM_GROUPS;
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
    
    //rot=ofVec2f(1.0,0.0)*0.02;
    ofSetCircleResolution(90);
}


//--------------------------------------------------------------
void testApp::update(){
    
    for (vector<wave>::iterator iter=waves.begin();iter!=waves.end();iter++) {
        iter->pos.z = MAX_DEPTH+(ofGetElapsedTimef()-iter->startTime)*VELOCITY;
        float k=128/iter->pos.z;
        iter->sPos = (iter->pos)*k+ofVec3f(ofGetWidth()/2,ofGetHeight()/2);
        iter->sPos.z = 1-iter->pos.z/MAX_DEPTH;
        
    }
    
    for (vector<star>::iterator iter=stars.begin(); iter!=stars.end(); iter++) {
       
        iter->pos.z = MAX_DEPTH+(ofGetElapsedTimef()-iter->startTime)*VELOCITY;
        float k=128/iter->pos.z;
        iter->pos.x+=rot.x*k;
        iter->pos.y+=rot.y*k;
        
        
        if  (iter->pos.z<=0 || ofVec2f(iter->pos).length()*k>ofGetHeight()/2) {
            randomStar(*iter,true);
            
            k=128/iter->pos.z;
           
            if (ofVec2f(iter->pos*k).length()>ofGetHeight()/2 && ((ofVec2f(iter->pos)+rot)*k).length()<=ofGetHeight()/2) {
                
            } else {
                iter->pos.z = MAX_DEPTH;
            }
            
        }
        
        
        k=128/iter->pos.z;
        iter->sPos = (iter->pos)*k+ofVec3f(ofGetWidth()/2,ofGetHeight()/2);
        iter->sPos.z = 1-iter->pos.z/MAX_DEPTH;
        
        
        
        float dist = 100;
        
        for (vector<wave>::iterator witer=waves.begin();witer!=waves.end();witer++) {
            dist = min(dist,abs(witer->sPos.distance(iter->sPos)-(ofGetElapsedTimef()-witer->startTime)*witer->sPos.z*1000));
            
        }
        
        float sigma = 1.0;
        iter->color.setHsb(0.0,255*1/(sigma*sqrt(2*PI))*exp(-pow(dist/sigma, 2)/2),iter->sPos.z*255,255); // 
        
                
        //color.setBrightness(brt);
         
        switch (iter->group) {
            case 1: 
                
                iter->sPos+=ofVec3f(ofClamp(channels[0].amplitude*100,0,1)*iter->sPos.z*5,0).rotate(ofRandom(360),ofVec3f(0,0,1));
        
                break;
                
            case 2: {
                float r = ofClamp(channels[0].amplitude*100,0,1)*iter->sPos.z*5;
                float N = 3;
                float lambda = M_PI/2;//(N-1)/N*M_PI/2;
                float a = 3;
                float b = 4;
                float k = 0.008;
                iter->sPos+=ofVec3f(r*sin(k*a*ofGetElapsedTimeMillis()+lambda+iter->randomPhase),r*sin(k*b*ofGetElapsedTimeMillis()+iter->randomPhase));
                
                
            } break;
            
            default: 
                
                break;
                
        }
        
//        ofVec3f dist;
//        float r = 0;
        
//        for (vector<wave>::iterator witer=waves.begin();witer!=waves.end();witer++) {
//            ofVec3f vec = iter->sPos-witer->pos;
//            
//            float mu = vec.length();
//            float sigma = 1; // sqrt(0.2);
//            float lambda = 0.0005;
//            
//            float dt = ofGetElapsedTimeMillis()-witer->startTime;
//            
//            float gauss = 1/(sigma*sqrt(2*PI))*exp(-pow((dt/2-mu)/sigma, 2)/2);
//
//                       
//            //dist+=vec.getNormalized()*2*sin(dt/20+mu/750); // exp(-lambda*dt)*
//            r+=2*sin(dt/50+mu/750); // exp(-lambda*dt)*
//        }
        
        //iter->sPos.z+=r;
  
        
    }
    
    
}


//--------------------------------------------------------------
void testApp::draw(){
    ofFill();
    
    
    
    for (vector<star>::iterator iter=stars.begin(); iter!=stars.end(); iter++) {
       
        switch (iter->group) {
            case 3: {
                int length = ofClamp(channels[0].amplitude*100,0,1)*20;
                
                
                for (int i = 0; i < length; i++){
                    
                    float f = 20;
                    float z = MAX_DEPTH+(ofGetElapsedTimef()-f*(length-i)-iter->startTime)*VELOCITY;
                    
                    float k=128.0/z;
                    ofColor color;
                    ofSetColor(color.r,color.g,color.b,255*i/length); //ofMap(i,0,length,0,255));//255*i/envSize);
                    ofCircle(iter->pos.x*k + ofGetWidth()/2, iter->pos.y*k + ofGetHeight()/2, iter->sPos.z/2);
                }
                
                
                
                
                // ofRect(x, y, size,size);
               
            }  break;
            default: 
               
                break;
                
                
        }
        
        
        ofSetColor(iter->color.r,iter->color.g,iter->color.b);
        ofCircle(iter->sPos, iter->sPos.z*2.5);
       
        
        
        
    }
    
    ofNoFill();
    ofSetColor(150,150,150,150);
    
    for (vector<wave>::iterator iter=waves.begin();iter!=waves.end();iter++) {
        ofCircle(iter->sPos, (ofGetElapsedTimef()-iter->startTime)*iter->sPos.z*1000); // 
        
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
        case 'w': {
            wave w;
            
            w.pos=ofVec3f(ofRandom(MAX_SIZE),0,MAX_DEPTH).rotate(ofRandom(360), ofVec3f(0,0,1));
            
            //w.src= ofVec3f(ofGetWidth()/2,ofGetHeight()/2);
            
            w.startTime = ofGetElapsedTimef();
            waves.push_back(w);
        } break;
            
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

