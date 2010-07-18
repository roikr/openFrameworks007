/*
 *  ofxFramesDriver.h
 *  ofxFramesDriver
 *
 *  Created by Roee Kremer on 3/17/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _OFX_FRAMES_DRIVER
#define _OFX_FRAMES_DRIVER

#include <string>
#include <vector>
#include <map>

class ofxXmlSettings;

using namespace std;

struct videoData {
	string name;
	vector<int> frames;
};

struct sampleData {
	vector<int> lipsync;
	vector<videoData> videos;
};


class ofxFramesDriver {
public:
	
	void loadVectors(string filename);
	void loadVectors(ofxXmlSettings *xml);
	bool doesSampleExist(string sampleName);
	bool doesSampleExist(int sampleNum);
	void setCurrentSample(string sampleName);
	void setCurrentSample(int sampleNum);
	int getNumVideos();
	void setCurrentVideo(int videoNum);
	string getVideoName();
	
	
	int getNumVideoFrames();
	int getNumLipsFrames();
	int getVideoFrame(int frameNum); 
	int getLipsFrame(int frameNum);
	
	void updatePoint(int pointNum,int x,int y);
	void saveVectors(string filename);
	
	
private:
	vector<sampleData> samples;
	vector<sampleData>::iterator currentSample;
	vector<videoData>::iterator currentVideo;
	map<string,int> sampleByName;
	//map<int,int> sampleByNum;
};

#endif
