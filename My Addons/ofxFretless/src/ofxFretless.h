/*
 *  ofxFretless.h
 *  iPhoneNewerGrabExample
 *
 *  Created by Roee Kremer on 12/7/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <vector>

using namespace std;

enum  {
	SONG_IDLE,
	SONG_RECORD,
	SONG_PLAY
};

struct event {
	int absolute;
	float note;
};

class ofxFretless {
	
	
	
public:
	ofxFretless() {state = SONG_IDLE;};
	void record();
	void addNoteEvent(float note);
	void stop();
	void process(vector<event> &events);
	void play();
	int getSongState();
	
protected:
	
	vector<event> events;
	vector<event>::iterator iter;
	
	int startTime;
	int state;
	
};