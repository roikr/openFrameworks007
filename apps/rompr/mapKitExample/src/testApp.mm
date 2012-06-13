#include "testApp.h"
#import <QuartzCore/QuartzCore.h>


#define HOME_LATITUDE		32.082725 // 32.081725
#define HOME_LONGITUDE		34.772147

#define MAP_BLOCK_WIDTH     1500.0


//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
    ofSetLogLevel(OF_LOG_VERBOSE);
	ofRegisterTouchEvents(this);

	//ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
	bikers.loadImage("images/bikers.jpg");
	gears.loadImage("images/gears.gif");
	tdf.loadImage("images/tdf_1972_poster.jpg");
	tdfSmall.loadImage("images/tdf_1972_poster.jpg");
	tdfSmall.resize(tdf.width / 4, tdf.height / 4);
	tdfSmall.setImageType(OF_IMAGE_GRAYSCALE);
	transparency.loadImage("images/transparency.png");
	bikeIcon.loadImage("images/bike_icon.png");
	bikeIcon.setImageType(OF_IMAGE_GRAYSCALE);
    
    ofxiPhoneSetGLViewTransparent(true);
    ofxiPhoneSetGLViewUserInteraction(false);
    
    mapKit.open();
    
    // set initial position to specified coordinates, and cover 1000 Km x 1000 Km area
    mapKit.setRegionWithMeters(HOME_LATITUDE, HOME_LONGITUDE, 1000, 1000);
    
	// optionally add testApp as a listener for map event (callbacks below)
	mapKit.addListener(this);
	
	// set other optional parameters
	mapKit.setShowUserLocation(true);
	mapKit.setType(OFXIPHONE_MAPKIT_HYRBID);
    //	mapKit.setAllowZoom(false);
    //	mapKit.setAllowScroll(false);
    //	mapKit.setAllowUserInteraction(false);

//    [ofxiPhoneGetUIWindow() addSubview:[[MKMapView alloc] initWithFrame:[[UIScreen mainScreen] bounds]]];
    [ofxiPhoneGetUIWindow() bringSubviewToFront:ofxiPhoneGetGLView()];
    
    bUpdatingRegion = false;
    scale = 1.0f;
}

//--------------------------------------------------------------
void testApp::update(){
	if (bUpdatingRegion) {
//        cout << mapKit.getScreenCoordinatesForLocation(HOME_LATITUDE, HOME_LONGITUDE) << endl;
        MKMapRect rect = mapKit.getMKMapView().visibleMapRect;
//        cout << rect.size.width << endl;
        scale = MAP_BLOCK_WIDTH/rect.size.width;
    }
}

//--------------------------------------------------------------
void testApp::draw(){	
    
	glClearColor(0,0,0,0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    

	ofSetHexColor(0xFFFFFF);
	
//	bikers.draw(0,0);
	gears.draw(600,0);
	tdf.draw(600,300);
	
	ofSetHexColor(0xDD3333);
    ofPushMatrix();
    CLLocationCoordinate2D location = mapKit.getMKMapView().userLocation.location.coordinate;
    ofTranslate(mapKit.getScreenCoordinatesForLocation(location.latitude, location.longitude));
    ofScale(scale, scale);
    ofTranslate(-ofPoint(tdfSmall.getWidth(),tdfSmall.getHeight())/2);
	tdfSmall.draw(0,0);
    ofPopMatrix();
    
	
	
	ofSetHexColor(0xFFFFFF);
	ofEnableAlphaBlending();
    transparency.draw(sin(ofGetElapsedTimeMillis()/1000.0f) * 100 + 500,20);
	ofDisableAlphaBlending();
	
   
	//ofSetHexColor(0xFFFFFF);
    ofPushMatrix();
    
    ofSetHexColor(0x000000);
    unsigned char * pixels = bikeIcon.getPixels();
	int w = bikeIcon.width;
	int h = bikeIcon.height;
    
     
    ofTranslate(mapKit.getScreenCoordinatesForLocation(HOME_LATITUDE, HOME_LONGITUDE));
	    
    ofScale(scale, scale);
    ofTranslate(-5*ofPoint(w,h));
    
    //bikeIcon.draw(0,0, 20,20);
    
    // getting the pixels out of an image, 
	// and then use the values to draw circles
	
	for (int i = 0; i < w; i++){
		for (int j = 0; j < h; j++){
			int value = pixels[j * w + i];
			float pct = 1 - (value / 255.0f);
			ofCircle(i*10,j*10,1 + 5*pct);		
		}
	}

    
    ofPopView();
}

//--------------------------------------------------------------
void testApp::regionWillChange(bool animated){
	printf("testApp::regionWillChange | animated: %i\n", animated);
    bUpdatingRegion = true;
}

//--------------------------------------------------------------
void testApp::regionDidChange(bool animated){
	printf("testApp::regionDidChange | animated: %i\n", animated);
    bUpdatingRegion = false;
}

//--------------------------------------------------------------
void testApp::willStartLoadingMap(){
	printf("testApp::willStartLoadingMap\n");
}

//--------------------------------------------------------------
void testApp::didFinishLoadingMap(){
	printf("testApp::didFinishLoadingMap\n");
}

//--------------------------------------------------------------
void testApp::errorLoadingMap(string errorDescription){
	printf("testApp::errorLoadingMap : %s\n", errorDescription.c_str());
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
void testApp::touchCancelled(ofTouchEventArgs& args){

}

