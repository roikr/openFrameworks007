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
};

struct slide {
	int tag;
	int delay;
	string text;
};

class ofxInteractiveTutorial {
public:
	ofxInteractiveTutorial() : state(TUTORIAL_IDLE) {};	
	
	void addSlide(string& text,int delay,int tag);
	void loadFile(string filename);
	void start();
	void skip();
	
	void update();
	
	string getCurrentSlideText();
	int	 getCurrentSlideNumber();
	int	 getCurrentSlideTag();
	
	
	int getTimesCompleted();

	int getState();
	
private:
	
	void next();
	
	void setState(int state);

	vector<slide> slides;
	vector<slide>::iterator citer;
	
	int timerStart;
	int state;
	int timesCompleted;
	string filename;
	
	
	
};

