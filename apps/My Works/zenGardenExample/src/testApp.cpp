
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
	
	ofBackground(255,255,255);
	ofSetFrameRate(60);
	ofSoundStreamSetup(numOutputChannels,numInputChannels,this, sampleRate,blockSize, 4);
	ofSoundStreamStop();
	
	//const float sampleRate = 22050.0f;
		
	
	
	string patchstr = "drms-test.pd";
	char *patch = (char *) malloc((patchstr.size() + 1) * sizeof(char));
	strcpy(patch, patchstr.c_str());
	
	string pathstr = ofToDataPath("pd-patches/");
	char * path = (char *) malloc((pathstr.size() + 1) * sizeof(char));
	strcpy(path, pathstr.c_str());
	
	
	graph = zg_new_graph(path, patch,blockSize, numInputChannels, numOutputChannels, sampleRate);	
	if (graph == NULL) {
		printf("graph is NULL. Is the given path correct?");
		return ;
	}
	
	zg_register_callback(graph, callbackFunction, NULL);
	
	inputBuffers = (float *) calloc(numInputChannels * blockSize, sizeof(float));
	outputBuffers = (float *) calloc(numOutputChannels * blockSize, sizeof(float));
	
	//zg_process(graph, inputBuffers, outputBuffers);
	
	
	ofSoundStreamStart();
	
	bKey = false;
	
	
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

void testApp::keyPressed( int key ) {
	bKey = true;

	//if (bStarted) {
//		//zg_send_message(graph, "msg", "ff",5.0f,50.0f);
//			} else {
//	
//		bStarted = true;
//		
//	}
	
		//zg_send_midinote(graph, 0, 1, 50, 0);
}

void testApp::audioRequested 	(float * output, int bufferSize, int nChannels){
	if (bKey) {
		bKey = false;
		zg_send_message(graph, "msg", "ff",1.0f,50.0f);
	}
	
	zg_process(graph, inputBuffers, outputBuffers);
	//cout << bufferSize << " " << nChannels << endl;
		
	//zen garden is probably not interlaced but SoundStream is
	for (int i = 0; i < bufferSize; i++){
		
		output[i*nChannels] = outputBuffers[i];
		output[i*nChannels+1] = outputBuffers[i+bufferSize];
		
	}
	
}