#include "testApp.h"
#include "ofxXmlSettings.h"

#import <MediaPlayer/MediaPlayer.h>

//--------------------------------------------------------------
void testApp::setup(){	
   
    
    CGRect frame = CGRectMake(0, -100, 10, 0);
    MPVolumeView *volumeView = [[[MPVolumeView alloc] initWithFrame:frame] autorelease];
    [volumeView sizeToFit];
    [[[[UIApplication sharedApplication] windows] objectAtIndex:0] addSubview:volumeView];
    
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	//If you want a landscape oreintation 
	//iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
	    
//    keyboard = new ofxiPhoneKeyboard(80,60,160,40);
    keyboard = new ofxiPhoneKeyboard(80,-40,160,40);
	keyboard->setVisible(false);
   	keyboard->setBgColor(75, 75, 75, 255);
	keyboard->setFontColor(150,150,150, 255);
	keyboard->setFontSize(32);

	    
    font.loadFont("LCDPHONE.TTF", 32);
	
	int bufferSize = 256;
    float size = ofGetWidth()/4;
    
    for (int i=0; i<16;i++) {
        button b;
//        string filename = "beep"+ofToString(i)+".caf";
        std::ostringstream filename;
        filename << "HFBEEP_" << std::setw( 3 ) << std::setfill( '0' ) << i << ".caf";
        cout << filename << ": " << b.audio.load(ofToDataPath(filename.str()), bufferSize) << endl;
        b.rect = ofRectangle(i%4*size,floor(i/4)*size+(ofGetHeight()-ofGetWidth())/2,size,size);
        b.text = ofToString(i+1);
        buttons.push_back(b);
    }
    
   
    ofxXmlSettings xml;
    if (xml.loadFile(ofxiPhoneGetDocumentsDirectory()+"buttons.xml")) {
        [[MPMusicPlayerController applicationMusicPlayer] setVolume:xml.getAttribute("buttons", "volume", 1.0f)];
        xml.pushTag("buttons");
        for (int i=0;i<xml.getNumTags("button");i++) {
            string text = xml.getAttribute("button", "text", "",i);
            buttons[i].text = text;
            
        }
        xml.popTag();
    }
    
    slider.setup(ofRectangle((ofGetWidth()-120)/2,ofGetHeight()-73,120,60),[[MPMusicPlayerController applicationMusicPlayer] volume]);
    
    
    
	
    
	ofSoundStreamSetup(2, 0, this, 44100, bufferSize, 4);
	
    ofBackground(75);
    ofSetColor(150);

    ofEnableAlphaBlending();
    
    background.loadImage("background.png");
   
    bEditMode = false;
    bButtonDown = false;
    delayTimer = ofGetElapsedTimeMillis();
    trans.setup();
}

//--------------------------------------------------------------
void testApp::update(){
    
    
    trans.update();
    
    
    if(keyboard->isKeyboardShowing()) {
        buttons[keyNum].text = keyboard->getLabelText();
    } else {
        if (bEditMode) {
            bEditMode = false;
            trans.start(EASE_OUT_QUAD,ofVec2f(0,0));
            keyboard->setVisible(false);
            
            
        }
    }
    
    bool bPlaying = false;
    for (vector<button>::iterator iter = buttons.begin(); iter!=buttons.end(); iter++) {
        
        if (iter->audio.getNumPlaying()) {
            bPlaying = true;
            break;
        }
    }
    
    if (!bPlaying && !queue.empty() && ofGetElapsedTimeMillis()>delayTimer) {

        buttons[queue.back()].audio.play();
        queue.pop_back();
    }
    
    slider.value = [[MPMusicPlayerController applicationMusicPlayer] volume];
}

