#include "testApp.h"
#include <sstream>
#include <iomanip>
#include "Settings.h"

#define NUMBER_OF_PAGES 27


//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
    
    if ([[UIDevice currentDevice].systemVersion floatValue] < 6.0) {
        ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
    } else {
        ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_PORTRAIT);
    }
    
    switch (iPhoneGetDeviceType()) {
        case OFXIPHONE_DEVICE_IPAD:
           
            
            break;
        case OFXIPHONE_DEVICE_IPHONE:
        case OFXIPHONE_DEVICE_IPODTOUCH:
            
            
            break;
        default:
            break;
    }


	
    
    action_url = "http://"+Settings::getString("host")+":"+Settings::getString("port")+"/postImage.of";
	ofAddListener(upload.newResponseEvent,this,&testApp::newResponse);
	upload.start();
    counter = 0;
    
    ofxRegisterStillCameraNotification(this);
    ofxRegisterVolumeButtonsNotification(this);
    buttons.start();
    cout << ofGetWidth() << "x" << ofGetHeight() << endl;
    offscreen.setup(ofGetWidth(),ofGetHeight());
    pixels.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);
    
   
    
//    if (iPhoneGetDeviceType() == OFXIPHONE_DEVICE_IPHONE) {
//        mat.translate(64, 0, 0);
//        mat.scale(5.0/6.0, 5.0/6.0, 1.0);
//    }
    
    ofEnableAlphaBlending();
    
    sliderPrefs prefs;

       
//    ofDirectory dir;
//    dir.allowExt("png");
//    dir.listDir(ofToDataPath("masks"));
//    for (int i=0; i<dir.numFiles(); i++) {
//        cout << "loading " << dir.getName(i) << endl;
//		card c;
//        c.state = STATE_MEMORY_UNLOADED;
//        c.filename = dir.getPath(i);
//		cards.push_back(c);
//		prefs.pages.push_back(ofPoint(0,ofGetHeight()*i));
//		
//	}
    
    
    for (int i=0;i<NUMBER_OF_PAGES;i++) {
        stringstream stream;
        stream << std::setfill('0') << std::setw(2) << i+1;
        card c;
        c.state = STATE_MEMORY_UNLOADED;
        
        c.filename = "iphone/PG-1" + stream.str() + ".png";
        cout << c.filename << endl;
        c.bMask = false;
        c.audio.loadSound("iphone/AUDIO-1"+stream.str()+".caf");
		cards.push_back(c);
		prefs.pages.push_back(ofPoint(0,ofGetHeight()*(2*i)));
        
        c.filename = "iphone/MASK-1" + stream.str() + ".png";
        cout << c.filename << endl;

        c.bMask = true;
		cards.push_back(c);
		prefs.pages.push_back(ofPoint(0,ofGetHeight()*(2*i+1)));
    }
    
	prefs.direction = SLIDER_VERTICAL;
	prefs.bCyclic = true;
	prefs.lastPageSize = ofPoint(0,ofGetHeight());
	slider.setup(1,prefs);
    
    cam.preview();
    
    startThread();
    
    lastPage = 0;
    
    cards[0].audio.play();
    
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(255,255,255);	
   
    cam.update();
    slider.update();
    
    
    int j=0;
    int i=slider.getCurrentPage()-2;
    
    for (;j<5;i++,j++) {
        int k = (i+cards.size())%cards.size();
        if (cards[k].state==STATE_MEMORY_LOADED) {
            cards[k].image.setUseTexture(true);
            cards[k].image.update();
            cards[k].state=STATE_TEXTURE_LOADED;
             cout << "load texture " << k << endl;
            
        }
    }
    for (;j<cards.size();i++,j++) {
        int k = (i+cards.size())%cards.size();
        if (cards[k].state==STATE_TEXTURE_LOADED) {
            cards[k].image.getTextureReference().clear();
            cards[k].state = STATE_TEXTURE_UNLOADED;
             cout << "unload texture " << k << endl;
        }
    }
    
    int lastAudio = lastPage/2;
    int currentAudio = slider.getCurrentPage()/2;
    
    if ( lastAudio!=currentAudio) {
        cards[lastAudio*2].audio.stop();
        cards[currentAudio*2].audio.play();
    }
    
    if (lastPage!=slider.getCurrentPage()) {
        
        lastPage = slider.getCurrentPage();
        if (photo.isAllocated()) {
            upload.upload(ofxFile("card_"+ofToString(counter)+".jpg", ofxiPhoneGetDocumentsDirectory()+"temp.jpg",action_url));
            photo.clear();
            counter++;
        }
    }
    
}

void testApp::threadedFunction() {
    
    while( isThreadRunning() != 0 ){
        
        int j=0;
        int i=slider.getCurrentPage()-2;
        
        for (;j<5;i++,j++) {
            int k = (i+cards.size())%cards.size();
            if (cards[k].state==STATE_MEMORY_UNLOADED) {
                cards[k].image.setUseTexture(false);
                cards[k].image.loadImage(cards[k].filename);
                cards[k].state=STATE_MEMORY_LOADED;
                cout << "load memory " << k << endl;
            }
        }
        for (;j<cards.size();i++,j++) {
            int k = (i+cards.size())%cards.size();
            if (cards[k].state==STATE_TEXTURE_UNLOADED) {
                cards[k].image.clear();
                cards[k].state=STATE_MEMORY_UNLOADED;
                cout << "unload memory " << k << endl;
            }
        }
        ofSleepMillis(10);
        
        
    }
    
    
    
}

