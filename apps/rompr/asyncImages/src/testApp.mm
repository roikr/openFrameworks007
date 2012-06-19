#include "testApp.h"
#import <QuartzCore/QuartzCore.h>



#define HOME_LATITUDE		32.081725
#define HOME_LONGITUDE		34.772147

#define MAP_BLOCK_WIDTH     1500.0

#define SIMULTANEOUS_CONNECTIONS 5

#define MAP_RECT_WIDTH      mapKit.getMKMapView().visibleMapRect.size.width


//--------------------------------------------------------------
void testApp::setup(){	
    
    
	// register touch events
    ofSetLogLevel(OF_LOG_VERBOSE);
	ofRegisterTouchEvents(this);
    ofxRegisterURLNotification(this);

	//ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
	ofEnableSmoothing();
    
    ofxiPhoneSetGLViewTransparent(true);
    ofxiPhoneSetGLViewUserInteraction(false);
    
    mapKit.open();
    
    // set initial position to specified coordinates, and cover 1000 Km x 1000 Km area
    mapKit.setRegionWithMeters(HOME_LATITUDE, HOME_LONGITUDE, 1000, 1000);
    
	// optionally add testApp as a listener for map event (callbacks below)
	mapKit.addListener(this);
	
	// set other optional parameters
	mapKit.setShowUserLocation(true);
	mapKit.setType(OFXIPHONE_MAPKIT_MAP); // OFXIPHONE_MAPKIT_HYRBID
    //	mapKit.setAllowZoom(false);
    //	mapKit.setAllowScroll(false);
    //	mapKit.setAllowUserInteraction(false);

//    [ofxiPhoneGetUIWindow() addSubview:[[MKMapView alloc] initWithFrame:[[UIScreen mainScreen] bounds]]];
    [ofxiPhoneGetUIWindow() bringSubviewToFront:ofxiPhoneGetGLView()];
    
    bUpdatingRegion = false;
    
    string url = "http://107.21.224.181/mobile/start/549453367/AAACtapZAgifcBAEGBY8KYPyxlFMSZAjdhE3jd2EEQSLF9W3RnaiPYuuFiIhOW1sj3B94d01SJJlUzIPGCyE8JkBLWMEpRxxnkM2M2TXAZDZD";
    
    ofxHttpResponse response = ofxLoadURL(url);
        
    string recommendationsUrl = "http://107.21.224.181/recommendations/json/1";
    ofxHttpRequest recommendationsRequest(recommendationsUrl, recommendationsUrl, response.cookies);
    
    ofxHttpResponse recommendationsResponse = ofxLoadURL(recommendationsRequest);
    
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
                
                item r;
                r.img_path = filename;
                r.longitude = atof(element["longitude"].asString().c_str()) ;
                r.latitude = atof(element["latitude"].asString().c_str()) ;
               
                
                items.push_back(r);
                
                cout <<r.img_path << endl;
                
                list.push_back(r.img_path);
                
                item &s = items.back();
                cout << s.image.getWidth() << "\t" << s.image.getHeight() << "\t" << setprecision(15) << s.longitude <<"\t" << s.latitude << endl;
                
            }
            
        }
        
	}
    else
    {
		cout  << "Failed to parse JSON" << endl;
	}
    
    logo.loadImage("logo.png");

}


void testApp::urlResponse(ofxHttpResponse &response) {
    string url = response.request.name;
    status = response.status;
    
    cout << "urlResponse: " << url <<", status: " << status << endl;
    
    
    vector<string>::iterator iter;
    for (iter=queue.begin();iter!=queue.end();iter++) {
        if (*iter == url) {
            break;
        }
    }
    
    if (iter==queue.end()) {
        status = -1;
        error = "could not find "+url+" in the queue";
    } else {
        
        queue.erase(iter);
        
        for (vector<item>::iterator riter=items.begin();riter!=items.end();riter++) {
            if (riter->img_path == url) {
                
                switch (response.status) {
                    case 200: 
                        riter->image.loadImage(response.data);
                     break;
                        
                    case -1:
                        error = response.error;
                        list.push_back(url);
                        break;
                        
                    default:
                        error = response.error;
                        break;
                }
                
                
                break;
            }
        }
    }
}


//--------------------------------------------------------------
void testApp::update(){
	if (bUpdatingRegion) {
//        cout << mapKit.getScreenCoordinatesForLocation(HOME_LATITUDE, HOME_LONGITUDE) << endl;
        
    }
   
    if (queue.size()<SIMULTANEOUS_CONNECTIONS) {
        
        vector<string>::iterator iter = list.begin();
        
        while (iter!=list.end() && queue.size()<SIMULTANEOUS_CONNECTIONS) {
            ofxLoadURLAsync(*iter);
            queue.push_back(*iter);
            list.erase(iter);
            iter = list.begin();
            
        }
    }

}

//--------------------------------------------------------------
void testApp::draw(){	
    
	glClearColor(0,0,0,0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    

	
    
	
    ofFill();
	
	for (vector<item>::iterator iter = items.begin();iter!=items.end();iter++) {
        ofPushMatrix();

        ofTranslate(mapKit.getScreenCoordinatesForLocation(iter->latitude, iter->longitude));
        float blockWidth =  MAP_BLOCK_WIDTH / MAP_RECT_WIDTH * ofGetWidth();
        
        if (iter->image.getWidth()) {

            float scale = blockWidth/max(iter->image.getWidth(),iter->image.getHeight());
            ofScale(scale, scale);
            ofTranslate(-0.5*ofPoint(iter->image.getWidth(),iter->image.getHeight()));
            float margin = 10;
            ofSetHexColor(0x000000);
            ofRect(-margin, -margin, iter->image.getWidth()+2*margin, iter->image.getHeight()+2*margin);
            ofSetHexColor(0xFFFFFF);
            iter->image.draw(0, 0);
        } else {
            float scale = blockWidth/logo.getWidth();
            ofScale(scale, scale);
            ofTranslate(-0.5*ofPoint(logo.getWidth(),logo.getHeight()));
            ofSetHexColor(0xFFFFFF);
            ofEnableAlphaBlending();
            logo.draw(0, 0);
            ofDisableAlphaBlending();
        }
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

