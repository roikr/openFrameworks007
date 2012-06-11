#include "testApp.h"

#include "ofxXmlSettings.h"

#define VECTOR_SIZE 150
#define RECT_HEIGHT 150


#define MAX_SIZE 50
#define MAX_DEPTH 32
#define NUM_GROUPS 5


void randomStar(star &s,float velocity,bool bInit) {
    
    s.pos.x = ofRandom(-MAX_SIZE, MAX_SIZE);
    s.pos.y = ofRandom(-MAX_SIZE, MAX_SIZE);
    s.pos.z = bInit ? ofRandom(1,MAX_DEPTH) : MAX_DEPTH;
    s.startTime = bInit ? ofGetElapsedTimeMillis()-(s.pos.z-MAX_DEPTH)/(velocity/1000) : ofGetElapsedTimeMillis();
    s.group = rand() % NUM_GROUPS;
    s.phase = ofRandom(PI*2);
    s.frequency = ofRandom(0.5,5);
}


//--------------------------------------------------------------
void testApp::setup(){
    ofBackground(0);
    ofSetVerticalSync(true);
    ofEnableSmoothing();
    ofSetCircleResolution(180);
       
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
    
    
    
    
    scale = xml.getValue("scale", 1.5);
    velocity = xml.getValue("velocity", -0.05);
    rot = ofVec2f(0.0,0.0)*xml.getValue("rotation", 0.0001) ;
    
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
    
    
    stars.assign(512, star());
    for (vector<star>::iterator iter=stars.begin(); iter!=stars.end(); iter++) {
        randomStar(*iter,velocity,true);
    }
    
    quadWarp.setup();
    
    
}


