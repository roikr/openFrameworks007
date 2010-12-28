#include "ofxiVideoGrabber.h"
#include "ofxiVideoPlayer.h"
#include "testApp.h"
#include "ofxColor.h"

//--------------------------------------------------------------
void testApp::setup(){	
	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	
	
	ofSetFrameRate(60);
	
	
	
	int bufferSize = 512;
	
	video.fps = 25;
	video.numIntroFrames = 4;
	video.sampleRate 			= 44100;
	video.numFrames = 25;
	video.sampleLength = 1000*video.numFrames/video.fps;
	
	video.audio.setup(bufferSize,video.sampleRate * video.sampleLength / (1000 * bufferSize));	
	
	
	
	
	 
	camera = new ofxiVideoGrabber();
	camera->setup(&video);
	
	
	player = new ofxiVideoPlayer();
	player->setup(&video);
	
	
	ofSoundStreamSetup(1, 1, this, video.sampleRate, video.audio.getBufferSize(), 2);
	
	camera->startCamera();
	
}


//--------------------------------------------------------------
void testApp::update()
{
	
	camera->update();
	player->update();
	
	if (fretless.getSongState() == SONG_PLAY) {
		vector<event> events;
		fretless.process(events);
		for (vector<event>::iterator iter=events.begin(); iter!=events.end(); iter++) {
			player->play(iter->note);
		}
	}
}

//--------------------------------------------------------------
void testApp::draw()
{
	
	
	
	ofSetColor(255, 255, 255);
	if (player->getIsPlaying()) {
		player->draw();
	} else if (camera->getState()==CAMERA_CAPTURING) {
		camera->draw();
		//camera->drawAudio();
	}
	
	ofFill();
	int width = ofGetWidth()/4;
	for (int i=0; i<4; i++) {
		
		
		ofxColorf color;
		color.setMode(OF_COLOR_HSV).set((float)i/4,1,0.9).setRange(255);
		ofSetColor(color.getRed(), color.getGreen(), color.getBlue());
		ofRect(i*width, 0, width, 30);
	}
}

//--------------------------------------------------------------
void testApp::exit() {
	camera->exit();
}


//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	
	if(touch.id == 0){
		switch ((int)((2*touch.y) / (ofGetHeight()+1))) {
			case 0: {
				switch ((int)((4*touch.x) / (ofGetWidth()+1))) {
					case 0:
						if (camera->getState()==CAMERA_RUNNING) {
							camera->startCapture();
						}
						camera->record();
						break;
					case 1:
						fretless.record();
						break;
					case 2:
						fretless.stop();
						break;
					case 3:
						fretless.play();
						break;
					default:
						break;
				}
						
			} break;
			case 1:
				if (!camera->getState()!=CAMERA_CAPTURING) {
					float note = 0.5 + touch.x / (ofGetWidth());
					player->play( note,1.0 );
					
					if (fretless.getSongState() == SONG_RECORD) {
						fretless.addNoteEvent(note);
					}
				}
				break;
				
//			case 2:
//				if (!camera->getIsRecording()) {
//					player->startScrubbing();
//					player->setPosition(touch.x / (ofGetWidth()+1));
//				}
//				break;
				
			default:
				break;
		}
	
	}
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
	if(touch.id == 0){
		switch ((int)((2*touch.y) / (ofGetHeight()+1))) {
			case 0:
				
				break;
			case 1:
				
				break;
				
//			case 2:
//				if (player->getIsScrubbing()) {
//					player->setPosition(touch.x / (ofGetWidth()+1));
//				}
//				break;
				
			default:
				break;
		}
		
	}		
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){	
	if(touch.id == 0){
		switch ((int)((2*touch.y) / (ofGetHeight()+1))) {
			case 0:
				
				break;
			case 1:
				
				break;
				
//			case 2:
//				if (player->getIsScrubbing()) {
//					player->stopScrubbing();
//				}
//				break;
				
			default:
				break;
		}
		
	}	
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
}

void testApp::audioReceived( float * input, int bufferSize, int nChannels ) {
	camera->audioReceived(input, bufferSize);
}

void testApp::audioRequested( float * output, int bufferSize, int nChannels ) {
	player->audioRequested(output, bufferSize);
	player->preProcess();
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


