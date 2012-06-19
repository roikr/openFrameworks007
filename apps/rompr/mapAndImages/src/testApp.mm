#include "testApp.h"
#import <QuartzCore/QuartzCore.h>
#include "ofxURLFileLoader.h"


#define HOME_LATITUDE		32.082725 // 32.081725
#define HOME_LONGITUDE		34.772147

#define MAP_BLOCK_WIDTH     1500.0

#define MAP_RECT_WIDTH      mapKit.getMKMapView().visibleMapRect.size.width


//--------------------------------------------------------------
void testApp::setup(){	
    
    
	// register touch events
    ofSetLogLevel(OF_LOG_VERBOSE);
	ofRegisterTouchEvents(this);

	//ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
	
    
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
    
    
    
    
    string url = "http://107.21.224.181/mobile/start/549453367/AAACtapZAgifcBAEGBY8KYPyxlFMSZAjdhE3jd2EEQSLF9W3RnaiPYuuFiIhOW1sj3B94d01SJJlUzIPGCyE8JkBLWMEpRxxnkM2M2TXAZDZD";
    
    ofxHttpResponse response = ofxLoadURL(url);
    
//    cout << response.data << endl;
    
    //    for (vector<pair<string, string> >::iterator iter=response.cookies.begin();iter!=response.cookies.end();iter++) {
    //        cout << iter->first << ": " << iter->second << endl;
    //    }
    
    string recommendationsUrl = "http://107.21.224.181/recommendations/json/1";
    ofxHttpRequest recommendationsRequest(recommendationsUrl, recommendationsUrl, response.cookies);
    
    ofxHttpResponse recommendationsResponse = ofxLoadURL(recommendationsRequest);
    
    
	
    
    
    
	// Now parse the JSON
	bool parsingSuccessful = result.parse(recommendationsResponse.data.getText());
	if ( parsingSuccessful )
    {
		cout << result.getRawString() << endl;
        
        ofxJSONElement elements = result["user_recommendation_list"];
        
        
        
        for(int i=0; i<elements.size(); i++)
        {
            ofxJSONElement element = elements[i]["recommendation"];
            string filename = element["img_path"].asString();
            cout << i << ": " << filename << endl;
            if (!filename.empty()) {
                
                recommendation r;
                r.img_path = filename;
                r.longitude = atof(element["longitude"].asString().c_str()) ;
                r.latitude = atof(element["latitude"].asString().c_str()) ;
               
                
                recommendations.push_back(r);
                
                cout <<r.img_path << endl;
                
                ofxHttpRequest imageRequest(filename, filename, response.cookies);
                
                ofxHttpResponse imageResponse = ofxLoadURL(imageRequest);
                
                recommendations.back().image.loadImage(imageResponse.data);
                //images.back().update();
                
                
                recommendation &s = recommendations.back();
                cout << s.image.getWidth() << "\t" << s.image.getHeight() << "\t" << setprecision(15) << s.longitude <<"\t" << s.latitude << endl;
                
            }
            //            cout << recommendations[i].asString() << endl;
        }
        
        //cout << result["user_recommendation_list"].size();
		
	}
    else
    {
		cout  << "Failed to parse JSON" << endl;
	}

}

//--------------------------------------------------------------
void testApp::update(){
	if (bUpdatingRegion) {
//        cout << mapKit.getScreenCoordinatesForLocation(HOME_LATITUDE, HOME_LONGITUDE) << endl;
        
    }
}

//--------------------------------------------------------------
void testApp::draw(){	
    
	glClearColor(0,0,0,0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    

	
    
	
    ofFill();
	
	for (vector<recommendation>::iterator iter = recommendations.begin();iter!=recommendations.end();iter++) {
        ofPushMatrix();
//        CLLocationCoordinate2D location = mapKit.getMKMapView().userLocation.location.coordinate;
//        ofTranslate(mapKit.getScreenCoordinatesForLocation(location.latitude, location.longitude));
//        ofTranslate(mapKit.getScreenCoordinatesForLocation(HOME_LATITUDE, HOME_LONGITUDE));
         ofTranslate(mapKit.getScreenCoordinatesForLocation(iter->latitude, iter->longitude));
        float blockWidth =  MAP_BLOCK_WIDTH / MAP_RECT_WIDTH * ofGetWidth();
        float scale = blockWidth/max(iter->image.getWidth(),iter->image.getHeight());
        ofScale(scale, scale);
        ofTranslate(-0.5*ofPoint(iter->image.getWidth(),iter->image.getHeight()));
        float margin = 10;
        ofSetHexColor(0x000000);
        ofRect(-margin, -margin, iter->image.getWidth()+2*margin, iter->image.getHeight()+2*margin);
        ofSetHexColor(0xFFFFFF);
        iter->image.draw(0, 0);
        ofPopMatrix();
    }
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

