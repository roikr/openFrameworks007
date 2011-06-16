
#include "testApp.h"
#include "ofxXmlSettings.h"


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
	
	ofxXmlSettings xml;
	
	bool loaded = xml.loadFile("layout.xml");
	assert(loaded);
	
	xml.pushTag("layout");
	for (int i=0; i<xml.getNumTags("player");i++) {
		
		playerLayout p;
		p.x = xml.getAttribute("player", "x", 0, i) ;
		p.y = xml.getAttribute("player", "y", 0, i) ;
		p.scale = xml.getAttribute("player", "scale", 1.0f, i) ;
		p.degree = xml.getAttribute("player", "degree", 0, i) ;
		
		layouts.push_back(p);
		
	}
	xml.popTag();
	
	
	video.fps = 25;
	video.bufferSize	= 512;
	video.sampleRate 			= 44100;
	video.numFrames = 50;
	video.sampleLength = 1000*video.numFrames/video.fps;
	video.numBuffers = 44100 * video.sampleLength / (1000 * video.bufferSize);
	video.sample				= new float[video.numBuffers*video.bufferSize];
	memset(video.sample, 0, video.numBuffers*video.bufferSize * sizeof(float));

	buffer	= new float[video.bufferSize];
	
	
		
	camera.setup(&video);
	
	for (int i=0;i<4;i++) {
		player[i].setup(&video);
	}
	
	
	ofSoundStreamSetup(1, 1, this, video.sampleRate, video.bufferSize, 2);
	
	camera.startSession();
	
}


//--------------------------------------------------------------
void testApp::update()
{
	
	camera.update();
	
	for (int i=0;i<4;i++) {
		player[i].update();
	}
	
	
}

//--------------------------------------------------------------
void testApp::draw()
{
	
	
	
	int i=0;
	for (vector<playerLayout>::iterator iter=layouts.begin(); iter!=layouts.end(); iter++,i++) {

	
		ofPushMatrix();
		ofTranslate(iter->x, iter->y, 0);
		ofScale(iter->scale,iter->scale,1.0);
		ofRotateZ(iter->degree);
		//ofTranslate(i % 2 * ofGetWidth()/2, (int)(i / 2) * ofGetHeight() /2);
		//ofScale(0.5, 0.5, 1);
		if (player[i].getIsPlaying()) {
			player[i].draw();
		} else {
			camera.draw();
		}
		ofPopMatrix();
	
	}
	
	camera.drawAudio();
	
}

//--------------------------------------------------------------
void testApp::exit() {
	camera.exit();
}


//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	
	if (touch.x > ofGetWidth()-30 && touch.y < 30) {
		camera.startRecording();
		cout << "Start recording" << endl;
	} else {
		if (!camera.getIsRecording()) {
			int column = 2*touch.x / (ofGetWidth()+1);
			int row = 2*touch.y / (ofGetHeight()+1);
			int playerNum = 2*row + column;
			float note = (column ?  -0.5: 0.5 ) + 2*(touch.x / ofGetWidth()) ;
			player[playerNum].play( note );
		}
	}
	
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
		
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){	
	
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
}

void testApp::audioReceived( float * input, int bufferSize, int nChannels ) {
	camera.audioReceived(input, bufferSize);
}

void testApp::audioRequested( float * output, int bufferSize, int nChannels ) {
	memset(buffer, 0, bufferSize*sizeof(float));
	
	for (int i=0; i<4; i++) {
		player[i].mix(buffer, bufferSize);
	}
	
	for (int i = 0; i < bufferSize; i++){
		output[i ] = buffer[i];
		
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


