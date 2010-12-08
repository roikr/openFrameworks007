#include "ofxiVideoGrabber.h"
#include "ofxiVideoPlayer.h"
#include "testApp.h"


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
	
	video.fps = 25;
	video.bufferSize	= 512;
	video.sampleRate 			= 44100;
	video.numFrames = 50;
	video.sampleLength = 1000*video.numFrames/video.fps;
	video.numBuffers = 44100 * video.sampleLength / (1000 * video.bufferSize);
	video.sample				= new float[video.numBuffers*video.bufferSize];
	memset(video.sample, 0, video.numBuffers*video.bufferSize * sizeof(float));

	
	 
	camera = new ofxiVideoGrabber();
	camera->setup(&video);
	
	
	player = new ofxiVideoPlayer();
	player->setup(&video);
	
	
	ofSoundStreamSetup(1, 1, this, video.sampleRate, video.bufferSize, 2);
	
	camera->startSession();
	
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
	if (player->getIsPlaying() || player->getIsScrubbing()) {
		player->draw();
	} else {
		camera->draw();
		camera->drawAudio();
	}
	
}

//--------------------------------------------------------------
void testApp::exit() {
	camera->exit();
}


//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	
	if(touch.id == 0){
		switch ((int)((3*touch.y) / (ofGetHeight()+1))) {
			case 0: {
				switch ((int)((4*touch.x) / (ofGetWidth()+1))) {
					case 0:
						camera->startRecording();
						cout << "Start recording" << endl;
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
				if (!camera->getIsRecording()) {
					float note = 0.5 + touch.x / (ofGetWidth());
					player->play( note );
					
					if (fretless.getSongState() == SONG_RECORD) {
						fretless.addNoteEvent(note);
					}
				}
				break;
				
			case 2:
				if (!camera->getIsRecording()) {
					player->startScrubbing();
					player->setPosition(touch.x / (ofGetWidth()+1));
				}
				break;
				
			default:
				break;
		}
	
	}
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
	if(touch.id == 0){
		switch ((int)((3*touch.y) / (ofGetHeight()+1))) {
			case 0:
				
				break;
			case 1:
				
				break;
				
			case 2:
				if (player->getIsScrubbing()) {
					player->setPosition(touch.x / (ofGetWidth()+1));
				}
				break;
				
			default:
				break;
		}
		
	}		
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){	
	if(touch.id == 0){
		switch ((int)((3*touch.y) / (ofGetHeight()+1))) {
			case 0:
				
				break;
			case 1:
				
				break;
				
			case 2:
				if (player->getIsScrubbing()) {
					player->stopScrubbing();
				}
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
	camera->audioReceived(input, bufferSize);
}

void testApp::audioRequested( float * output, int bufferSize, int nChannels ) {
	player->audioRequested(output, bufferSize);
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