//--------------------------------------------------------------
void testApp::draw(){
    
    
    ofSetColor(255, 255, 255, 50);
    
    background.draw(0, 0);
    
    trans.begin();
	for (vector<button>::iterator iter = buttons.begin(); iter!=buttons.end(); iter++) {
        
        ofSetColor(150);
        
        if (iter->audio.getNumPlaying() || ((bButtonDown || bEditMode) && distance(buttons.begin(), iter) == keyNum)) {
            ofFill();
            ofRect(iter->rect);
            ofSetColor(75);
           
        } else {
            ofNoFill();
            ofRect(iter->rect);
        }
        
        float width = font.stringWidth(iter->text);
        float height = font.stringHeight(iter->text);
        
        ofVec2f pos = iter->rect.getCenter()+0.5*ofVec2f(-width,height);
        font.drawString(iter->text, pos.x, pos.y);
        
        
    }
    trans.end();
    
    slider.draw();
   
   
}

//--------------------------------------------------------------
void testApp::exit(){
    ofxXmlSettings xml;
    xml.addTag("buttons");
    xml.addAttribute("buttons", "volume", [[MPMusicPlayerController applicationMusicPlayer] volume],0);
    xml.pushTag("buttons");
    
    for (vector<button>::iterator iter = buttons.begin(); iter!=buttons.end(); iter++) {
        int i = distance(buttons.begin(), iter);
        xml.addTag("button");
        xml.addAttribute("button", "text", iter->text,i);
    }
    xml.popTag();
    
    xml.saveFile(ofxiPhoneGetDocumentsDirectory()+"buttons.xml");
//    [[MPMusicPlayerController applicationMusicPlayer] setVolume:launchVolume];
}

void testApp::audioRequested( float * output, int bufferSize, int nChannels ) {
	memset(output, 0, sizeof(float)*bufferSize*nChannels);
    for (vector<button>::iterator iter = buttons.begin(); iter!=buttons.end(); iter++) {
        
        if (iter->audio.getNumPlaying()) {
            iter->audio.mixChannel(output, 0, nChannels);
            iter->audio.mixChannel(output, 1, nChannels);
            iter->audio.postProcess();
            
            if (!iter->audio.getNumPlaying()) {
                delayTimer = ofGetElapsedTimeMillis() + 300;
            }
        }
    }
	
	
	
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    if(keyboard->isKeyboardShowing()) {
        return;
    }
    
    if (touch.id == 0) {
        
        
        vector<button>::iterator iter;
        for (iter = buttons.begin(); iter!=buttons.end(); iter++) {
            
            if (iter->rect.inside(ofVec2f(touch.x,touch.y))) {
                break;
            }
        }
        
        if (iter!=buttons.end()){
            downTime = ofGetElapsedTimeMillis();
            keyNum = distance(buttons.begin(), iter);
            bButtonDown = true;
        }
    }
    
//    slider.touchDown(touch);
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
    if(keyboard->isKeyboardShowing()) {
        return;
    }
    
    
//    slider.touchMoved(touch);
    
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
    if(keyboard->isKeyboardShowing()) {
        return;
    }
    
    if (touch.id == 0 && bButtonDown) {
        bButtonDown = false;
        if (ofGetElapsedTimeMillis()-downTime>250) {
            
            bEditMode = true;
           
            if(!keyboard->isKeyboardShowing()){
                keyboard->setVisible(true);
                keyboard->setText(buttons[keyNum].text);
                keyboard->openKeyboard();
                
            } 
            
            ofVec2f pos = 0.5*ofVec2f(ofGetWidth(),0.5*(ofGetHeight()-ofGetWidth()));
            ofVec2f rel = buttons[keyNum].rect.getCenter()-ofVec2f(buttons[0].rect.x,buttons[0].rect.y) ;
            trans.start(EASE_OUT_QUAD,pos-rel);
            
        } else {
            queue.push_front(keyNum);
            
        }
    }
    
//    slider.touchUp(touch);
    
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){

}

void testApp::touchCancelled(ofTouchEventArgs &touch) {
    
}

//--------------------------------------------------------------
void testApp::lostFocus(){

}

//--------------------------------------------------------------
void testApp::gotFocus(){

}

//--------------------------------------------------------------
void testApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void testApp::deviceOrientationChanged(int newOrientation){

}

