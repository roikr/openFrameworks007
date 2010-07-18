/*
 *  ofxRKUtils.mm
 *  ofxiPhone SndObj Example
 *
 *  Created by Roee Kremer on 2/25/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxRKUtils.h"

string ofToDocumentsPath(string path){
	
	/*
	 NSString *bundle_path_ns = [[NSBundle mainBundle] bundlePath];
	 string dataPathRoot = [bundle_path_ns UTF8String];
	 
	 dataPathRoot.append( "/Documents/" ); // ZACH
	 */
	
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
	string documentsPathRoot = [documentsDirectory UTF8String];
	path = documentsPathRoot+"/"+path;
	
	return path;
}
