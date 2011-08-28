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
#include "ofxAudioLimiter.h"

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
	int player;
	
};

struct card {
	vector<actor> actors;
	vector<player> players;
	ofxRKTexture *background;
	bool bDisableNoteOff;
};



class testApp:public ofSimpleApp  { // : public   ofxiPhoneApp
	
public:
	void setup();
	void update();
	
	void render();
	void draw();
	void exit();
	
	void live();
	void record();
	void preview();
//	void playIntro(); 
	
	bool getIsPlaying();
	
	int getState();
	
	int getSongState();
	void setSongState(int songState);
	
	void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	
	void audioReceived( float * input, int bufferSize, int nChannels );
	void audioRequested( float * output, int bufferSize, int nChannels );
	void renderAudio() ;
	void renderVideo() ;
	
	void lostFocus();
	void gotFocus();
	void gotMemoryWarning();
	void deviceOrientationChanged(int newOrientation);
	
	void seekFrame(int frame); // for video rendering
	int getSongVersion();
	void soundStreamStart();
	void soundStreamStop();
	float getRenderProgress();
	
	void preRender();
	void postRender();
	
	bool cameraToggle();
	
	bool bNeedDisplay;
	
	ofxiPhoneVideo video;
	ofxiVideoGrabber grabber;
	ofxAudioSampler sampler;
	
	
	vector<card> cards;
	vector<card>::iterator citer;
	vector<card>::iterator oiter; // old iter to keep playing when sliding
	
	ofxAudioSample sample;
	float sampleRate;
//	float *buffer; // for save song 
//	int bufferSize;
//	int nChannels;
	
	
	

	int songState;
	int state;
	
	ofxAudioTrigger trigger;
	//ofxDynamicCompressor compressor;
		
	//float gain;
	bool bTriggerRecord;
	
	
	ofxAudioFile song; // just for saving
	float duration;
	int currentBlock;    //using to seekFrame and renderAudio for rendering video & audio;
	int totalBlocks; // calculating by renderAudio before rendering video - 
	// because we don't use midi instrument while video rendering, we need to know when the last sample occured...
	int songVersion;
	
	ofxSlider slider;
	bool bSlide;
	bool bCardChanged;
	int delayStart;
	
	ofxAudioLimiter limiter;
	
	bool bCameraOffset;
	
};

