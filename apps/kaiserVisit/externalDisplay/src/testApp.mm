#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	//If you want a landscape oreintation 
//	iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
	ofBackground(127,127,127);
    
    NSArray			*screens;
	UIScreen		*aScreen;
	UIScreenMode	*mode;
	
	// 1.	
	
	// Log the current screens and display modes
	screens = [UIScreen screens];
    
    
    ss << "Device has " << [screens count] << " screen(s)."<<endl;
	
	uint32_t screenNum = 1;
	for (aScreen in screens) {			  
		NSArray *displayModes;
        
        ss << "Screen "<<screenNum << endl;
		
		displayModes = [aScreen availableModes];
		for (mode in displayModes) {
            ss << "Screen mode: " << mode.size.width << "x" << mode.size.height   << endl;
		}
		
		screenNum++;
	}
	
	NSUInteger screenCount = [screens count];
	
    bExt = screenCount > 1;
    
	if (screenCount > 1) {
		// 2.
		
		// Select first external screen
		extScreen = [screens objectAtIndex:1];
        
        if (extWindow == nil || !CGRectEqualToRect(extWindow.bounds, [extScreen bounds])) {
            // Size of window has actually changed
            
            // 4.
            extWindow = [[UIWindow alloc] initWithFrame:[extScreen bounds]];
            
            // 5.
            extWindow.screen = extScreen;
            
//            [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1 sharegroup:[iPhoneGetGLView().context sharegroup]]
            
//            [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1 
//                                  sharegroup:theContext.sharegroup];
            

            glView = [[SharedGLView alloc] initWithFrame:[extWindow frame] andDepth:iPhoneGetOFWindow()->isDepthEnabled() andContext:[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1 sharegroup:[iPhoneGetGLView().context sharegroup]]];
            
//            glView = [[EAGLView alloc] initWithFrame:[extWindow frame] andDepth:iPhoneGetOFWindow()->isDepthEnabled() andAA:iPhoneGetOFWindow()->isAntiAliasingEnabled() andNumSamples:iPhoneGetOFWindow()->getAntiAliasingSampleCount() andRetina:iPhoneGetOFWindow()->isRetinaSupported()];
            
//            [iPhoneGetGLView() removeFromSuperview];
            
            [extWindow addSubview:glView];
//            [view release];
            
            // 6.
            
            // 7.
            [extWindow makeKeyAndVisible];

        }
    }
//
//    
//    
//    
}

//--------------------------------------------------------------
void testApp::update(){
    
    
    
    if (bExt) {
        CGSize size = [extWindow frame].size;
        [glView startRender];
        
        //we do this as ofGetWidth() now accounts for rotation 
        //so we just make our viewport across the whole screen
        glViewport( 0, 0, size.width, size.height );
        
        float * bgPtr = ofBgColorPtr();
        bool bClearAuto = ofbClearBg();
        if ( bClearAuto == true){
            glClearColor(bgPtr[0],bgPtr[1],bgPtr[2], bgPtr[3]);
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        
        ofSetupScreenPerspective(size.width, size.height, ofGetOrientation(), true);
        ofDrawBitmapString(ss.str(), 0,0);
        [glView finishRender];
         
    }
       
}

//--------------------------------------------------------------
void testApp::draw(){
	ofDrawBitmapString(ss.str(), 0,0);
}

//--------------------------------------------------------------
void testApp::exit(){

}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::lostFocus(){

}

//--------------------------------------------------------------
void testApp::gotFocus(){

}

//--------------------------------------------------------------
void testApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void testApp::deviceOrientationChanged(int newOrientation){

}


//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs& args){

}

