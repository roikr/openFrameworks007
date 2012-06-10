#include "ofMain.h"
#include "ofAppGlutWindow.h"
#include "ofxCocoaWindow.h"
#include "testApp.h"




//========================================================================
int main( ){

    bool bUseCocoaWindow = true;
    
    if( bUseCocoaWindow )
    {
        ofxCocoaWindow cocoaWindow;
        ofSetupOpenGL( &cocoaWindow, 800, 600, OF_WINDOW );
    }
    else
    {
        ofAppGlutWindow window;
        ofSetupOpenGL(&window, 1024,768, OF_WINDOW);			// <-------- setup the GL context
    }
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new testApp());

}
