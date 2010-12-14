
#include "testApp.h"
#include "Constants.h"
#include "ofSoundStream.h"
#include "ofxXmlSettings.h"

#include "ofxiPhoneVideo.h"
#include "ofxiVideoGrabber.h"
#include "ofxiVideoPlayer.h"

#include <iostream>
#include "ofMain.h"




//--------------------------------------------------------------
void testApp::setup(){	
	
	ofSetOrientation(OF_LANDSCAPE);
	
	ofDisableDataPath();
	
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	//ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	//ofxiPhoneAlerts.addListener(this);
	
	background.setup(ofToResourcesPath("santa.pvr"));
	background.init();
	background.load();
	
	ofSetFrameRate(60);
	//ofBackground(255,255,255);
	
	video = new ofxiPhoneVideo;
	
	int bufferSize = 512;
	
	video->fps = 25;
	video->numIntroFrames = 1;
	video->sampleRate 			= 44100;
	video->numFrames = 25;
	video->sampleLength = 1000*video->numFrames/video->fps;
	
	video->audio.setup(bufferSize,video->sampleRate * video->sampleLength / (1000 * bufferSize));	
	
	
	//video->sample				= new float[video->numBuffers*video->bufferSize];
	//memset(video->sample, 0, video->numBuffers*video->bufferSize * sizeof(float));
	
	song.setupForSave(video->audio.getBufferSize());
	
	ofxXmlSettings xml;
	
	bool loaded = xml.loadFile(ofToResourcesPath("players.xml"));
	assert(loaded);
	
	
	xml.pushTag("players");
	numPlayers = xml.getNumTags("player");
	
	int maxX,minX;
//	maxX = 0;
//	minX = ofGetWidth();
	
	for (int i=0; i<numPlayers;i++) {
		
		player p;
		p.x = xml.getAttribute("player", "x", 0, i) ;
		p.y = xml.getAttribute("player", "y", 0, i) ;
		p.scale = xml.getAttribute("player", "scale", 1.0f, i) ;
		p.degree = xml.getAttribute("player", "degree", 0.0f, i) ;
		p.song.setup(video->audio.getBufferSize(), video->sampleRate);
		p.song.loadTrack(ofToResourcesPath(xml.getAttribute("player", "song", "", i)));
		p.video = new ofxiVideoPlayer;
		p.video->setup(video);
		
		p.pan = (float)p.x/(float)(ofGetWidth());
		
//		minX = min(p.x,minX);
//		maxX = max(p.x,maxX);
		
		players.push_back(p);
		

		
		
		
	}
	xml.popTag();
	
//	for (vector<player>::iterator iter=players.begin(); iter!=players.end(); iter++) {
//		iter->pan = (float)(iter->x - minX)/(float)(maxX-minX);
//		cout << iter-> pan << endl;
//	}
	
	
	lAudio	= new float[video->audio.getBufferSize()];
	rAudio	= new float[video->audio.getBufferSize()];
	
	
	camera = new ofxiVideoGrabber;
	camera->setup(video);
	
	
	
	songState = SONG_IDLE;
	bRecording = false;
	trigger.setAutoThresh(0.1,50);
	//trigger.setThresh(0.15);
	
	ofSoundStreamSetup(2, 1, this, video->sampleRate, video->audio.getBufferSize(), 2);
	
	camera->startCamera();
	
	songVersion = 0;
	
}




//--------------------------------------------------------------
void testApp::update()
{
	
	camera->update();
	
	
	switch (songState) {
		case SONG_IDLE:
		case SONG_PLAY:
			for (vector<player>::iterator iter=players.begin(); iter!=players.end(); iter++) { 
				iter->video->update();
			}
		case SONG_RENDER_AUDIO:
		case SONG_CANCEL_RENDER_AUDIO:
			if (! getIsPlaying()) {
				
				songState = SONG_IDLE;
				//bNeedDisplay = true;
			}
			break;
			
		case SONG_RENDER_VIDEO:
			
			if  (currentBlock / totalBlocks >= 1.0) {
				setSongState(SONG_IDLE);
				//songState = SONG_IDLE; // TODO: check why not notifying players...
				//bNeedDisplay = true;
			}
			
			
			break;
			
			
		default:
			break;
	}
	
}

