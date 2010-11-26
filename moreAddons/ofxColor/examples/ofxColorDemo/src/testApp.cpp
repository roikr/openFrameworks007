#include "testApp.h"

#include "ofxColor.h"

void testApp::setup(){
	ofxColorf myColor(.1, .2, .3);
	cout << "ofxColorf myColor(.1, .2, .3);" << endl;
	cout << myColor << endl << endl;

	myColor.setRange(255);
	cout << "myColor.setRange(255);" << endl;
	cout << myColor << endl << endl;

	myColor.setMode(OF_COLOR_HSV);
	cout << "myColor.setMode(OF_COLOR_HSV);" << endl;
	cout << myColor << endl << endl;

	myColor.normalize();
	cout << "myColor.normalize();" << endl;
	cout << myColor << endl << endl;

	myColor.setMode(OF_COLOR_RGB);
	cout << "myColor.setMode(OF_COLOR_RGB);" << endl;
	cout << myColor << endl << endl;

	myColor.reset();
	cout << "myColor.reset();" << endl;
	cout << myColor << endl << endl;

	myColor.set(.1, .2, .3).setRange(255).setMode(OF_COLOR_HSV).normalize().setMode(OF_COLOR_RGB);
	cout << "myColor.set(.1, .2, .3).setRange(255).setMode(OF_COLOR_HSV).normalize().setMode(OF_COLOR_RGB);" << endl;
	cout << myColor << endl << endl;

	myColor.setRed(1.1).clamp();
	cout << "myColor.setRed(1.1).clamp();" << endl;
	cout << myColor << endl << endl;

	ofxColorf target;
	target.set(0, 1, 0).setRange(255);
	cout << "target.set(0, 1, 0).setRange(255);" << endl;
	myColor.lerp(target, .8);
	cout << "myColor.lerp(target, .8);" << endl;
	cout << myColor << endl << endl;

	cout << "myColor.set(0, 1, 0).distance(target.set(255, 0, 0);" << endl;
	cout << myColor.set(0, 1, 0).distance(target.set(255, 0, 0)) << endl;
}

void testApp::update(){
}

void testApp::draw(){
}
