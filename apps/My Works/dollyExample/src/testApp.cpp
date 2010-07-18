#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(255,255,255);
	
	sound.loadSound("VOC_YOUARE/VOC_YOUARE_1.aif", false);
	movie.loadMovie("VOC_SX1_2.mov");
	
	driver.loadVectors("VOC_YOUARE/VOC_YOUARE.xml");
	
	driver.setCurrentSample("VOC_YOUARE_1");
	driver.setCurrentVideo(0);
	
	
	movie.play();
	movie.setPaused(true);
	
	ofSetFrameRate(25);
	startFrame = - 50;
}

//--------------------------------------------------------------
void testApp::update(){
	
	movie.idleMovie();
	
	
	int currentFrame = ofGetFrameNum() - startFrame;
	if ( currentFrame != lastFrame && currentFrame < driver.getNumFrames()) {
		lastFrame = currentFrame;
		int lf;
		int vf;
		if (driver.getFrame(currentFrame, lf, vf))
			movie.setFrame(vf*2+lf);
	} else {
		//movie.firstFrame();
	}
	 

    
}

//--------------------------------------------------------------
void testApp::draw(){

	ofSetColor(0xFFFFFF);
	movie.draw(20,20);

}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
    switch(key){
        case OF_KEY_LEFT:
            
        break;
        case OF_KEY_RIGHT:
            
        break;
			
		case 'p':
			sound.play();
			startFrame = ofGetFrameNum();
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
void testApp::resized(int w, int h){

}
