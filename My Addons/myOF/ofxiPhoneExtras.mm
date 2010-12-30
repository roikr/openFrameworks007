/*
 *  ofxiPhoneExtras.mm
 *  VideoToTextures
 *
 *  Created by Roee Kremer on 12/29/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxiPhoneExtras.h"
#include "ofMain.h"

int orientation = OFXIPHONE_ORIENTATION_PORTRAIT;

void ofxiPhoneSetOrientation(UIDeviceOrientation newOrientation) {
	
	ofLog(OF_LOG_VERBOSE, "ofAppiPhoneWindow::setOrientation: " + ofToString(orientation));
	switch (orientation) {
		case OFXIPHONE_ORIENTATION_PORTRAIT:
			[[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationPortrait];
			break;
		case OFXIPHONE_ORIENTATION_UPSIDEDOWN:
			[[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationPortraitUpsideDown];
			break;
		case OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT:
			[[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationLandscapeRight];
			break;
		case OFXIPHONE_ORIENTATION_LANDSCAPE_LEFT:
			[[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationLandscapeLeft];			
			break;
			
		default:
			break;
	}
	
	orientation = newOrientation;
}

UIDeviceOrientation ofxiPhoneGetOrientation() {
	return (UIDeviceOrientation)orientation;
}