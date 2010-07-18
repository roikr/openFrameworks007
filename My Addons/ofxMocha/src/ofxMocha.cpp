/*
 *  ofxMocha.cpp
 *  mocha2xml
 *
 *  Created by Roee Kremer on 2/17/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofMain.h"
#include "ofxMocha.h"
#include "ofxXmlSettings.h"
#include <vector>
#include <iostream>
//#include <OpenGL/OpenGL.h>
#import <OpenGLES/ES1/gl.h>

void clearframe(frame &f) {
	f.num = 0;
	f.x = 0;
	f.y = 0;
	f.z = 0;
	f.scaleX = 0;
	f.scaleY = 0;
	f.scaleZ = 0;
	f.rotation = 0;
}

void dumpframe(frame *f) {
	cout << "frame: " << f->num << ", x: " << f->x << ", y: " << f->y << ", z: " << f->z << ", sx: " << f->scaleX << 
	", sy: " << f->scaleY << ", sz: " << f->scaleZ << ", rot: " << f->rotation << endl;
}


void ofxMocha::loadFromText (string filename) {
	
	frames.clear();
	
	
	FILE * pFile;
	pFile = fopen (filename.c_str() , "r");
	if (pFile == NULL) 
		return;
	
	
	vector<frame>::iterator iter;
	int num;
	bool res;
	char line [100];
	char str [100];
	
	
	while (!feof(pFile)) {
		fgets (line , 100 , pFile);
		cout << line ;
		
		if (sscanf(line, "%s",str) != 1) {
			continue;
		}
		
		
		
		if (strcmp(str,"Position") == 0) {
			
			fgets (line , 100 , pFile);
			cout << line ;
			fgets (line , 100 , pFile);
			frame f;
			clearframe(f);
			
			while (sscanf(line, "%u %f %f %f",&f.num,&f.x,&f.y,&f.z) == 4) {
				frames.push_back(f);
				dumpframe(&f);
				clearframe(f);
				fgets (line , 100 , pFile);
			} 
		} else if (strcmp(str,"Scale") == 0) {
			
			fgets (line , 100 , pFile);
			cout << line ;
			fgets (line , 100 , pFile);
			iter = frames.begin();
			
			while (sscanf(line, "%u %f %f %f",&num,&(iter->scaleX),&(iter->scaleY),&(iter->scaleZ)) == 4) {
				dumpframe(&*iter);
				iter++;
				fgets (line , 100 , pFile);
			} 
		} else if (strcmp(str,"Rotation") == 0) {
			
			
			fgets (line , 100 , pFile);
			cout << line ;
			fgets (line , 100 , pFile);
			iter = frames.begin();
			
			while (sscanf(line, "%u %f",&num,&iter->rotation) == 2) {
				dumpframe(&*iter);
				iter++;
				fgets (line , 100 , pFile);
			} 
			
		} 
	}
	
	fclose (pFile);
	
	vector<string> vec1 = ofSplitString(filename, "/");
	vector<string> vec2 = ofSplitString(vec1.back(), ".");
	prefix = vec2.front();
}

void ofxMocha::setXml(string filename) {
	xml = new ofxXmlSettings();
	bool loaded = xml->loadFile(filename);
	assert(loaded);
	xml->pushTag("trackers");
	
}

/*
void ofxMocha::loadFromXml (string filename,string tracker) {
}

void ofxMocha::saveToXML (string filename,string prefix) {
}
 */

void ofxMocha::getFromXml (string prefix,ofxXmlSettings *xml) {
	
	if (xml==0) {
		assert(this->xml);
		xml = this->xml;
		
	}
	
	int which=0;
	while( which<xml->getNumTags("sequence") && xml->getAttribute("sequence", "prefix","",  which) != prefix)
		which++;
	
	this->prefix = xml->getAttribute("sequence", "prefix", "", which);
	
	frames.clear();
	xml->pushTag("sequence", which);
	
	for (int i= 0; i <xml->getNumTags("frame") ; i++) {
		frame f;
		
		f.num = xml->getAttribute("frame", "number", 0.0 , i);
		f.x = xml->getAttribute("frame", "x", 0.0 , i);
		f.y = xml->getAttribute("frame", "y", 0.0 , i);
		f.z = xml->getAttribute("frame", "z", 0.0 , i);
		f.scaleX = xml->getAttribute("frame", "scaleX", 0.0 , i);
		f.scaleY = xml->getAttribute("frame", "scaleY", 0.0 , i);
		f.scaleZ = xml->getAttribute("frame", "scaleZ", 0.0 , i);
		f.rotation = xml->getAttribute("frame", "rotation", 0.0 , i);
			
		frames.push_back(f);
	}
	
	xml->popTag();
}

void ofxMocha::addToXml (ofxXmlSettings *xml) {
	
	for (int i=0; i<xml->getNumTags("sequence"); i++) {
		if (xml->getAttribute("sequence", "prefix","",  i) == prefix) {
			xml->removeTag("sequence", i);
			break;
		}
	}
	
	int which = xml->addTag("sequence");
	
	xml->addAttribute("sequence", "prefix", prefix, which);
	xml->pushTag("sequence", which);

	for (vector<frame>::iterator iter=frames.begin() ; iter!=frames.end(); iter++) {
		which = xml->addTag("frame");
		xml->addAttribute("frame", "number", iter->num, which);
		xml->addAttribute("frame", "x", iter->x, which);
		xml->addAttribute("frame", "y", iter->y, which);
		xml->addAttribute("frame", "z", iter->z, which);
		xml->addAttribute("frame", "scaleX", iter->scaleX, which);
		xml->addAttribute("frame", "scaleY", iter->scaleY, which);
		xml->addAttribute("frame", "scaleZ", iter->scaleZ, which);
		xml->addAttribute("frame", "rotation", iter->rotation, which);
	}
	
	xml->popTag();
}

void ofxMocha::frameTransform (int num) {
	if (num < frames.size()) {
		frame currentFrame  = frames.at(num);
		glTranslatef(currentFrame.x,currentFrame.y,0);
		glRotatef(currentFrame.rotation , 0,0,1);
		glScalef(currentFrame.scaleX/100.0, currentFrame.scaleY/100.0, 1.0);
	}
}

void ofxMocha::getFrame (int num,frame &f) {
	if (num < frames.size()) {
		f = frames.at(num);
	}
}