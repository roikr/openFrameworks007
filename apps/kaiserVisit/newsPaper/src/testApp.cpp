#include "testApp.h"
#include "ofxXmlSettings.h"

#define FADE_DURATION 500



//--------------------------------------------------------------
void testApp::setup(){
    ofHideCursor();
	ofBackground(255,255,255);

    bSerial = false;

    ofEnableAlphaBlending();

    ofxXmlSettings xml;

    if (xml.loadFile("newspaper.xml")) {
        video.loadMovie(xml.getAttribute("newspaper", "video", ""));
        video.setLoopState(OF_LOOP_NONE);
        background.loadImage(xml.getAttribute("newspaper", "background", ""));
        xml.pushTag("newspaper");

        string portname = xml.getAttribute("serial", "portname", "COM3");
        int baudrate = xml.getAttribute("serial", "baudrate",9600);

#if defined(TARGET_OSX) || defined(TARGET_LINUX)
        serial= new ofxSerial(portname,baudrate);
        bSerial = serial!=0;
#endif

        serial.listDevices();
        vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();

        bSerial = serial.setup(portname,baudrate);


        for (int i=0;i<xml.getNumTags("page");i++) {
            page p;
            p.start=xml.getAttribute("page", "start", 0,i);
            p.loop=xml.getAttribute("page", "loop", 0,i);
            p.end=xml.getAttribute("page", "end", 0,i);
            p.mask = xml.getAttribute("page", "mask", 0,i);
            p.lang = xml.getAttribute("page", "lang", 0,i);
            pages.push_back(p);
        }
        xml.popTag();

        video.play();
    }

	alpha = 1.0;
    nextPage = currentPage = 0;

    lang = 0;

    bPageDown = false;





}

void testApp::read(){


    if (!bSerial) {
        return;
    }

    int b=serial.readByte();
    if (b==-1) {
        return;
    }

     while  (b==0) {
        b=serial.readByte();
    }

        string str="";

        while (b!='e') {
            if (b!=0) {
                str+=b;
            }
            b=serial.readByte();
        }

        //cout << str << endl;



        for (int i=0;i<11;i++) {
            if (str[i]=='1') {
                if (i>=8 && !(pins & (1<<i) )) {
                    lang=i-8;
                    pageUp();
                }
                pins |= 1 << i;
            } else {
                pins &= ~(1<< i);
            }
        }




        if (!bFadeOut && (pins & pages[currentPage].mask) != pages[currentPage].mask ) {
            pageUp();
        }

        if (bFadeOut && !bPageDown) {
            for (vector<page>::iterator iter=pages.begin();iter!=pages.end();iter++) {
                if (iter->mask && (pins & iter->mask) == iter->mask && iter->lang == lang ) {
                    pageDown(distance(pages.begin(), iter));
                    break;
                }
            }
        }





}
//--------------------------------------------------------------
void testApp::update(){


#if defined(TARGET_OSX) || defined(TARGET_LINUX)
    if (bSerial) {


        serial->readUntil(str, 'e');

        if (!str.empty()) {




        }


    }
#endif




    read();


    if (bFadeOut && ofGetElapsedTimeMillis()-fadeStart > FADE_DURATION) {

        if (currentPage != nextPage ) {
            currentPage = nextPage;
            video.setPaused(true);
            video.setFrame(pages[currentPage].start+1);
            video.setPaused(false);
            bFadeOut = false;
        }

    }







    if (video.getCurrentFrame() > pages[currentPage].end+1 || video.getIsMovieDone()) {
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

 /*
    ofSetColor(255, 0, 0);
    ofDrawBitmapString(ofToString(pins & 255)+" "+ofToString(lang), 10, 10);
    ofDrawBitmapString(ofToString(video.getCurrentFrame())+" "+ofToString(ofGetFrameRate()), 10, 20);
  */
    //ofDrawBitmapString(str, 10, 30);
}

void testApp::pageUp() {
    cout << "pageUp" << endl;
    bPageDown = false;
    fadeStart = ofGetElapsedTimeMillis();
    bFadeOut = true;
}

void testApp::pageDown(int pageNum) {
    cout << "pageDown: " << pageNum << endl;
    bPageDown = true;
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