void testApp::drawCard(card &c) {
    if (c.bMask && photo.isAllocated()) {
        photo.draw(0, 0, ofGetWidth(), ofGetHeight());
    } else {
        if (c.state==STATE_TEXTURE_LOADED) {
            c.image.draw(0, 0,ofGetWidth(),ofGetHeight());
        }
    }
}

//--------------------------------------------------------------
void testApp::draw(){	
    ofSetHexColor(0xFFFFFF);
    
    if (cam.getIsPlaying() && cam.getIsFrameVisible()) {
//        float tw = imageRect.width/imageRect.height*cam.getHeight()/cam.getWidth();
//        cam.draw(imageRect, ofRectangle((1-tw)/2,0,tw,1));
        
        int yoffset = cam.getHeight()<ofGetHeight() ? (ofGetHeight()-cam.getHeight())/2 : 0;
        cam.draw(ofRectangle(0, yoffset, cam.getWidth(), cam.getHeight()), ofRectangle(1, 1, -1, -1));
    }
    
    
    ofPushMatrix();
	slider.transform();
	
	
    ofPushMatrix();
    ofTranslate(0, -ofGetHeight(), 0);
    drawCard(*(cards.end()-1));
    ofPopMatrix();
    
    for (vector<card>::iterator iter=cards.begin(); iter!=cards.end(); iter++) {
        
        ofPushMatrix();
        ofTranslate(0, distance(cards.begin(), iter) *ofGetHeight(), 0);
        drawCard(*iter);
        ofPopMatrix();
       
    }
    
    ofPushMatrix();
    ofTranslate(0, ofGetHeight()*cards.size(), 0);
    drawCard(*(cards.begin()));
    ofPopMatrix();
    
    ofPopMatrix();
    
    
}

void testApp::exit() {
    ofxUnregisterVolumeButtonsNotification(this);
     buttons.stop();
    ofxUnregisterStillCameraNotification(this);
       
}

void testApp::toggleAudio() {
    int currentAudio = slider.getCurrentPage()/2;
    if (cards[currentAudio*2].audio.getIsPlaying()) {
        cards[currentAudio*2].audio.stop();
    } else {
        cards[currentAudio*2].audio.play();
    }
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    slider.touchDown(touch.x, touch.y,touch.id);
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
   slider.touchMoved(touch.x, touch.y,touch.id);
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
    slider.touchUp(touch.x, touch.y,touch.id);
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
//    mailStruct m;
//    m.subject = "Hindsight, first prototype";
//    for (vector<card>::iterator iter=cards.begin(); iter!=cards.end(); iter++) {
//        if (iter->photo.isAllocated()) {
//            m.attachments.push_back(attachmentStruct(iter->buf,"image/jpeg", "CARD_"+ofToString(distance(cards.begin(), iter))+".jpg"));
//        }
//    }
//    mail.sendMail(m);
    
//    if (slider.getCurrentPage() % 2) {
//        cam.snap();
//    }
    
    
    toggleAudio();
 
}

//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs& args){

}



void testApp::pictureTaken(ofPixels &pixels) {
    //int yoffset = cam.getHeight()<ofGetHeight() ? (ofGetHeight()-cam.getHeight())/2 : 0;
    card &c = cards[slider.getCurrentPage()];
    
//    if (cam.getHeight()>=ofGetHeight()) {
//        pixels.crop(cam.getWidth()-ofGetWidth(), cam.getHeight()-ofGetHeight(), ofGetWidth(), ofGetHeight());
//    }
   
    //photo.allocate(c.image.width, c.image.height, OF_IMAGE_COLOR_ALPHA);
    pixels.crop(cam.getWidth()-ofGetWidth(), cam.getHeight()-ofGetHeight(), ofGetWidth(), ofGetHeight());
    photo.setFromPixels(pixels);
    
    
    offscreen.begin();
    ofSetColor(255);
    photo.draw(ofGetWidth(),ofGetHeight(),-ofGetWidth(),-ofGetHeight());
    c.image.draw(0, 0);
    
    glReadPixels(0, 0, this->pixels.getWidth(), this->pixels.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, this->pixels.getPixels());
    
    offscreen.end();
    photo.setFromPixels(this->pixels);
    photo.setImageType(OF_IMAGE_COLOR);
    photo.saveImage(ofxiPhoneGetDocumentsDirectory()+"temp.jpg",OF_IMAGE_QUALITY_MEDIUM); //
    //c.buf=ofBufferFromFile(ofxiPhoneGetDocumentsDirectory()+"temp.jpg");
    
    
}

void testApp::volumeButtonPressed(int &button) {
    
    switch (button) {
        case VOLUME_BUTTON_UP:
            if (cards[slider.getCurrentPage()].bMask) {
                if (!photo.isAllocated()) {
                    cam.snap();
                } else {
                    photo.clear();
                }
            }
            break;
        case VOLUME_BUTTON_DOWN:
            toggleAudio();
            break;
        default:
            break;
    }
    
    
}

//--------------------------------------------------------------
void testApp::newResponse(ofxHttpResponse & response){
	//responseStr = ofToString(response.status) + ": " + (string)response.responseBody;
}


