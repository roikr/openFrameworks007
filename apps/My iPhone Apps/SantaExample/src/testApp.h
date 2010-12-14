#pragma once

#include "ofMain.h"
//#include "ofMainExt.h"



#include "ofxMidiTrack.h"
#include "ofxAudioTrigger.h"
#include "ofxSndFile.h"
#include "ofxRKTexture.h"
//#include "ofxDynamicCompressor.h"





class ofxiVideoGrabber;
class ofxiVideoPlayer;
class ofxiPhoneVideo;

struct player {
	int x;
	int y;
	float scale;
	float degree;
	
	ofxiVideoPlayer *video;
	ofxMidiTrack song;
	
	float pan;
	
	bool bDidStartPlaying;
};

class testApp:public ofSimpleApp  { // : public   ofxiPhoneApp
	
public:
	void setup();
	void update();
	void draw();
	void exit();
	
	void record();
	void preview();
	
	bool getIsPlaying();
	
	int getSongState();
	void setSongState(int songState);
	
	void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);

	void audioReceived( float * input, int bufferSize, int nChannels );
	
	void audioProcess(int bufferSize);
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
	
	ofxiPhoneVideo *video;
	ofxiVideoGrabber *camera;
	
	vector<player> players;
	
	int numPlayers;
	
	//float *buffer;
	float 	* lAudio;
	float   * rAudio;
	
	ofxRKTexture background;

	int songState;
	
	ofxAudioTrigger trigger;
	//ofxDynamicCompressor compressor;
		
	//float gain;
	bool bRecording;
	
	ofxSndFile song; // just for saving
	float duration;
	int currentBlock;    //using to seekFrame and renderAudio for rendering video & audio;
	int totalBlocks; // calculating by renderAudio before rendering video - 
	// because we don't use midi instrument while video rendering, we need to know when the last sample occured...
	int songVersion;
	

};

