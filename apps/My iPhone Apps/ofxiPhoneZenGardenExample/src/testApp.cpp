
#include "testApp.h"

extern "C" {
	void callbackFunction(ZGCallbackFunction function, void *userData, void *ptr) {
		switch (function) {
			case ZG_PRINT_STD: {
				printf("%s\n", (char *) ptr);
				break;
			}
			case ZG_PRINT_ERR: {
				printf("ERROR: %s\n", (char *) ptr);
				break;
			}
			default: {
				break;
			}
		}
	}
};

//--------------------------------------------------------------
void testApp::setup(){	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	// touch events will be sent to testApp
	ofxMultiTouch.addListener(this);
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	ofBackground(255,255,255);
	
	bZGInitialized = false;
	
	
	
	
		//zg_process(graph, inputBuffers, outputBuffers);
	
	
	//ofSoundStreamSetup(2,2,this, sampleRate,blockSize, 4);
	
	inputBuffers = (float *) calloc(numInputChannels * blockSize, sizeof(float));
	outputBuffers = (float *) calloc(numOutputChannels * blockSize, sizeof(float));

	ofSoundStreamSetup(2,0,this, sampleRate,blockSize, 4);
	
	ofSetFrameRate(60);
	
}


//--------------------------------------------------------------
void testApp::update() {

}

//--------------------------------------------------------------
void testApp::draw() {

}

void testApp::exit() {
	zg_delete_graph(graph);
	free(inputBuffers);
	free(outputBuffers);
}


//--------------------------------------------------------------
void testApp::touchDown(float x, float y, int touchId, ofxMultiTouchCustomData *data){
	
	
}

//--------------------------------------------------------------
void testApp::touchMoved(float x, float y, int touchId, ofxMultiTouchCustomData *data){
}

//--------------------------------------------------------------
void testApp::touchUp(float x, float y, int touchId, ofxMultiTouchCustomData *data){
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(float x, float y, int touchId, ofxMultiTouchCustomData *data){
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



void testApp::audioRequested 	(float * output, int bufferSize, int nChannels){
	if (!bZGInitialized) {
		string patchstr = "drms-test.pd";
		char *patch = (char *) malloc((patchstr.size() + 1) * sizeof(char));
		strcpy(patch, patchstr.c_str());
		
		string pathstr = ofToDataPath("pd-roee/");
		char * path = (char *) malloc((pathstr.size() + 1) * sizeof(char));
		strcpy(path, pathstr.c_str());
		
				
		
		graph = zg_new_graph(path, patch,blockSize, numInputChannels, numOutputChannels, sampleRate);
		
		if (graph == NULL) {
			printf("graph is NULL. Is the given path correct?");
			return ;
		}
		
		free(path);
		free(patch);
		
		zg_register_callback(graph, callbackFunction, NULL);
		bZGInitialized = true;
		blockIndex = 0;
	}		
	
	if (blockIndex==100) {
		zg_send_message(graph, "msg", "ff",1.0f,120.0f);
	}
	
	zg_process(graph, inputBuffers, outputBuffers);
	blockIndex++;
		
	//zen garden is probably not interlaced but SoundStream is
	for (int i = 0; i < bufferSize; i++){
		
		output[i*nChannels] = outputBuffers[i];
		output[i*nChannels+1] = outputBuffers[i+bufferSize];
		
	}
	 
	 
	
}