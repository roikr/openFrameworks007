/*
 *  ofxiVideoLoader.cpp
 *  VideoToTextures
 *
 *  Created by Roee Kremer on 12/23/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxiVideoLoader.h"
#import "VideoImporter.h"

void ofxiVideoLoad(string filename,ofxiPhoneVideo &video) {
	[[VideoImporter videoImporter] import:[NSURL fileURLWithPath:[NSString stringWithFormat:@"%s",filename.c_str()]] withVideo:&video];
}
