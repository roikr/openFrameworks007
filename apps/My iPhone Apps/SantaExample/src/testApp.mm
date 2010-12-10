
#include "testApp.h"
#include "ofxXmlSettings.h"

#include "ofxiPhoneVideo.h"
#include "ofxiVideoGrabber.h"
#include "ofxiVideoPlayer.h"

#include <iostream>


//--------------------------------------------------------------
void testApp::setup(){	
	
	
	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	background.loadImage("santa.png");
	
	ofSetFrameRate(60);
	ofBackground(255,255,255);
	
	video = new ofxiPhoneVideo;
	
	video->fps = 25;
	video->numIntroFrames = 1;
	video->bufferSize	= 512;
	video->sampleRate 			= 44100;
	video->numFrames = 25;
	video->sampleLength = 1000*video->numFrames/video->fps;
	video->numBuffers = 44100 * video->sampleLength / (1000 * video->bufferSize);
	video->sample				= new float[video->numBuffers*video->bufferSize];
	memset(video->sample, 0, video->numBuffers*video->bufferSize * sizeof(float));
	
	ofxXmlSettings xml;
	
	bool loaded = xml.loadFile("players.xml");
	assert(loaded);
	
	
	xml.pushTag("players");
	numPlayers = xml.getNumTags("player");
	
	for (int i=0; i<numPlayers;i++) {
		
		player p;
		p.x = xml.getAttribute("player", "x", 0, i) ;
		p.y = xml.getAttribute("player", "y", 0, i) ;
		p.scale = xml.getAttribute("player", "scale", 1.0f, i) ;
		p.degree = xml.getAttribute("player", "degree", 0.0f, i) ;
		p.song.setup(video->bufferSize, video->sampleRate);
		p.song.loadTrack(ofToDataPath(xml.getAttribute("player", "song", "", i)));
		p.video = new ofxiVideoPlayer;
		p.video->setup(video);
		
		players.push_back(p);
		
		
		
	}
	xml.popTag();
	
	
	buffer	= new float[video->bufferSize];
	
	
	camera = new ofxiVideoGrabber;
	camera->setup(video);
	
	
	
	songState = SONG_IDLE;
	bRecording = false;
	trigger.setThresh(0.2);
	
	ofSoundStreamSetup(2, 1, this, video->sampleRate, video->bufferSize, 2);
	
	camera->startCamera();
	
	
	
}


//--------------------------------------------------------------
void testApp::update()
{
	
	camera->update();
	
	for (vector<player>::iterator iter=players.begin(); iter!=players.end(); iter++) { 
		iter->video->update();
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
		ofRotateZ(iter->degree);
		//ofTranslate(i % 2 * ofGetWidth()/2, (int)(i / 2) * ofGetHeight() /2);
		//ofScale(0.5, 0.5, 1);
		if (iter->video->getIsPlaying()) {
			iter->video->draw();
		} else {
			camera->draw();
		}
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
	//camera->setTrigger(thresh);
	//camera->startRecording();
	//cout << "Start recording" << endl;
}

void testApp::preview() {
	players[0].video->play(1.0f);
}


void testApp::play() {
	songState = SONG_PLAY;
	for (vector<player>::iterator iter=players.begin(); iter!=players.end(); iter++)  {
		iter->song.play();
	}
}

void testApp::stop() {
	songState = SONG_IDLE;
	for (vector<player>::iterator iter=players.begin(); iter!=players.end(); iter++)  {
		iter->song.stop();
	}
}


//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	
	if (touch.x > ofGetWidth()-30) {
		
		switch ((int)(3*touch.y/(ofGetHeight()+1))) {
			case 0:
				record();
				break;
			case 1:
				preview();
				break;

			case 2:
				play();
				break;
			default:
				break;
		}  

	} else if (touch.x < 30) {
		trigger.setThresh(1-(touch.y/ofGetHeight()));
	}


}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
		
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){	
	if (touch.x > ofGetWidth()-30) {
		
		switch ((int)(3*touch.y/(ofGetHeight()+1))) {
			case 0:
				
				break;
			case 1:
				
				break;
				
			case 2:
				stop();
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
	
	
	
	if (trigger.getTriggerState() == STATE_TRIGGER) {
		trigger.resetTrigger();
		camera->record();
		bRecording = true;
	}
	
	//compressor.audioProcess(input,bufferSize);
	
	camera->audioReceived(input, bufferSize);
	
	if (bRecording && camera->getState()!=CAMERA_RECORDING) {
		bRecording = false;
		gain = 0.6f/trigger.getRmsPeak();
		printf("rms: %1.2f, players: %i, gain: %1.2f\n", trigger.getRmsPeak(),players.size(),gain);
	}
}

void testApp::audioRequested( float * output, int bufferSize, int nChannels ) {
	memset(buffer, 0, bufferSize*sizeof(float));
	
	for (vector<player>::iterator iter=players.begin(); iter!=players.end(); iter++)  {
		
		vector<event> events;
		
		switch (songState) {
			case SONG_PLAY:
			case SONG_RENDER_AUDIO: {
				iter->song.process(events);
				
				for (vector<event>::iterator niter=events.begin(); niter!=events.end(); niter++) {
					float fr = exp((float)(niter->note-60)/12.0*log(2.0));
					//printf("note:  %i %1.2f\n", niter->note,fr);
					if (niter->bNoteOn) {
						
						iter->video->play(fr);
						
						//					midiInstrument->noteOn(note, iter->velocity*volume);
						//					
						//					if (songState!=SONG_RENDER_AUDIO) {
						//						currentPlayer->play(midiToSample[note]); // TODO: manage animations for multi player (drum)
						//					}
						
					}
				}
			} break;
				
				
				
				
			default:
				break;
		}
		
		iter->video->mix(buffer, bufferSize,1.0f/players.size());
	}
	
	
	for (int i = 0; i < bufferSize; i++){
		output[i*nChannels] = buffer[i] * gain;
		output[i*nChannels + 1] = buffer[i] * gain;
	}
	
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



