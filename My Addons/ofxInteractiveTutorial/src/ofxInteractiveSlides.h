/*
 *  ofxInteractiveSlides.h
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
	SLIDE_IDLE,
	SLIDE_PRE_DELAY,
	SLIDE_READY,
	SLIDE_DONE
};

struct slide {
	int tag;
	int predelay;
	int delay;
	string text;
	bool done; // for extra
};

class ofxInteractiveSlides {
public:
	ofxInteractiveSlides() : state(SLIDE_IDLE),bNeedRefresh(false) {};	
	
	void loadFile(string filename);
	void reset();
	void start(int slideNum);
	void skip();
	void done(int slideNum);
	void update();
	
	int	 getCurrentSlideNumber();
	bool getIsDone(int slideNum);
	int	 getCurrentSlideTag();
	int getTimesCompleted();
	int getState();
	
	bool getIsNeedRefresh();
	void setRefreshed();
	
private:
	void eval();
	
	void setState(int state);

	vector<slide> slides;
	vector<slide>::iterator citer;
	
	int preTimerStart;	
	int timerStart;
	int state;
	int timesCompleted;
	string filename;
	
	bool bNeedRefresh;
	
	
};

