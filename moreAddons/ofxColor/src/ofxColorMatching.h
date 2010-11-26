#pragma once

#include "ofxColorShared.h"

#define absdiff(x, y) (x > y ? x - y : y - x)

namespace ofxColor {

template<typename A, typename B>
inline float distanceRgbSquared(const A a[3], const B b[3]) {
	float x = a[0] - b[0];
	float y = a[1] - b[1];
	float z = a[2] - b[2];
	return x * x + y * y + z * z;
}

template<typename A, typename B>
float distanceRgb(const A a[3], const B b[3]) {
	return sqrtf(distanceRgbSquared(a, b));
}

inline float distanceHue(const float a[3], const float b[3]) {
	float x, y;
	if(a[0] > b[0]) {
		x = a[0] - b[0];
		y = (1 - a[0]) + b[0];
		return x < y ? x : y;
	} else {
		x = b[0] - a[0];
		y = (1 - b[0]) + a[0];
		return x < y ? x : y;
	}
}

/*
 This is actually a bit different from the above, as the distance between
 255 and 0 is 1 for unsigned char, while the distance between 1 and 0 for
 float is approximately 0.
*/
inline float distanceHue(const unsigned char a[3], const unsigned char b[3]) {
	float x, y;
	if(a[0] > b[0]) {
		x = a[0] - b[0];
		y = (256 - a[0]) + b[0];
		return x < y ? x : y;
	} else {
		x = b[0] - a[0];
		y = (256 - b[0]) + a[0];
		return x < y ? x : y;
	}
}

template<typename T>
inline float distanceHsvSquared(const T a[3], const T b[3]) {
	float x = distanceHue(a, b);
	float y = a[1] - b[1];
	float z = a[2] - b[2];
	return x * x + y * y + z * z;
}

template<typename T>
float distanceHsv(const T a[3], const T b[3]) {
	return sqrtf(distanceHsvSquared(a, b));
}

template<typename C, typename R>
bool matchRgb(const C color[3], const R reference[3], const float threshold[3]) {
	return
		absdiff(color[0], reference[0]) < threshold[0] &&
		absdiff(color[1], reference[1]) < threshold[1] &&
		absdiff(color[2], reference[2]) < threshold[2];
}

template<typename C, typename R>
bool matchHsv(const C color[3], const R reference[3], const float threshold[3]) {
	return
		distanceHue(color[0], reference[0]) < threshold[0] &&
		absdiff(color[1], reference[1]) < threshold[1] &&
		absdiff(color[2], reference[2]) < threshold[2];
}

}
