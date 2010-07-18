/*
 *  ofxMocha2xml.h
 *  mocha2xml
 *
 *  Created by Roee Kremer on 2/17/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _OFXMOCHA
#define _OFXMOCHA

#include <string>
#include <vector>
using namespace std;
class ofxXmlSettings;

using namespace std;

struct frame {
	int num;
	float x;
	float y;
	float z;
	float scaleX;
	float scaleY;
	float scaleZ;
	float rotation;
};


class ofxMocha {
public:
	
	void loadFromText (string filename);
	//void loadFromXml (string filename,string prefix);
	//void saveToXML (string filename,string prefix);
	void setXml(string filename);
	void getFromXml (string prefix,ofxXmlSettings *xml = 0);
	void addToXml (ofxXmlSettings *xml);
	void getFrame (int num,frame &f);
	void frameTransform (int num);

protected:
	vector<frame> frames;
	string prefix;
	ofxXmlSettings* xml;
};

#endif
