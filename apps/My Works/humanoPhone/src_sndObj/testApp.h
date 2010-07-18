#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxAlphaVideoPlayer.h"
#include "ofxOsc.h"
#include "AudioDefs.h"
#include "ofxThread.h"
#include "ofxMidi.h" 

struct note {
	int midi;
	ofxAlphaVideoPlayer noteOn;
	ofImage idle;
	
	int x;
	int y;
	float scale;
	
	bool bNoteOn;
	bool bIdle;
	bool bAudio;
	int group;
	
	bool bPlaying;
	bool bAudioPlaying;
	
	bool bStartVideo;
	bool bStopVideo;
	
	int start;
	
	int key;
	
	SndAiff *audio;
	SndIn input;
	Gain gain;
	
};

class control {
public:
	control() {};
	
	void setup();
	int getState();
	void setState(int state);
	void draw();
	bool didChange();
	void resetChanged();
	
protected:
	vector<ofImage*> images;
	int state;
	int x;
	int y;
	bool bChanged;
};
	

class toggleButton : public control {
public:
	toggleButton() {};
	void setup(string name,int statesNum,int x,int y);
	void mousePressed(int x, int y, int button);
	
private:
	int statesNum;
	
};

class switchButton : public control {
public:
	switchButton() {};
	void setup(string name,int x,int y);
	void mousePressed(int x, int y, int button);
};

class testApp : public ofBaseApp, public ofxThread{

	public:
		void setup();
		void loadFromXml();
		void saveToXml();
		void refresh();
		void update();
		void draw();
	
		void noteEvent(int note,int velocity,bool bNoteOn);
	
	
		void exit();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
	
	void threadedFunction();
			
	ofImage background;
	map<int,note*> notes;
	map<int,note*>::iterator noteIter;
	
	map<int,note*> keys;
	map<int,note*> beatBoxKeys;
	
	ofxOscReceiver	receiver;
	
	int downX;
	int downY;
	
	int x0;
	int y0;
	float s0;
	
	
	bool bScaling;
	
	
	float scale;
	
	
	Mixer mix;
	SndCoreAudio output;
	SndThread thread;
	
	ofxMidi midi;
	
	int startTime;
	
	int millisPerTick;
	
	toggleButton state;
	switchButton demo;
	
	bool bEditMode;
};

#endif
