#include "testApp.h"
#include <ctype.h>

//--------------------------------------------------------------

void testApp::addMovieForSample(string sampleName) {
	if (driver.doesSampleExist(sampleName)) {
		driver.setCurrentSample(sampleName);
		driver.setCurrentVideo(0);
		string videoName = driver.getVideoName();
		map<string,ofVideoPlayer*>::iterator iter = movies.find(videoName);
		if (iter==movies.end()) {
			ofVideoPlayer *video = new ofVideoPlayer;
			char str[255];
			sprintf(str,"loaded movie %s for samples %s",driver.getVideoName().c_str(),sampleName.c_str());
			ofLog(OF_LOG_VERBOSE, str);
			video->loadMovie("movies/"+videoName+".mov");
			video->play();
			video->setPaused(true);
			movies[videoName] = video;
		}
	}
}

void testApp::setMovieForSample(string sampleName) {
	if (driver.doesSampleExist(sampleName)) {
		driver.setCurrentSample(sampleName);
		driver.setCurrentVideo(0);
		movie = movies[driver.getVideoName()];
	}
	startFrame = ofGetFrameNum();
	lastFrame = 0;
	int lf;
	int vf;
			
	if (driver.getFrame(0, lf, vf))
		movie->setFrame(vf*6+lf);
}


void testApp::setup(){
	ofBackground(255,255,255);
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	driver.loadVectors("WINTER/WINTER.xml");
	
	for (int i=0; i<8; i++) {
		string currentSample = "WINTER_0"+ofToString(i+1);
		ofSoundPlayer *sound = new ofSoundPlayer;
		sound->loadSound("WINTER/"+currentSample+".aif", false);
		sounds.push_back(sound);
		addMovieForSample(currentSample);
		
	}
	
	
	
	addMovieForSample("IDLE");
	setMovieForSample("IDLE");
		
	ofSetFrameRate(25);
	startFrame = - 50;
}

//--------------------------------------------------------------
void testApp::update(){
	
	
	movie->idleMovie();
	
	
	int currentFrame = ofGetFrameNum() - startFrame;
	
	if ( currentFrame < driver.getNumFrames()) {
		if ( currentFrame != lastFrame ) {
			lastFrame = currentFrame;
			int lf;
			int vf;
			if (driver.getFrame(currentFrame, lf, vf))
				movie->setFrame(vf*6+lf);
		}
	} else {
		setMovieForSample("IDLE");
	}
	
	
	 

    
}

//--------------------------------------------------------------
void testApp::draw(){

	ofSetColor(0xFFFFFF);
	movie->draw(20,20);

}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
    switch(key){
        case OF_KEY_LEFT:
            
        break;
        case OF_KEY_RIGHT:
            
        break;
			
		case 'p':
			//sound.play();
			
			break;
	}
	
	if (isdigit(key)) {
		sounds[key-'1']->play();
		string currentSample = "WINTER_0"+ofToString(key-'0');
		setMovieForSample(currentSample);
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
