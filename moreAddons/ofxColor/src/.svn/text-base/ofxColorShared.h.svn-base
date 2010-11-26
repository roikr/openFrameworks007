#pragma once

#include "ofMain.h"

enum ofxColorMode {
	OF_COLOR_RGB,
	OF_COLOR_HSV
};

namespace ofxColor {

template<typename A, typename B>
inline void set(A a[3], B b) {
	a[0] = (A) b;
	a[1] = (A) b;
	a[2] = (A) b;
}

template<typename A, typename B>
inline void set(A a[3], B b[3]) {
	a[0] = (A) b[0];
	a[1] = (A) b[1];
	a[2] = (A) b[2];
}

template<typename A, typename B, typename C, typename D>
inline void set(A a[3], B b, C c, D d) {
	a[0] = (A) b;
	a[1] = (A) c;
	a[2] = (A) d;
}

}
