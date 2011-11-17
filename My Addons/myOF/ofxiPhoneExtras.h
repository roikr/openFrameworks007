/*
 *  ofxiPhoneExtras.h
 *  VideoToTextures
 *
 *  Created by Roee Kremer on 12/29/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#import <UIKit/UIKit.h>

extern int orientation;
extern int wideDimension;
extern int narrowDimension;

// possible values for iPhoneSetOrientation or iPhoneGetOrientation
#define	OFXIPHONE_ORIENTATION_PORTRAIT			UIDeviceOrientationPortrait
#define OFXIPHONE_ORIENTATION_UPSIDEDOWN		UIDeviceOrientationPortraitUpsideDown
#define OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT	UIDeviceOrientationLandscapeRight
#define OFXIPHONE_ORIENTATION_LANDSCAPE_LEFT	UIDeviceOrientationLandscapeLeft


void ofxiPhoneSetOrientation(UIDeviceOrientation newOrientation);
UIDeviceOrientation ofxiPhoneGetOrientation();

void ofxiPhoneSetDimensions(int wide,int narrow);

