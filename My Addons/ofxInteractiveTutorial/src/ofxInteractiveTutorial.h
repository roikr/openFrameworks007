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
	TUTORIAL_READY,
	TUTORIAL_TIMER_STARTED,
	TUTORIAL_DONE
};


class ofxInteractiveTutorial {
public:
	ofxInteractiveTutorial() : state(TUTORIAL_IDLE) {};	
	
	void addMessage(string& str,int delay);
	void loadFile(string filename);
	void start();
	void skip();
	
	void update();
	
	string getCurrentText();
	int	 getCurrentNumber();
	
	
	int getTimesCompleted();

	int getState();
	
private:
	
	void next();
	
	void setState(int state);

	vector<pair<string,int> > messages;
	vector<pair<string,int> >::iterator citer;
	
	int timerStart;
	int state;
	int timesCompleted;
	string filename;
	
	
	
};

