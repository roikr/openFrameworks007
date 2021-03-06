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
	TUTORIAL_PRE_DELAY,
	TUTORIAL_READY,
	TUTORIAL_TIMER_STARTED,
};

struct tutorialSlide {
	int tag;
	int predelay;
	int delay;
	string text;
};

class ofxInteractiveTutorial {
public:
	ofxInteractiveTutorial() : state(TUTORIAL_IDLE),bNeedRefresh(false) {};	
	
	void loadFile(string filename);
	void start();
	void skip();
	
	void update();
	
	int	 getCurrentSlideTag();
	int	 getCurrentSlideNumber();
	
	int getTimesCompleted();
	int getState();
	
	bool getIsNeedRefresh();
	void setRefreshed();
	
private:
	
	string getCurrentSlideText();
	
	void setState(int state);

	vector<tutorialSlide> slides;
	vector<tutorialSlide>::iterator citer;
	
	int timerStart;
	int state;
	int timesCompleted;
	string filename;
	
	bool bNeedRefresh;
	
};