//--------------------------------------------------------------
void testApp::update(){
    
    for (vector<channel>::iterator iter=channels.begin(); iter!=channels.end(); iter++) {
        if (iter->bTriggered) {
            int ix = iter->pos.x * image.getWidth()/ofGetWidth();
            int iy = iter->pos.y *image.getHeight()/ofGetHeight();
            
            float y = ofMap(iter->envelope, iter->thresholds[THRESHOLD_MIN], iter->thresholds[THRESHOLD_MAX], 0.0f, 1.0f,true);
            
            
            buffers[counter % buffers.size()][iy*(int)image.getWidth()+ix]= 255*y;
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
    
    for (int y=0;y<image.height; y++) {
        for (int x=0; x<image.width; x++) {
            color.setHsb(150, 100, ofClamp(buffer1[y*width+x], 0, 255));//);
            image.getPixelsRef().setColor(x, y, color);
        }
    }
    
    
    
    image.update();
    
    counter++;
    
    
    for (vector<star>::iterator iter=stars.begin(); iter!=stars.end(); iter++) {
        
        iter->pos.z = MAX_DEPTH+(ofGetElapsedTimeMillis()-iter->startTime)*velocity/1000;
        float k=128/iter->pos.z;
        iter->pos.x+=rot.x*k;
        iter->pos.y+=rot.y*k;
        
        
        if  (iter->pos.z<=0 || ofVec2f(iter->pos).length()*k>scale*ofGetHeight()/2) {
            randomStar(*iter,velocity,true);
            
            k=128/iter->pos.z;
            
            
            if (ofVec2f(iter->pos*k).length()>scale*ofGetHeight()/2 && ((ofVec2f(iter->pos)+rot)*k).length()<=scale*ofGetHeight()/2) {
                
            } else {
                iter->pos.z = MAX_DEPTH;
            }
            
            
            
        }
        
        
    }
    
}

void drawGraph(vector<float>& vec,int offset,vector<float> &thresh, bool bLog) {
    
    ofSetColor(255);
    ofRect(0, 0, vec.size(), RECT_HEIGHT);
    
    ofSetColor(245, 58, 135);

    if (bLog) {
        for (int i=0; i<thresh.size(); i++) {
            float y =  20*log(thresh[i])/log(10) ;
            ofLine(0, -2*y, vec.size(), -2*y);
            ofDrawBitmapString(ofToString(i+1), i*VECTOR_SIZE/THRESHOLD_NUM,-2*y-2);
            
        }
        ofBeginShape();
        for (int i = 0; i < vec.size(); i++) {
            ofVertex(i, -2*20*log(vec[(offset+i)%vec.size()])/log(10));
        }
        ofEndShape(false);
    } else {
        
        for (int i=0; i<thresh.size(); i++) {
            float y =  ofMap(thresh[i], thresh[THRESHOLD_MIN], thresh[THRESHOLD_MAX], 0.0f, 1.0f,true);
            ofLine(0, RECT_HEIGHT*(1-y), vec.size(), RECT_HEIGHT*(1-y));
            ofDrawBitmapString(ofToString(i+1), i*VECTOR_SIZE/THRESHOLD_NUM,RECT_HEIGHT*(1-y)-2);
            
        }
        ofBeginShape();
        for (int i = 0; i < vec.size(); i++) {
            float y =  ofMap(vec[(offset+i)%vec.size()], thresh[THRESHOLD_MIN], thresh[THRESHOLD_MAX], 0.0f, 1.0f,true);
            ofVertex(i, RECT_HEIGHT*(1-y));
        }
        ofEndShape(false);
    }
    
    
    
    

}

//--------------------------------------------------------------
void testApp::draw(){
    
    quadWarp.begin();
    
    ofSetColor(255);
    
    
    image.setAnchorPoint(scale*ofGetWidth()/2, scale*ofGetHeight()/2);
    image.draw(ofGetWidth()/2, ofGetHeight()/2, scale*ofGetWidth(), scale*ofGetHeight());
    
    
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
        
        float b = sin((ofGetElapsedTimeMillis()-iter->startTime)*iter->frequency*PI*2/1000+iter->phase);
        
        color.setHsb(130+ofRandom(60),ofRandom(50),brt*ofMap(b, -1, 1, 0.75, 1),255); // 
        //color.setBrightness(brt);
        ofSetColor(color.r,color.g,color.b);
        
        
        ofCircle(x, y, size/2);
                        
        
    }
    
    ofNoFill();
    
    if (bDebug) {
        
        for (vector<channel>::iterator iter=channels.begin();iter!=channels.end();iter++) {
            
            ofPushMatrix();
            ofTranslate(10+(VECTOR_SIZE+10)*distance(channels.begin(),iter), 0,0);
    
            ofPushMatrix();
            ofTranslate(0, 10, 0);
            drawGraph(iter->peakVec, bufferCounter, iter->thresholds, true);
            ofPopMatrix();
           
            ofPushMatrix();
            ofTranslate(0, RECT_HEIGHT+20, 0);
            drawGraph(iter->peakVec, bufferCounter, iter->thresholds, false);
            ofPopMatrix();
            
            ofPushMatrix();
            ofTranslate(0, 2*RECT_HEIGHT+30, 0);
            drawGraph(iter->envelopeVec, bufferCounter, iter->thresholds, false);
            ofPopMatrix();
                       
            ofPushMatrix();
            ofTranslate(0, 3*RECT_HEIGHT+40, 0);
            drawGraph(iter->limiterVec, bufferCounter, iter->thresholds, false);
            ofPopMatrix();
        
            ofPopMatrix();
        
            
            //ofSetColor(255*((d+1) & 0x4), 255*((d+1) & 0x2), 255*((d+1) & 0x1));
            
        }
            
        
        drawCounter++;
        
        ofSetColor(225);
        string reportString = "buffers received: "+ofToString(bufferCounter)+"\ndraw routines called: "+ofToString(drawCounter)+"\nticks: " + ofToString(soundStream.getTickCount());
        ofDrawBitmapString(reportString, 32, 589);
    }
    
    quadWarp.end();
    quadWarp.draw();
    
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
                c.pos=ofVec3f(ofGetWidth()/2,ofGetHeight()/2)+ofVec3f(ofRandom(ofGetHeight()/2),0,0).rotate(ofRandom(360), ofVec3f(0,0,1));
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


void testApp::save() {
    ofxXmlSettings xml;
    xml.addTag("settings");
    xml.pushTag("settings");
    xml.addTag("scale");
    xml.setValue("scale", scale);
    xml.addTag("velocity");
    xml.setValue("velocity", velocity);
    xml.addTag("rotation");
    xml.setValue("rotation", rot.x);
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

        
        save();
        
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

