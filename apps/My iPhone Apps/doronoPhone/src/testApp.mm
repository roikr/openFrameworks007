#include "testApp.h"
#include "ofMainExt.h"



//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofxRegisterMultitouch(this);
		
	//ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);

	counter = 0;
	ofSetCircleResolution(50);
	ofBackground(0,0,0);
	bSmooth = false;
	
	
	//image.loadImage("esb2.jpg");
	texture.load(ofToResourcesPath("nyc.pvr"));
	//pincher.setup(ofPoint(0,-140),0.7,pincherPrefs(480,320,ofRectangle(0,0,700,700),0.5,1.5));
	//pincher.setup(ofPoint(0,0),0.3,pincherPrefs(480,320,ofRectangle(0,0,1628,1085),0.1,15));
	//pincher.setup(ofPoint(0,0),0.3,pincherPrefs(480,320,ofRectangle(0,0,2912,4368),0.1,15));
	//pincher.setup(ofPoint(0,0),0.3,pincherPrefs(320,480,ofRectangle(0,0,728,1092),0.1,10));
	pincher.setup(ofPoint(0,0),0.3,pincherPrefs(320,480,ofRectangle(0,0,1365,2048),0.234,10));
	
	//pincher.setup(ofPoint(0,0),0.5,pincherPrefs(480,320,ofRectangle(0,0,700,700),0.5,1));
	ofSetFrameRate(60);
	
	
	video.bufferSize = 512;
	video.fps = 25;
	video.numIntroFrames = 0;
	video.sampleRate 			= 44100;
	
	video.widthFraction = 1.0;
	video.heightFraction = 1.0;
	video.textureWidth = 256;
	video.textureHeight = 128;
	
	
	loader.setup(&video);
	loader.load(ofToDataPath("orenv.mov"));
	video.bHorizontal = false;
	player.setup(&video, false);
	
	lAudio	= new float[video.audio.getBufferSize()];
	rAudio	= new float[video.audio.getBufferSize()];
	
	song.setup(video.audio.getBufferSize(), video.sampleRate);
	song.loadTrack(ofToResourcesPath("wedding_march_01.mid"));
	
	ofSoundStreamSetup(2, 0, this, video.sampleRate, video.bufferSize, 2);
	
	bSongPlaying = false;
	
}

//--------------------------------------------------------------
void testApp::update(){
	//image.update();
	player.update();
	counter = counter + 0.033f;
	if (pincher.getIsAnimating()) {
		pincher.update((float)(ofGetElapsedTimeMillis() - animStart)/250.0);
	}
}

//--------------------------------------------------------------
void testApp::draw(){

	pincher.transform();
	
	//image.draw(0,0);
	texture.draw();
	
	ofPushMatrix();
	ofTranslate(940, 1462, 0);
	ofScale(0.1, 0.1, 1);
	player.draw();
	ofPopMatrix();
	
	
	//--------------------------- rectangles
//	ofFill();
//	for (int i = 0; i < 200; i++){
//		ofSetColor((int)ofRandom(0,255),(int)ofRandom(0,255),(int)ofRandom(0,255));
//		ofRect(ofRandom(250,350),ofRandom(250,350),ofRandom(10,20),ofRandom(10,20));
//	}
//	ofSetColor(0x000000);
//	ofDrawBitmapString("rectangles", 275,500);
//
//
//	pincher.draw();

}



void testApp::touchDown(int x, int y, int id) {
	
	pincher.touchDown(x, y, id);	
	
	ofPoint trans = pincher.touchToPoint(x, y);
	printf("x: %.0f, y: %.0f\n",trans.x,trans.y);

	
	player.play( 1.0f,1.0f );
	//player.play(60,127);
	bSmooth = true;
	

}


void testApp::touchMoved(int x, int y, int id) {
	
	pincher.touchMoved(x, y, id);
	
}

void testApp::touchUp(int x, int y, int id) {
	bSmooth = false;
	
		
	pincher.touchUp(x, y, id);
	
}

void testApp::touchDoubleTap(int x, int y, int id) {
//	if (!pincher.getIsAnimating()) {
//		animStart = ofGetElapsedTimeMillis();
//		pincher.touchDoubleTap(x,y,id);
//	}
	
	if (!bSongPlaying) {
		bSongPlaying = true;
		song.play();
	} else {
		if (song.getIsPlaying()) {
			song.stop();
			
		}
		bSongPlaying = false;
	}

	
}


void testApp::audioRequested( float * output, int bufferSize, int nChannels ) {
	memset(lAudio, 0, bufferSize*sizeof(float));
	memset(rAudio, 0, bufferSize*sizeof(float));
	
	float pan = 0.5;
	
	float leftScale = 1 - pan;
	float rightScale = pan;
	
	if (bSongPlaying) {
	
		vector<event> events;
		song.process(events);
	
		for (vector<event>::iterator niter=events.begin(); niter!=events.end(); niter++) {
			if (niter->bNoteOn) {
				player.play(niter->note,niter->velocity);
			}
		}
	}
	
	
	player.mix(lAudio, bufferSize,leftScale);
	player.mix(rAudio, bufferSize,rightScale);
	player.preProcess();
	
	for (int i = 0; i < bufferSize; i++){
		output[i*nChannels] = lAudio[i];// * gain;
		output[i*nChannels + 1] = rAudio[i];// * gain;
	}
	
}



