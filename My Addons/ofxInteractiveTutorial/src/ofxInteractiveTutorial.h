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
	TUTORIAL_TIMER_FINISHED,
	TUTORIAL_MESSAGE_PRESENTED,
	TUTORIAL_SCREEN_TOUCHED
};

class ofxInteractiveTutorial {
public:
	void setup(bool bKeepOrder);
	void addMessage(string& str);
	void loadFile(string filename);
	void start();
	void update();
	void messagePresented();
	void done(int num);
	
	string getCurrentText();
	int	 getCurrentNumber();
	int getState();
	void setState(int state);
	
private:
	vector<pair<string,bool> > messages;
	vector<pair<string,bool> >::iterator citer;
	
	int timerStart;
	int delay;
	int state;
	bool bKeepOrder;
};