//--------------------------------------------------------------
void testApp::draw()
{
	ofSetColor(255,255,255,255);
	
	for (vector<player>::iterator iter=players.begin(); iter!=players.end(); iter++)  {

	
		ofPushMatrix();
		ofTranslate(iter->x, iter->y, 0);
		ofScale(iter->scale,iter->scale,1.0);
		ofRotate(iter->degree);
		//ofTranslate(i % 2 * ofGetWidth()/2, (int)(i / 2) * ofGetHeight() /2);
		//ofScale(0.5, 0.5, 1);
		
		if (camera->getState()==CAMERA_CAPTURING) {
			camera->draw();
		} else {
			iter->video->draw();
		}
		
		//if (iter->video->getIsPlaying()) {
//			iter->video->draw();
//		} else {
//			camera->draw();
//		}
		ofPopMatrix();
	
	}
	
	ofEnableAlphaBlending();
	
	ofPushMatrix();
	background.draw(0, 0);
	ofPopMatrix();
	
	ofDisableAlphaBlending();
	
	trigger.draw();
		
}

//--------------------------------------------------------------
void testApp::exit() {
	camera->exit();
}


void testApp::record() {
	trigger.setTrigger();
	trigger.resetMeters();
	camera->startCapture();
	
	songVersion++;
	//camera->setTrigger(thresh);
	//camera->startRecording();
	//cout << "Start recording" << endl;
}

void testApp::preview() {
	for (vector<player>::iterator iter=players.begin(); iter!=players.end(); iter++)  {		
		
		iter->video->play(60,127);
		
	}
	
}

bool testApp::getIsPlaying() {
	for (vector<player>::iterator iter=players.begin(); iter!=players.end(); iter++)  {		
		if (iter->video->getIsPlaying() || iter->song.getIsPlaying()) {
			return true;
		} 
	}
	return false;
}


int  testApp::getSongState() { 
	return songState;
}

void testApp::setSongState(int songState) {
	
		
	// song is valid and can Overwritten only when FINISHING RECORD
//	if (this->songState==SONG_RECORD && songState!=SONG_RECORD) {
//		songVersion++;
//	}
	
//	if (this->songState==SONG_RENDER_VIDEO && songState!=SONG_RENDER_VIDEO) {
//		currentFrame =(ofGetElapsedTimeMillis()-startTime)  / 40;
//	}
	
	this->songState = songState;
	
	if (songState == SONG_RENDER_AUDIO || songState == SONG_RENDER_VIDEO) {
		duration = 0;
		
		for (vector<player>::iterator piter=players.begin(); piter!=players.end(); piter++)  {
			duration = max(duration, piter->song.getDuration());
		}

	}		
	
	if (songState == SONG_RENDER_VIDEO) { 
		currentBlock = 0;
	}
	
//	for (int i=0;i<3;i++) {
//		player[i].setSongState(songState);
//	}
	
	
	
	switch (songState) {
		case SONG_IDLE:
		case SONG_RENDER_AUDIO_FINISHED:
		case SONG_CANCEL_RENDER_AUDIO:
			songState = SONG_IDLE;
			for (vector<player>::iterator iter=players.begin(); iter!=players.end(); iter++)  {
				iter->song.stop();
			}
			break;
		case SONG_PLAY:
		case SONG_RENDER_AUDIO:
		case SONG_RENDER_VIDEO:
			for (vector<player>::iterator iter=players.begin(); iter!=players.end(); iter++)  {
				iter->song.play();
			}
			break;
		default:
			break;
	}
	
	bNeedDisplay = true;
}


int testApp::getSongVersion() {
	return songVersion;
}

void testApp::soundStreamStart() {
	ofSoundStreamStart();
}

void testApp::soundStreamStop() {
	ofSoundStreamStop();
}




//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	
	if (touch.x > ofGetWidth()-30) {
		
		switch ((int)(4*touch.y/(ofGetHeight()+1))) {
			case 0:
				record();
				break;
			case 1:
				preview();
				break;

			case 2:
				setSongState(SONG_PLAY);
				break;
				
			case 3:
				ofSoundStreamStop();
				renderAudio();
				ofSoundStreamStart();
				break;
			default:
				break;
		}  

	} else if (touch.x < 30) {
		trigger.setThresh(1-(touch.y/ofGetHeight()));
	}


}

//--------------------------------------------------------------

void testApp::touchMoved(ofTouchEventArgs &touch)	{	
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){	
	if (touch.x > ofGetWidth()-30) {
		
		switch ((int)(4*touch.y/(ofGetHeight()+1))) {
			case 0:
				
				break;
			case 1:
				
				break;
				
			case 2:
				setSongState(SONG_IDLE);
				break;
			default:
				break;
		}  
		
	}
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
}

