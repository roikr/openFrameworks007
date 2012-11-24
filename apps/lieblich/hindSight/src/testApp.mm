#include "testApp.h"





//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);

	// hacking of ofxiPhoneAppDelegate.mm - flip frame buffer width and height
    if ([[UIDevice currentDevice].systemVersion floatValue] < 6.0) {
        ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
    } else {
        ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_PORTRAIT);
    }
    //
    
    ofxRegisterStillCameraNotification(this);
    ofxRegisterVolumeButtonsNotification(this);
    buttons.start();
    ofxRegisterMailNotification(this);
    mail.setup();
    cout << ofGetWidth() << "x" << ofGetHeight() << endl;
    offscreen.setup(ofGetWidth(),ofGetHeight());
    pixels.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);
    
   
    
//    if (iPhoneGetDeviceType() == OFXIPHONE_DEVICE_IPHONE) {
//        mat.translate(64, 0, 0);
//        mat.scale(5.0/6.0, 5.0/6.0, 1.0);
//    }
    
    ofEnableAlphaBlending();
    
    sliderPrefs prefs;

       
    ofDirectory dir;
    dir.allowExt("png");
    dir.listDir(ofToDataPath("masks"));
    for (int i=0; i<dir.numFiles(); i++) {
        cout << "loading " << dir.getName(i) << endl;
		card c;
		c.mask.loadImage(dir.getPath(i));
		cards.push_back(c);
		prefs.pages.push_back(ofPoint(0,ofGetHeight()*i));
		
	}
    
	prefs.direction = SLIDER_VERTICAL;
	prefs.bCyclic = true;
	prefs.lastPageSize = ofPoint(0,ofGetHeight());
	slider.setup(1,prefs);
    
    cam.preview();
    
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(255,255,255);	
   
    cam.update();
    slider.update();
}

void testApp::drawCard(card &c) {
    if (c.photo.bAllocated()) {
        c.photo.draw(0, 0);
    } else {
        c.mask.draw(0, 0);
    }
}

//--------------------------------------------------------------
void testApp::draw(){	
    ofSetHexColor(0xFFFFFF);
    
    if (cam.getIsPlaying() && cam.getIsFrameVisible()) {
//        float tw = imageRect.width/imageRect.height*cam.getHeight()/cam.getWidth();
//        cam.draw(imageRect, ofRectangle((1-tw)/2,0,tw,1));
        
        cam.draw(ofRectangle(0, 0, cam.getWidth(), cam.getHeight()), ofRectangle(1, 1, -1, -1));
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
    ofxUnregisterMailNotification(this);
    mail.exit();
   
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
    mailStruct m;
    m.subject = "Hindsight, first prototype";
    for (vector<card>::iterator iter=cards.begin(); iter!=cards.end(); iter++) {
        if (iter->photo.isAllocated()) {
            m.attachments.push_back(attachmentStruct(iter->buf,"image/jpeg", "CARD_"+ofToString(distance(cards.begin(), iter))+".jpg"));
        }
    }
    mail.sendMail(m);
}

//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs& args){

}

void testApp::pictureTaken(ofPixels &pixels) {
    card &c = cards[slider.getCurrentPage()];
    pixels.crop(cam.getWidth()-ofGetWidth(), cam.getHeight()-ofGetHeight(), ofGetWidth(), ofGetHeight());
    ofImage photo;
    photo.setFromPixels(pixels);
    
    
    offscreen.begin();
    ofSetColor(255);
    photo.draw(ofGetWidth(),ofGetHeight(),-ofGetWidth(),-ofGetHeight());
    c.mask.draw(0, 0);
    
    glReadPixels(0, 0, this->pixels.getWidth(), this->pixels.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, this->pixels.getPixels());
    
    offscreen.end();
    c.photo.setFromPixels(this->pixels);
    c.photo.setImageType(OF_IMAGE_COLOR);
    c.photo.saveImage(ofxiPhoneGetDocumentsDirectory()+"temp.jpg",OF_IMAGE_QUALITY_MEDIUM); //
    c.buf=ofBufferFromFile(ofxiPhoneGetDocumentsDirectory()+"temp.jpg");
    
    
}

void testApp::volumeButtonPressed(int &button) {
    switch (button) {
        case VOLUME_BUTTON_UP:
            cam.snap();
            break;
        case VOLUME_BUTTON_DOWN:
            cards[slider.getCurrentPage()].photo.clear();
            break;
        default:
            break;
    }
    
}

void testApp::mailComposer(int &result) {
    cout << "mailComposer: " << result << endl;
}

