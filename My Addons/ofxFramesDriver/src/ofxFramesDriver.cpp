/*
 *  ofxFramesDriver.cpp
 *  ofxFramesDriver
 *
 *  Created by Roee Kremer on 2/17/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofMain.h"
#include "ofxFramesDriver.h"
#include "ofxXmlSettings.h"
#include <vector>
#include <iostream>


void ofxFramesDriver::loadVectors(string filename) {
	ofxXmlSettings xml;
	bool loaded = xml.loadFile(filename);
	
	

	assert(loaded);
	loadVectors(&xml);
//	if (loaded) {
//		cout << "loaded" << endl;
//		
//	} else {
//		cout << "didn't loaded" << endl;
//	}

}


void ofxFramesDriver::loadVectors(ofxXmlSettings *xml) {
	xml->pushTag("samples");
	for (int i=0; i<xml->getNumTags("sample"); i++) {
		string name = xml->getAttribute("sample","name","",i);
		sampleByName[name] = i;
		
		//int num = xml->getAttribute("sample","number",0,i);
		//sampleByNum[num] = i;
		
		//cout << "ofxFramesDriver::loadVectors sample " << i << ": " << name  << endl; // << ", " << num
		xml->pushTag("sample",i);
		
		sampleData sd;
		
		xml->pushTag("lipsync");
		for (int j=0; j<xml->getNumTags("frame");j++) {
			sd.lipsync.push_back(xml->getAttribute("frame","use",0,j));
		}
		xml->popTag();
		
		for (int k=0; k<xml->getNumTags("video");k++) {
			videoData vd;
			vd.name = xml->getAttribute("video","name","",k);
			xml->pushTag("video",k);
			for (int j=0; j<xml->getNumTags("frame");j++) {
				vd.frames.push_back(xml->getAttribute("frame","use",0,j));
			}
			xml->popTag();
			sd.videos.push_back(vd);
		}
		
		xml->popTag(); // sample
		samples.push_back(sd);
	}
}

bool ofxFramesDriver::doesSampleExist(string sampleName) {
	return  sampleByName.find(sampleName) != sampleByName.end();
}

void ofxFramesDriver::setCurrentSample(string sampleName) {
	map<string,int>::iterator iter=sampleByName.find(sampleName);
	if (iter != sampleByName.end()) {
		currentSample = samples.begin()+iter->second;
	} else {
		sampleByName[sampleName] = samples.size();
		sampleData sd;
		samples.push_back(sd);
		currentSample = samples.end()-1;
	}
}

/*
bool ofxFramesDriver::doesSampleExist(int sampleNum) {
	return  sampleByNum.find(sampleNum) != sampleByNum.end();
}

void ofxFramesDriver::setCurrentSample(int sampleNum) {
	map<int,int>::iterator iter=sampleByNum.find(sampleNum);
	if (iter != sampleByNum.end()) {
		currentSample = samples.begin()+iter->second;
	} else {
		sampleByNum[sampleNum] = samples.size();
		sampleData sd;
		samples.push_back(sd);
		currentSample = samples.end()-1;
	}
}
 */

int ofxFramesDriver::getNumVideos() {
	return currentSample->videos.size();
}

void ofxFramesDriver::setCurrentVideo(int videoNum) {
	currentVideo = currentSample->videos.begin()+videoNum;
}


string ofxFramesDriver::getVideoName() {
	return currentVideo->name;
}


int ofxFramesDriver::getNumVideoFrames() {
	return currentVideo->frames.size();	
}

int ofxFramesDriver::getNumLipsFrames() {
	return currentSample->lipsync.size();
	
}

int ofxFramesDriver::getVideoFrame(int frameNum) {
	return  frameNum<currentVideo->frames.size() ? currentVideo->frames[frameNum] : 0;
}

int ofxFramesDriver::getLipsFrame(int frameNum) {
	return frameNum<currentSample->lipsync.size() ? currentSample->lipsync[frameNum] : 0;
}

		

		
		

/*
void dumptrack(track t) {
	cout << "track: " << t.name;
	for (vector<pair<int,int> >::iterator iter = t.points.begin(); iter!=t.points.end(); iter++) {
		cout << ", (" << iter->first << "," << iter->second << ")";
	}
	cout << endl;
}
*/

/*
 void ofxFramesDriver::updatePoint(int pointNum,int x,int y) {
	
	

	if (pointNum >= currentSample->points.size()) {
		int diff = pointNum-currentSample->points.size()+1;
		for (int i=0; i<diff; i++) {
			currentTrack->points.push_back(make_pair(x, y));
		}
		
	} else {
		
		currentTrack->points[pointNum] = make_pair(x, y);
	//dumptrack(*currentTrack);
	//dumptrack(tracks[0]);
	}
	
}
*/
/*
 void ofxFramesDriver::saveVectors(string filename) {
	
	
	ofxXmlSettings xml;
	bool loaded = xml.loadFile(filename);
	
	if (loaded) {
		xml.pushTag("samples");
		
		for (vector<sampleData>::iterator iter=samples.begin(); iter!=samples.end(); iter++) {
			for (int i=0; i<xml.getNumTags("sample"); i++) {
				if (xml.getAttribute("sample", "name","",  i) == sampleByName[iter->name) {
					xml.removeTag("sample", i);
					break;
				}
			}
		}
	} else {
		xml.addTag("samples");
		xml.pushTag("samples");
	}
	
	
	for (vector<sampleData>::iterator samplesIter=samples.begin(); samplesIter!=samples.end(); samplesIter++) {
		int which = xml.addTag("sample");
		xml.addAttribute("sample", "name", samplesIter->name, which);
		xml.pushTag("sample", which);
		
		xml.addTag("lipsync");
		xml.pushTag("lipsync");
		
		for (vector<int>::iterator iter=samplesIter->lipsync.begin() ; iter!=samplesIter->lipsync.end(); iter++) {
			which = xml.addTag("frame");
			xml.addAttribute("frame", "number", distance(samplesIter->lipsync.begin(),iter), which);
			xml.addAttribute("frame", "use", iter->first, which);
		}
		
		xml.popTag();
		
		for (vector<videoData>::iterator videosIter=samplesIter->videos.begin(); videosIter!=samplesIter->videos.end(); videosIter++) {
			which = xml.addTag("video");
			xml.addAttribute("video","name",videoIter->name,which);
			xml.pushTag("video",which);
				
			for (vector<int>::iterator iter=videoIter->frames.begin() ; iter!=videoIter->frames.end(); iter++) {
				which = xml.addTag("frame");
				xml.addAttribute("frame", "number", distance(videoIter->frames.begin(),iter), which);
				xml.addAttribute("frame", "use", iter->first, which);
			}
			
			xml.popTag();
		}
		
		
		xml.popTag();
	}
	
	xml.popTag(); // samples
	
	xml.saveFile(filename);
	
}

*/
