#include "testApp.h"


#define RECONNECT_TIME 400
#define NUM_BUFFERS 100

//--------------------------------------------------------------
void testApp::setup(){
	// we don't want to be running to fast
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	//If you want a landscape oreintation 
	//iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
	ofBackground(127,127,127);
	

    //create the socket and set to send to 127.0.0.1:11999
	udpConnection.Create();
	udpConnection.Connect("10.0.0.8",11999);
	udpConnection.SetNonBlocking(true);

	initialBufferSize	= 512;
	sampleRate 			= 44100;
	drawCounter			= 0;
	bufferCounter		= 0;
	buffersSent = 0;
	
	buffer				= new float[initialBufferSize * NUM_BUFFERS];
	memset(buffer, 0, initialBufferSize * NUM_BUFFERS * sizeof(float));
	
	bRecord = false;
	
	ofSoundStreamSetup(0, 1, this, sampleRate, initialBufferSize, 4);
}

//--------------------------------------------------------------
void testApp::update(){
	while (buffersSent < bufferCounter) {
		int sent = udpConnection.Send((char*)buffer+buffersSent * initialBufferSize  * sizeof(float), initialBufferSize  * sizeof(float));
		buffersSent++;
	}
}

//--------------------------------------------------------------
void testApp::draw(){

	ofSetColor(20, 20, 20);
	ofDrawBitmapString("openFrameworks UDP Send Example ", 15, 30);
    ofDrawBitmapString("drag to draw", 15, 50);
	for(int i=1;i<stroke.size();i++){
		ofLine(stroke[i-1].x,stroke[i-1].y,stroke[i].x,stroke[i].y);
	}

	ofSetHexColor(0x333333);
	drawCounter++;
	char reportString[255];
	sprintf(reportString, "buffers received: %i\ndraw routines called: %i\n", bufferCounter,drawCounter);
	ofDrawBitmapString(reportString, 70,308);
	
	sprintf(reportString, "buffers sent: %i", buffersSent);
	ofDrawBitmapString(reportString, 70,348);
	
	sprintf(reportString, "sendBufferSize: %i\nmaxMsgSize: %i\n", udpConnection.GetSendBufferSize(),udpConnection.GetMaxMsgSize());
	ofDrawBitmapString(reportString, 70,368);
}

void testApp::exit(){
	
}

//--------------------------------------------------------------
void testApp::audioIn(float * input, int bufferSize, int nChannels){
	
	if( initialBufferSize != bufferSize ){
		ofLog(OF_LOG_ERROR, "your buffer size was set to %i - but the stream needs a buffer size of %i", initialBufferSize, bufferSize);
		return;
	}	
	
	if (bRecord) {
		for (int i = 0; i < bufferSize; i++){
			buffer[i+bufferCounter*bufferSize] = input[i];
		}
		bufferCounter++;
		
		if (bufferCounter == NUM_BUFFERS) {
			bRecord = false;
		}
		
	}
	// samples are "interleaved"
	
	
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	stroke.clear();
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
	stroke.push_back(ofPoint(touch.x,touch.y));
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
	string message="";
	for(int i=0; i<stroke.size(); i++){
		message+=ofToString(stroke[i].x)+"|"+ofToString(stroke[i].y)+"[/p]";
	}
//	int sent = udpConnection.Send(message.c_str(),message.length());
	
	if (!bRecord) {
		bufferCounter = 0;
		buffersSent = 0;
		bRecord = true;
	}
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
	
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


//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs& args){
	
}






