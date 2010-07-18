#pragma once

#include "AudioDefs.h" // must define PI before of
#include "SndCoreAudio.h"
#include "ofMain.h"


#include "ofxiPhone.h"
#include "Ball.h"
#include "ofxThread.h"



class Mixer;
class Player;
class SndCoreAudio;

class testApp : public ofxiPhoneApp, public ofxThread {
	
public:
	void setup();
	void update();
	void draw();
	void exit();

	void touchDown(float x, float y, int touchId, ofxMultiTouchCustomData *data);
	void touchMoved(float x, float y, int touchId, ofxMultiTouchCustomData *data);
	void touchUp(float x, float y, int touchId, ofxMultiTouchCustomData *data);
	void touchDoubleTap(float x, float y, int touchId, ofxMultiTouchCustomData *data);
	
	void lostFocus();
	void gotFocus();
	void gotMemoryWarning();
	
	
	
	vector<Player*> players;
	
	
	int ticksPerBeat;
	int startTime;
	
	void threadedFunction();
	
	Mixer mix;
	SndCoreAudio output;
	SndThread thread;
};
