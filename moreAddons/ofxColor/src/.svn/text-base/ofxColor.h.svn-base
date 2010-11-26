#pragma once

#include "ofxColorBase.h"

typedef ofxColor::ofxColorBase<float> ofxColorf;
typedef ofxColor::ofxColorBase<unsigned char> ofxColoruc;

/*
 ofxColor has two sides:

 1 The ofxColorBase class and its common types ofxColorf and ofxColoruc. This
  stores an HSV and RGB representation internally simultaneously, along with
  range information, making it easy to store, sort, compare and transform
  palettes of colors.

 2 A collection of templated functions for doing standard operations on
	RGB and HSV colors stored as 3-element arrays. These make it easy to operate
	on image data like unsigned char arrays without needing to create an object.
	These functions are meant for advanced users doing per-pixel processing.
*/

/*
ofxColorShared
	set

ofxColorMath
	add/subtract/multiply/divide[Self]
	average
	min/max

ofxColorMatching
	distanceRgb/Hsv[Squared]
	distanceHue
	matchRgb/Hsv

ofxColorConversion
	lerp
	map
	normalize
	unnormalize
	clampRgb/Hsv
	convertRgbToHsv/HsvToRgb
*/

/*
 All base functions are written as generally as possible. If they need to know
 about max/min ranges, they are specialized to their types (float or unsigned
 char). When ofxColor calls them, it normalizes io using floats.

 Not everything should be inline (e.g.: conversion). Once implemented, these
 need to be separated out.

 - seamless integration of hsv and rgb in one class
 - sorting of colors by a parameter
 - uses alpha, or no alpha
 - blending functions
 - ranges per channel
 - all useful operators overriden, including << and >>
 - use set() or assignment with hex and string representations
 - comparisons between different types, e.g.: distance(ofxColorf, ofxColoruc)
 - distanceHue, distanceHsv, and convert are type specific, can be used for fast
  per-pixel processing

 x add basic blending functions
 2 start wrapping things with ofxColorBase
  x add support for range setting
  b wrapping all above functions:
		x clamp
		x map
		x lerp
		x distance
		x match
		average
		min/max
		arithmetic
 3 add sorting to ofxColorbase
 4 add functions for generating color theoretic colors!
 5 factory method for generating random color
 6 constants for setting ranges to various standards (normalized, 8-bit, opencv)
*/
