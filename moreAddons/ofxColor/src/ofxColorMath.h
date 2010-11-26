#pragma once

#include "ofxColorShared.h"

/*
 Some color-math related links:
 http://dev.processing.org/reference/core/javadoc/processing/core/PImage.html#blendColor%28int,%20int,%20int%29
 http://www.nathanm.com/photoshop-blending-math/
 http://en.wikipedia.org/wiki/Blend_modes
*/

namespace ofxColor {

template<typename A, typename B, typename R>
inline void add(const A a[3], const B b[3], R result[3]) {
	result[0] = (R) (a[0] + b[0]);
	result[1] = (R) (a[1] + b[1]);
	result[2] = (R) (a[2] + b[2]);
}

template<typename A, typename B, typename R>
inline void subtract(const A a[3], const B b[3], R result[3]) {
	result[0] = (R) (a[0] - b[0]);
	result[1] = (R) (a[1] - b[1]);
	result[2] = (R) (a[2] - b[2]);
}

template<typename A, typename B, typename R>
inline void multiply(const A a[3], const B b[3], R result[3]) {
	result[0] = (R) (a[0] * b[0]);
	result[1] = (R) (a[1] * b[1]);
	result[2] = (R) (a[2] * b[2]);
}

template<typename A, typename B, typename R>
inline void divide(const A a[3], const B b[3], R result[3]) {
	result[0] = (R) (a[0] / b[0]);
	result[1] = (R) (a[1] / b[1]);
	result[2] = (R) (a[2] / b[2]);
}

template<typename A, typename B, typename R>
inline void add(const A a[3], B b, R result[3]) {
	result[0] = (R) (a[0] + b);
	result[1] = (R) (a[1] + b);
	result[2] = (R) (a[2] + b);
}

template<typename A, typename B, typename R>
inline void subtract(const A a[3], B b, R result[3]) {
	result[0] = (R) (a[0] - b);
	result[1] = (R) (a[1] - b);
	result[2] = (R) (a[2] - b);
}

template<typename A, typename B, typename R>
inline void multiply(const A a[3], B b, R result[3]) {
	result[0] = (R) (a[0] * b);
	result[1] = (R) (a[1] * b);
	result[2] = (R) (a[2] * b);
}

template<typename A, typename B, typename R>
inline void divide(const A a[3], B b, R result[3]) {
	result[0] = (R) (a[0] / b);
	result[1] = (R) (a[1] / b);
	result[2] = (R) (a[2] / b);
}

template<typename A, typename B>
inline void addSelf(A a[3], const B b[3]) {
	a[0] += b[0];
	a[1] += b[1];
	a[2] += b[2];
}

template<typename A, typename B>
inline void subtractSelf(A a[3], const B b[3]) {
	a[0] -= b[0];
	a[1] -= b[1];
	a[2] -= b[2];
}

template<typename A, typename B>
inline void multiplySelf(A a[3], const B b[3]) {
	a[0] *= b[0];
	a[1] *= b[1];
	a[2] *= b[2];
}

template<typename A, typename B>
inline void divideSelf(A a[3], const B b[3]) {
	a[0] /= b[0];
	a[1] /= b[1];
	a[2] /= b[2];
}

template<typename A, typename B>
inline void addSelf(A a[3], B b) {
	a[0] += b;
	a[1] += b;
	a[2] += b;
}

template<typename A, typename B>
inline void subtractSelf(A a[3], B b) {
	a[0] -= b;
	a[1] -= b;
	a[2] -= b;
}

template<typename A, typename B>
inline void multiplySelf(A a[3], B b) {
	a[0] *= b;
	a[1] *= b;
	a[2] *= b;
}

template<typename A, typename B>
inline void divideSelf(A a[3], B b) {
	a[0] /= b;
	a[1] /= b;
	a[2] /= b;
}

template<typename B, typename T, typename R>
void min(const B bottom[3], const T top[3], R result[3]) {
	result[0] = bottom[0] < top[0] ? bottom[0] : top[0];
	result[1] = bottom[1] < top[1] ? bottom[1] : top[1];
	result[2] = bottom[2] < top[2] ? bottom[2] : top[2];
}

template<typename B, typename T, typename R>
void max(const B bottom[3], const T top[3], R result[3]) {
	result[0] = bottom[0] > top[0] ? bottom[0] : top[0];
	result[1] = bottom[1] > top[1] ? bottom[1] : top[1];
	result[2] = bottom[2] > top[2] ? bottom[2] : top[2];
}

inline void average(const float bottom[3], const float top[3], float result[3]) {
	set(result, bottom);
	addSelf(result, top);
	divideSelf(result, 2);
}

inline void average(const unsigned char bottom[3], const unsigned char top[3], unsigned char result[3]) {
	result[0] = bottom[0] >> 1 + top[0] >> 1;
	result[1] = bottom[1] >> 1 + top[1] >> 1;
	result[2] = bottom[2] >> 1 + top[2] >> 1;
}

}
