/*
 *  ofxButton.h
 *  Musicreed
 *
 *  Created by Roee Kremer on 7/4/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once


#include "ofMain.h"

class ofxButton {

	public:
	ofxButton(ofRectangle rect,ofColor color);
	void draw();
	void setDown(bool bDown);

	private:
	
	ofRectangle rect;
	ofColor color;

	bool bDown;
};