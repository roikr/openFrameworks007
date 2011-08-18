#pragma once

#include "ofMain.h"
//#include "ofMainExt.h"


#include "ofxiVideoGrabber.h"
#include "ofxAudioSampler.h"
#include "ofxMidiTrack.h"
#include "ofxAudioTrigger.h"
#include "ofxAudioFile.h"
#include "ofxRKTexture.h"
#include "ofxSlider.h"
//#include "ofxDynamicCompressor.h"

#include "ofxiPhoneVideo.h"
#include "ofxAudioSample.h"


class ofxiVideoPlayer;
class ofxAudioPlayer;


struct player {
	ofxiVideoPlayer *video;
	ofxAudioPlayer *audio;
	ofxMidiTrack song;
	bool bDidStartPlaying;
	
};

struct actor {
	int x;
	int y;
	float scale;
	float degree;
	float pan;
};

struct card {
	vector<actor> actors;
	ofxRKTexture *background;
};



class testApp:public ofSimpleApp  { // : public   ofxiPhoneApp
	
public:
	void setup();
	void update();
	
	
	void draw();
	void exit();
	
	void record();
	void preview();
	void playIntro(); 
	
	bool getIsPlaying();
	
	
	int getSongState();
	void setSongState(int songState);
	
	void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	
	void touchDown(float x, float y, int touchId);
	void touchMoved(float x, float y, int touchId);
	void touchUp(float x, float y, int touchId);
	void touchDoubleTap(float x, float y, int touchId);

	void audioReceived( float * input, int bufferSize, int nChannels );
	
	void audioProcess(float *buffer,int bufferSize, int nChannels);
	void audioRequested( float * output, int bufferSize, int nChannels );
	void renderAudio() ;
	
	void lostFocus();
	void gotFocus();
	void gotMemoryWarning();
	void deviceOrientationChanged(int newOrientation);
	
	void seekFrame(int frame); // for video rendering
	int getSongVersion();
	void soundStreamStart();
	void soundStreamStop();
	float getRenderProgress();
	
	bool cameraToggle();
	
	bool bNeedDisplay;
	
	ofxiPhoneVideo video;
	ofxiVideoGrabber grabber;
	ofxAudioSampler sampler;
	
	vector<player> players;
	vector<card> cards;
	vector<card>::iterator citer;
	
	ofxAudioSample sample;
	float sampleRate;
//	float *buffer; // for save song 
//	int bufferSize;
//	int nChannels;
	
	
	

	int songState;
	
	ofxAudioTrigger trigger;
	//ofxDynamicCompressor compressor;
		
	//float gain;
	bool bRecording;
	
	ofxAudioFile song; // just for saving
	float duration;
	int currentBlock;    //using to seekFrame and renderAudio for rendering video & audio;
	int totalBlocks; // calculating by renderAudio before rendering video - 
	// because we don't use midi instrument while video rendering, we need to know when the last sample occured...
	int songVersion;
	
	ofxSlider slider;
	
};

