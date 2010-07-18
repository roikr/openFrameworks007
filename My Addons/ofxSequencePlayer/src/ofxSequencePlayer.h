/*
 *  ofxSequencePlayer.h
 *  atlasCreatorFromImages
 *
 *  Created by Roee Kremer on 3/14/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once


#include <vector>
#include <string>
using namespace std;
class ofImage;

class ofxSequencePlayer  {
	
public:
	
	
	ofxSequencePlayer ();
	virtual ~ofxSequencePlayer();
	
	bool 				loadMovie(string name,float fps = 25.0);
	//void 				closeMovie();
	//void 				close();
	
	//void				update();			//same as idleMovie
	void 				idleMovie();		// rename to updateMovie?
	void 				play();
	//void 				stop();
	
	
	
	bool 				isFrameNew();
	unsigned char * 	getPixels();
	float 				getPosition();
	//float 				getSpeed();
	float 				getDuration();
	bool				getIsMovieDone();
	
	void 				setPosition(float pct);
	//void 				setVolume(int volume);
	void 				setLoopState(int state);
	//void   				setSpeed(float speed);
	void				setFrame(int frame);  // frame 0 = first frame...
	
	//void 				setUseTexture(bool bUse);
	//ofTexture &			getTextureReference();
	void 				draw(float x, float y, float w, float h);
	void 				draw(float x, float y);
	
	
	
	//the anchor is the point the image is drawn around.
	//this can be useful if you want to rotate an image around a particular point.
	//void				setAnchorPercent(float xPct, float yPct);	//set the anchor as a percentage of the image width/height ( 0.0-1.0 range )
	void				setAnchorPoint(int x, int y);				//set the anchor point in pixels
	void				resetAnchor();								//resets the anchor to (0, 0)
	
	void 				setPaused(bool bPause);
	
	int					getCurrentFrame();
	int					getTotalNumFrames();
	
	void				firstFrame();
	void				nextFrame();
	void				previousFrame();
	
	int 				getHeight();
	int					getWidth();
	
private:
	vector<ofImage*> frames;
	string name;
	int nFrames;
	vector<ofImage*>::iterator currentFrame;
	int 				width, height;
	float  				speed;
	bool 				bLoaded;
	bool				bPlaying;
	float				fps;
	int					startTime;
	bool 				bIsFrameNew;
	int					lastFrame;
	int					startFrame;
};