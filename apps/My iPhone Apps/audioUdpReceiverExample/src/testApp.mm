#include "testApp.h"

#define NUM_BUFFERS 100

//--------------------------------------------------------------
void testApp::setup(){
	//we run at 60 fps!
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
	

    //create the socket and bind to port 11999
	udpConnection.Create();
	udpConnection.Bind(11999);
	udpConnection.SetNonBlocking(true);

	//ofSetBackgroundAuto(false);
	ofBackground(255, 255, 255);
	
	bPlay = false;
	
	sampleRate 			= 44100;
	//for some reason on the iphone simulator 256 doesn't work - it comes in as 512!
	//so we do 512 - otherwise we crash
	initialBufferSize	= 512;
	
//	lAudio				= new float[initialBufferSize];
//	rAudio				= new float[initialBufferSize];
	buffer				= new float[initialBufferSize * NUM_BUFFERS];
	
//	memset(lAudio, 0, initialBufferSize * sizeof(float));
//	memset(rAudio, 0, initialBufferSize * sizeof(float));
	memset(buffer, 0, initialBufferSize * NUM_BUFFERS *sizeof(float));
	
	drawCounter			= 0;
	bufferCounter		= 0;
	buffersReceived = 0;
	
	ofSoundStreamSetup(2, 0, this, sampleRate, initialBufferSize, 4);
}

//--------------------------------------------------------------
void testApp::update(){

//	char udpMessage[100000];
//	int charRec = udpConnection.Receive(udpMessage,100000);
//	if (charRec > 0) {
//		string message=udpMessage;
//		if(message!=""){
//			stroke.clear();
//			float x,y;
//			vector<string> strPoints = ofSplitString(message,"[/p]");
//			for(int i=0;i<strPoints.size();i++){
//				vector<string> point = ofSplitString(strPoints[i],"|");
//				if( point.size() == 2 ){
//					x=atof(point[0].c_str());
//					y=atof(point[1].c_str());
//					stroke.push_back(ofPoint(x,y));
//				}
//			}
//		}
//		
//	}
	
	while (udpConnection.Receive((char *)buffer + initialBufferSize * buffersReceived * sizeof(float),initialBufferSize * sizeof(float))>0) {
		buffersReceived++;
		if (buffersReceived == NUM_BUFFERS) {
			buffersReceived = 0;
			bufferCounter = 0;
			bPlay = true;
		}
	} 
	
}

//--------------------------------------------------------------
void testApp::draw(){
    ofFill();
    ofSetHexColor(0xFFFFFF);
    ofRect(0,0,200,30);
	ofSetHexColor(0x101010);
	ofDrawBitmapString("UDP Receiver Example ", 10, 20);

	for(int i=1;i<stroke.size();i++){
		ofLine(stroke[i-1].x,stroke[i-1].y,stroke[i].x,stroke[i].y);
	}
	
	ofNoFill();
	ofSetHexColor(0x333333);
	drawCounter++;
	char reportString[255];
	sprintf(reportString, "buffers played: %i\ndraw routines called: %i\n", bufferCounter,drawCounter);
	ofDrawBitmapString(reportString, 70,308);
	
	sprintf(reportString, "buffers received: %i\n", buffersReceived);
	ofDrawBitmapString(reportString, 70,348);
}


//--------------------------------------------------------------
void testApp::exit(){
	
}

//--------------------------------------------------------------
void testApp::audioOut( float * output, int bufferSize, int nChannels ){
	
	memset(output, 0, bufferSize * nChannels *sizeof(float));
	
	if( initialBufferSize != bufferSize ){
		ofLog(OF_LOG_ERROR, "your buffer size was set to %i - but the stream needs a buffer size of %i", initialBufferSize, bufferSize);
		return;
	}	
	
	if (bPlay) {
		for (int i = 0; i < bufferSize; i++){
			
			output[i*nChannels    ] = buffer[i+bufferCounter*bufferSize];
			output[i*nChannels + 1] = buffer[i+bufferCounter*bufferSize];
		}
		
		bufferCounter++;
		
		if (bufferCounter == NUM_BUFFERS) {
			bPlay = false;
		}
	}
		
	
	
	
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	
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

