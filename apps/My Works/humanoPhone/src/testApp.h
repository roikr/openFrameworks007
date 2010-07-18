#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxAlphaVideoPlayer.h"
#include "ofxOsc.h"
//#include "AudioDefs.h"

struct note {
	int midi;
	ofxAlphaVideoPlayer noteOn;
	ofxAlphaVideoPlayer idle;
	ofSoundPlayer audio;
	int x;
	int y;
	float scale;
	
	bool bNoteOn;
	bool bIdle;
	bool bAudio;
	int group;
	
	bool bPlaying;
	bool bAudioPlaying;
	
	int start;
	
	int key;
	
	//SndAiff *audio;
	//SndIn	input;
	
};

class testApp : public ofBaseApp{

	public:
		void setup();
		void loadFromXml();
		void saveToXml();
		void refresh();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
	
	ofImage background;
	map<int,note*> notes;
	map<int,note*>::iterator noteIter;
	
	map<int,note*> keys;
	
	ofxOscReceiver	receiver;
	
	int downX;
	int downY;
	
	int x0;
	int y0;
	float s0;
	
	
	bool bScaling;
	bool bBackground;
	
	float scale;
	
	bool bPlayingKeyboard;
	
	//Mixer mix;
	//SndCoreAudio output;
	
};

#endif
