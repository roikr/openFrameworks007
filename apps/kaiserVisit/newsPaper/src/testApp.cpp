#include "testApp.h"
#include "ofxXmlSettings.h"

#define FADE_DURATION 500



//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(255,255,255);
    
    
    serial= new ofxSerial("/dev/tty.usbmodemfa131", 9600);
    
    ofEnableAlphaBlending();
    
    ofxXmlSettings xml;
    
    if (xml.loadFile("newspaper.xml")) {
        video.loadMovie(xml.getAttribute("newspaper", "video", ""));
        background.loadImage(xml.getAttribute("newspaper", "background", ""));
        xml.pushTag("newspaper");
        for (int i=0;i<xml.getNumTags("page");i++) {
            page p;
            p.start=xml.getAttribute("page", "start", 0,i);
            p.loop=xml.getAttribute("page", "loop", 0,i);
            p.end=xml.getAttribute("page", "end", 0,i);
            p.mask = xml.getAttribute("page", "mask", 0,i);
            pages.push_back(p);
        }
        xml.popTag();
        
        video.play();
    }

	alpha = 1.0;
    nextPage = currentPage = 0;
    
    lang = 1>>8;
    
}

//--------------------------------------------------------------
void testApp::update(){
    
    string str;
    if (serial && serial->readUntil(str, 'e')) {
        
        for (int i=8;i<11;i++) {
            if (str[i]=='1' && lang != 1 << i) {
                lang = 1 << i;
            }
        }
        
        pins = lang;
        for (int i=0;i<8;i++) {
            if (str[i]=='1') {
                pins |= 1 << i;
            }
        }
    }
    
    
    if (serial && !bFadeOut && (pins & pages[currentPage].mask) != pages[currentPage].mask ) {
        pageUp();
    }
    
    if (bFadeOut) {
        if (ofGetElapsedTimeMillis()-fadeStart > FADE_DURATION) {
            if (currentPage != nextPage) {
                currentPage = nextPage;
                video.setPaused(true);
                video.setFrame(pages[currentPage].start+1);
                video.setPaused(false);
                bFadeOut = false;
                
            }
        } else {
            if (serial) {
                for (vector<page>::iterator iter=pages.begin();iter!=pages.end();iter++) {
                    if (iter->mask && (pins & iter->mask) == iter->mask) {
                        pageDown(distance(pages.begin(), iter));
                        break;
                    }
                }
            }
        }
    }
    
    
    
    
    
    if (video.getCurrentFrame() > pages[currentPage].end+1) {
        video.setPaused(true);
        video.setFrame(pages[currentPage].loop+1);
        video.setPaused(false);
    }
    
    video.idleMovie();

    
    alpha = ofMap(ofGetElapsedTimeMillis()-fadeStart,0,FADE_DURATION,bFadeOut ? 1 : 0,bFadeOut ? 0 : 1,true);
    
}

//--------------------------------------------------------------
void testApp::draw(){

	ofSetColor(255,255,255,255*(1-alpha));
    background.draw(0,0);
    ofSetColor(255, 255, 255,255*alpha);
    video.draw(0, 0);
    
    ofSetColor(255, 0, 0);
    ofDrawBitmapString(ofToString(pins), 10, 10);    
}

void testApp::pageUp() {
    cout << "pageUp" << endl;
    fadeStart = ofGetElapsedTimeMillis();
    bFadeOut = true;
}

void testApp::pageDown(int pageNum) {
    cout << "pageDown: " << pageNum << endl;
    if (pageNum < pages.size()) {
        nextPage = pageNum;
    } else {
        nextPage = currentPage;
    }
    
    if (nextPage==currentPage) {
        fadeStart = ofGetElapsedTimeMillis() - FADE_DURATION*alpha;
        
        bFadeOut = false;
    }
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
    
    pageUp();
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    pageDown(key-'1');
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