void testApp::audioReceived( float * input, int bufferSize, int nChannels ) {
	
	trigger.audioReceived(input,bufferSize);
	
	
	
	if (trigger.getTriggerState() == TRIGGER_TRIGGERED) {
		trigger.resetTrigger();
		camera->record();
		bRecording = true;
	}
	
	//compressor.audioProcess(input,bufferSize);
	
	camera->audioReceived(input, bufferSize);
	
	if (bRecording && !video->audio.getIsRecording()) {
		bRecording = false;
		for (vector<player>::iterator iter=players.begin(); iter!=players.end(); iter++)  {
			iter->video->introFrame();
		}
		//gain = 0.6f/trigger.getRmsPeak();
		//printf("rms: %1.2f, players: %i, gain: %1.2f\n", trigger.getRmsPeak(),(int)players.size(),gain);
	}
}



void testApp::audioProcess(int bufferSize) {
	memset(lAudio, 0, bufferSize*sizeof(float));
	memset(rAudio, 0, bufferSize*sizeof(float));
	
	for (vector<player>::iterator piter=players.begin(); piter!=players.end(); piter++)  {
		
		switch (songState) {
			case SONG_PLAY:
			case SONG_RENDER_AUDIO: {
				vector<event> events;
				piter->song.process(events);
				
				for (vector<event>::iterator niter=events.begin(); niter!=events.end(); niter++) {
					if (niter->bNoteOn) {
						printf("player: %i, ", distance(players.begin(),piter));
						piter->video->play(niter->note,niter->velocity);
					}
				}
			
			} break;
		
			default:
				break;
		}
		
		float leftScale = 1 - piter->pan;
		float rightScale = piter->pan;
		
		piter->video->mix(lAudio, bufferSize,1.0f/players.size()*leftScale);
		piter->video->mix(rAudio, bufferSize,1.0f/players.size()*rightScale);
		piter->video->preProcess();
	}
}

void testApp::audioRequested( float * output, int bufferSize, int nChannels ) {
	
	audioProcess(bufferSize);
	
	for (int i = 0; i < bufferSize; i++){
		output[i*nChannels] = lAudio[i];// * gain;
		output[i*nChannels + 1] = rAudio[i];// * gain;
	}
	
}


void testApp::renderAudio() {
	
	setSongState(SONG_RENDER_AUDIO);
	
	cout << "renderAudio started" << endl;
	
	song.open(ofToDocumentsPath("santa.wav")); 
	
	currentBlock = 0;
	
	while (getSongState()==SONG_RENDER_AUDIO || getSongState()==SONG_CANCEL_RENDER_AUDIO) {
		
		//update(); // todo move to production for other thread
		audioProcess(video->audio.getBufferSize());
		
		song.saveWithBlocks(lAudio, rAudio);
		currentBlock++;
	}
	
	song.close();	
	
	cout << "renderAudio finished" << endl;
	
	setSongState(SONG_RENDER_AUDIO_FINISHED);
	
	totalBlocks = currentBlock;
	
}

void testApp::seekFrame(int frame) {
	
		
	int reqBlock = (float)frame/25.0f*(float)video->sampleRate/(float)video->audio.getBufferSize();
	
	for (vector<player>::iterator piter=players.begin(); piter!=players.end(); piter++) {
		piter->bDidStartPlaying = false;
	}
	
	for (;currentBlock<reqBlock;currentBlock++) { // TODO: or song finished...
		for (vector<player>::iterator piter=players.begin(); piter!=players.end(); piter++) {
			vector<event> events;
			piter->song.process(events);
			
			for (vector<event>::iterator niter=events.begin(); niter!=events.end(); niter++) {
				if (niter->bNoteOn) {
					printf("player: %i, ", distance(players.begin(),piter));
					piter->video->play(niter->note,niter->velocity);
					piter->bDidStartPlaying = true;
					
				}
			}
			
		}
	}
	
	for (vector<player>::iterator piter=players.begin(); piter!=players.end(); piter++) {
		if (piter->bDidStartPlaying) {
			piter->bDidStartPlaying = false;
		} else {
			piter->video->nextFrame();
		}

	}
	
}




float testApp::getRenderProgress(){
	
	switch (songState) {
		case SONG_RENDER_AUDIO: {
			return (float)currentBlock * (float)video->audio.getBufferSize() / (float)video->sampleRate/duration;
		}	break;
		case SONG_RENDER_VIDEO:
			return (float)currentBlock/(float)totalBlocks;
			break;
		default:
			return 0.0f;
	}
	
}

bool testApp::cameraToggle() {
	return camera->cameraToggle();
}
//--------------------------------------------------------------
void testApp::lostFocus() {
}

//--------------------------------------------------------------
void testApp::gotFocus() {
}

//--------------------------------------------------------------
void testApp::gotMemoryWarning() {
}

//--------------------------------------------------------------
void testApp::deviceOrientationChanged(int newOrientation){
}



