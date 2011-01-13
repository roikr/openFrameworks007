/*
 *  ofxInteractiveTutorial.h
 *  interactiveTutorialExample
 *
 *  Created by Roee Kremer on 1/11/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include <string>
#include <vector>

using namespace std;

enum {
	TUTORIAL_IDLE,
	TUTORIAL_TIMER_STARTED,
	TUTORIAL_READY,
	TUTORIAL_DONE
};

class ofxInteractiveTutorial {
public:
	void setup();
	void addMessage(string& str);
	void loadFile(string filename);
	void start();
	void update();
	void done(int num);
	
	string getCurrentText();
	int	 getCurrentNumber();
	int getState();
	void setState(int state);
	
private:
	vector<string> messages;
	vector<string>::iterator citer;
	
	int timerStart;
	int delay;
	int state;
};

