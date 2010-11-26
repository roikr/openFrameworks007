#pragma once

#include "ofxColorShared.h"
#include "ofxColorMath.h"

namespace ofxColor {

// amount = 0 means all a
// amount = 1 means all b
// not safe for a = result or b = result
template<typename A, typename B, typename R>
void lerp(const A a[3], const B b[3], float amount, R result[3]) {
	result[0] = (R) (amount * b[0]);
	result[1] = (R) (amount * b[1]);
	result[2] = (R) (amount * b[2]);
	amount = 1 - amount;
	result[0] += (R) (amount * a[0]);
	result[1] += (R) (amount * a[1]);
	result[2] += (R) (amount * a[2]);
}

inline float map(float source, float fromMin, float fromMax, float toMin, float toMax) {
	return
		((toMax - toMin) * (source - fromMin)) /
		(fromMax - fromMin) +
		toMin;
}

// map doesn't wrap hues :(
template<typename S, typename FMIN, typename FMAX, typename TMIN, typename TMAX, typename R>
void map(const S source[3], const FMIN fromMin[3], const FMAX fromMax[3], const TMIN toMin[3], const TMAX toMax[3], R result[3]) {
	result[0] = (R) map((float) source[0], (float) fromMin[0], (float) fromMax[0], (float) toMin[0], (float) toMax[0]);
	result[1] = (R) map((float) source[1], (float) fromMin[1], (float) fromMax[1], (float) toMin[1], (float) toMax[1]);
	result[2] = (R) map((float) source[2], (float) fromMin[2], (float) fromMax[2], (float) toMin[2], (float) toMax[2]);
}

inline float normalize(float x, float min, float max) {
	return (x - min) / (max - min);
}

template<typename S, typename R, typename MIN, typename MAX>
inline void normalize(const S source[3], R result[3], const MIN min[3], const MAX max[3]) {
	result[0] = (R) normalize((float) source[0], (float) min[0], (float) max[0]);
	result[1] = (R) normalize((float) source[1], (float) min[1], (float) max[1]);
	result[2] = (R) normalize((float) source[2], (float) min[2], (float) max[2]);
}

template<typename S, typename MIN, typename MAX>
inline void normalizeSelf(S source[3], const MIN min[3], const MAX max[3]) {
	source[0] = (S) normalize((float) source[0], (float) min[0], (float) max[0]);
	source[1] = (S) normalize((float) source[1], (float) min[1], (float) max[1]);
	source[2] = (S) normalize((float) source[2], (float) min[2], (float) max[2]);
}

inline float unnormalize(float x, float min, float max) {
	return x * (max - min) + min;
}

template<typename S, typename R, typename MIN, typename MAX>
inline void unnormalize(const S source[3], R result[3], const MIN min[3], const MAX max[3]) {
	result[0] = (R) unnormalize((float) source[0], (float) min[0], (float) max[0]);
	result[1] = (R) unnormalize((float) source[1], (float) min[1], (float) max[1]);
	result[2] = (R) unnormalize((float) source[2], (float) min[2], (float) max[2]);
}

template<typename S, typename MIN, typename MAX>
inline void unnormalizeSelf(S source[3], const MIN min[3], const MAX max[3]) {
	source[0] = (S) unnormalize((float) source[0], (float) min[0], (float) max[0]);
	source[1] = (S) unnormalize((float) source[1], (float) min[1], (float) max[1]);
	source[2] = (S) unnormalize((float) source[2], (float) min[2], (float) max[2]);
}

inline float clamp(float x) {
	if(x > 1)
		return 1;
	if(x < 0)
		return 0;
	return x;
}

inline void clampRgbSelf(float x[3]) {
	x[0] = clamp(x[0]);
	x[1] = clamp(x[1]);
	x[2] = clamp(x[2]);
}

inline void clampRgb(const float x[3], float result[3]) {
	result[0] = clamp(x[0]);
	result[1] = clamp(x[1]);
	result[2] = clamp(x[2]);
}

inline float wrap(float x) {
	if(x < 0)
			return 1.0f - fmodf(-x, 1);
	return fmodf(x, 1);
}

inline void clampHsvSelf(float x[3]) {
	x[0] = wrap(x[0]);
	x[1] = clamp(x[1]);
	x[2] = clamp(x[2]);
}

inline void clampHsv(const float x[3], float result[3]) {
	result[0] = wrap(x[0]);
	result[1] = clamp(x[1]);
	result[2] = clamp(x[2]);
}

inline void convertRgbToHsv(const float rgb[3], float hsv[3]) {
	float max = rgb[0];
	if(rgb[1] > max)
		max = rgb[1];
	if(rgb[2] > max)
		max = rgb[2];

	float min = rgb[0];
	if(rgb[1] < min)
		min = rgb[1];
	if(rgb[2] < min)
		min = rgb[2];

	if(max == min) { // greys
		hsv[0] = 0;
		hsv[1] = 0;
		hsv[2] = max;
		return;
	}

	if(rgb[0] == max) {
		hsv[0] = 0. + (rgb[1] - rgb[2]) / (max - min);
		if(hsv[0] < 0)
			hsv[0] += 6.;
	} else if (rgb[1] == max) {
		hsv[0] = 2. + (rgb[2] - rgb[0]) / (max - min);
	} else {
		hsv[0] = 4. + (rgb[0] - rgb[1]) / (max - min);
	}
	hsv[0] /= 6;
	hsv[1] = (max - min) / max;
	hsv[2] = max;
}

inline void convertRgbToHsv(const unsigned char rgb[3], unsigned char hsv[3]) {
	float max = rgb[0];
	if(rgb[1] > max)
		max = rgb[1];
	if(rgb[2] > max)
		max = rgb[2];

	float min = rgb[0];
	if(rgb[1] < min)
		min = rgb[1];
	if(rgb[2] < min)
		min = rgb[2];

	if(max == min) { // greys
		hsv[0] = 0;
		hsv[1] = 0;
		hsv[2] = (unsigned char) (255 * max);
		return;
	}

	float h;
	if(rgb[0] == max) {
		h = ((float) rgb[1] - (float) rgb[2]) / (max - min);
		if(h < 0)
			h += 6.;
	} else if (rgb[1] == max) {
		h = 2. + ((float) rgb[2] - (float) rgb[0]) / (max - min);
	} else {
		h = 4. + ((float) rgb[0] - (float) rgb[1]) / (max - min);
	}
	hsv[0] = (unsigned char) (255 * h / 6);
	hsv[1] = (unsigned char) (255 * (max - min) / max);
	hsv[2] = (unsigned char) max;
}

inline void convertHsvToRgb(const float hsv[3], float rgb[3]) {
	if(hsv[2] == 0) { // black
		set(rgb, 0);
	} else if(hsv[1] == 0) { // grays
		set(rgb, hsv[2]);
	} else {
		float h = hsv[0] * 6;
		int hCategory = (int) floorf(h);
		float hRemainder = h - hCategory;
		float pv = (1. - hsv[1]) * hsv[2];
		float qv = (1. - hsv[1] * hRemainder) * hsv[2];
		float tv = (1. - hsv[1] * (1. - hRemainder)) * hsv[2];
		switch(hCategory) {
			case 0: // r
				rgb[0] = hsv[2];
				rgb[1] = tv;
				rgb[2] = pv;
				break;
			case 1: // g
				rgb[0] = qv;
				rgb[1] = hsv[2];
				rgb[2] = pv;
				break;
			case 2:
				rgb[0] = pv;
				rgb[1] = hsv[2];
				rgb[2] = tv;
				break;
			case 3: // b
				rgb[0] = pv;
				rgb[1] = qv;
				rgb[2] = hsv[2];
				break;
			case 4:
				rgb[0] = tv;
				rgb[1] = pv;
				rgb[2] = hsv[2];
				break;
			case 5: // back to r
				rgb[0] = hsv[2];
				rgb[1] = pv;
				rgb[2] = qv;
				break;
		}
	}
}

inline void convertHsvToRgb(const unsigned char hsv[3], unsigned char rgb[3]) {
	if(hsv[2] == 0) { // black
		set(rgb, 0);
	} else if(hsv[1] == 0) { // grays
		set(rgb, hsv[2]);
	} else {
		float h = hsv[0] * 6. / 255.;
		float s = hsv[1] / 255.;
		int hCategory = (int) floorf(h);
		float hRemainder = h - hCategory;
		unsigned char pv = (unsigned char) ((1.f - s) * hsv[2]);
		unsigned char qv = (unsigned char) ((1. - s * hRemainder) * hsv[2]);
		unsigned char tv = (unsigned char) ((1. - s * (1. - hRemainder)) * hsv[2]);
		switch(hCategory) {
			case 0: // r
				rgb[0] = hsv[2];
				rgb[1] = tv;
				rgb[2] = pv;
				break;
			case 1: // g
				rgb[0] = qv;
				rgb[1] = hsv[2];
				rgb[2] = pv;
				break;
			case 2:
				rgb[0] = pv;
				rgb[1] = hsv[2];
				rgb[2] = tv;
				break;
			case 3: // b
				rgb[0] = pv;
				rgb[1] = qv;
				rgb[2] = hsv[2];
				break;
			case 4:
				rgb[0] = tv;
				rgb[1] = pv;
				rgb[2] = hsv[2];
				break;
			case 5: // back to r
				rgb[0] = hsv[2];
				rgb[1] = pv;
				rgb[2] = qv;
				break;
		}
	}
}

}
