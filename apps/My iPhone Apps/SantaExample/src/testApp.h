#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"


#include "ofxMidiTrack.h"
#include "ofxAudioTrigger.h"
//#include "ofxAudioCompressor.h"

enum {
	SONG_IDLE,
	SONG_PLAY,
	SONG_RENDER_AUDIO,
	SONG_RENDER_AUDIO_FINISHED,
	SONG_CANCEL_RENDER_AUDIO,
	SONG_RENDER_VIDEO
};




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
};

class testApp : public ofxiPhoneApp {
	
public:
	void setup();
	void update();
	void draw();
	void exit();
	
	void record();
	void preview();
	void play();
	void stop();
	
	
	void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	
	void audioReceived( float * input, int bufferSize, int nChannels );
	void audioRequested( float * output, int bufferSize, int nChannels );
	
	void lostFocus();
	void gotFocus();
	void gotMemoryWarning();
	void deviceOrientationChanged(int newOrientation);
	
	
	
	ofxiPhoneVideo *video;
	ofxiVideoGrabber *camera;
	
	
	vector<player> players;
	
	int numPlayers;
	
	float *buffer;
	
	
	
	ofImage background;
	
	
	int songState;
	
	ofxAudioTrigger trigger;
	//ofxAudioCompressor compressor;
	
	
	float gain;
	bool bRecording;

};

