#include "testApp.h"
#include "ofMain.h"



int main(int argc, char *argv[]) {
	ofSetupOpenGL(1024,768, OF_FULLSCREEN);			// <-------- setup the GL context

	ofRunApp(new testApp);
}
