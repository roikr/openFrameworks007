#include "testApp.h"
#import <QuartzCore/QuartzCore.h>
#include "ofxJSONElement.h"
#import "EventRouteWindow.h"


#define HOME_LATITUDE		32.081725
#define HOME_LONGITUDE		34.772147

#define MAP_BLOCK_WIDTH     1500.0

#define SIMULTANEOUS_CONNECTIONS 5

#define MAP_RECT_WIDTH      mapKit.getMKMapView().visibleMapRect.size.width

enum requestType {
    REQUEST_TYPE_LOGIN,
    REQUEST_TYPE_QUERY,
    REQUEST_TYPE_IMAGE,
    REQUEST_TYPE_RECOMMENDATION
    
};

//--------------------------------------------------------------
void testApp::setup(){	
    
    [ofxiPhoneGetGLView() removeFromSuperview];
    UIWindow *oldWindow = ofxiPhoneGetUIWindow();
    [oldWindow resignKeyWindow];
    [oldWindow release];
    
    
    CGRect screenBounds = [[UIScreen mainScreen] bounds];
	EventRouteWindow *window = [[EventRouteWindow alloc] initWithFrame:screenBounds];
	[window addSubview:ofxiPhoneGetGLView()];
    [window setView:ofxiPhoneGetGLView()];
    //[self.window setOverlayListener:[self.viewController view]];
	[window makeKeyAndVisible];
    
        
	// register touch events
//    ofSetLogLevel(OF_LOG_VERBOSE); // roikr: verbose for details
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
    
    
    // disable double tap on MKMapView responder
    for (UIGestureRecognizer *recognizer in [mapKit.getMKMapView() gestureRecognizers]) {
//        NSLog(@"%@",[recognizer description]);
        if ([recognizer isKindOfClass:[UITapGestureRecognizer class]]) {
            if (((UITapGestureRecognizer *)recognizer).numberOfTapsRequired == 2) {
                [mapKit.getMKMapView() removeGestureRecognizer:recognizer];
                break;
            }
        }
    }

	
	// set other optional parameters
	mapKit.setShowUserLocation(true);
	mapKit.setType(OFXIPHONE_MAPKIT_MAP); // OFXIPHONE_MAPKIT_HYRBID
    //	mapKit.setAllowZoom(false);
    //	mapKit.setAllowScroll(false);
    //	mapKit.setAllowUserInteraction(false);

//    [ofxiPhoneGetUIWindow() addSubview:[[MKMapView alloc] initWithFrame:[[UIScreen mainScreen] bounds]]];
    [ofxiPhoneGetUIWindow() bringSubviewToFront:ofxiPhoneGetGLView()];
    
    bUpdatingRegion = false;
    bQueryLocation = false;
    
    logo.loadImage("logo.png");
    
    string url = "http://107.21.224.181/mobile/start/549453367/AAACtapZAgifcBAEGBY8KYPyxlFMSZAjdhE3jd2EEQSLF9W3RnaiPYuuFiIhOW1sj3B94d01SJJlUzIPGCyE8JkBLWMEpRxxnkM2M2TXAZDZD";
    
    queue[ofxLoadURLAsync(ofxHttpRequest(url,url))] = REQUEST_TYPE_LOGIN;
    
    

}

bool testApp::getIsItemExist(int itemID) {
    for (list<item>::iterator iter = items.begin();iter!=items.end();iter++) {
        if (iter->itemID == itemID) {
            return true;
        }
    }
    return false;
}


void testApp::queryViewLocation() {
//    ofxMapKitLocation location;
//    location.latitude = HOME_LATITUDE;
//    location.longitude = HOME_LONGITUDE;
//    int distance = 1;
//    queue[ofxLoadURLAsync(ofxHttpRequest("http://107.21.224.181/recommendations/json/1",response.cookies))] = REQUEST_TYPE_QUERY;
    
    ofxMapKitLocation location=mapKit.getCenterLocation();
    MKMapRect rect = mapKit.getMKMapView().visibleMapRect;
    double meters = MKMetersBetweenMapPoints(rect.origin, MKMapPointMake(MKMapRectGetMaxX(rect), MKMapRectGetMaxY(rect)));
    
    for (list<item>::iterator iter = items.begin();iter!=items.end();iter++) {
        if (MKMetersBetweenMapPoints(MKMapPointForCoordinate(mapKit.getCenterLocation()),MKMapPointForCoordinate(iter->location))> 2*meters) {
            iter = items.erase(iter);
        }
    }
    
    int distance = round(meters/1000.0);
    string url = "http://107.21.224.181/mobile/recommendations/circle/"+ofToString(location.latitude,6)+"/"+ofToString(location.longitude,6)+"/"+ofToString(distance);
    cout << "viewDiagonal: " << meters << "\t" << distance << "\t" << url << endl;;
    queue[ofxLoadURLAsync(ofxHttpRequest(url,cookies))] = REQUEST_TYPE_QUERY;
}



void testApp::processQuery(ofBuffer &query) {
    ofxJSONElement result;
    bool parsingSuccessful = result.parse(query.getText());
	if ( parsingSuccessful )
    {
		cout << result.getRawString() << endl;
        ofxJSONElement elements = result["user_recommendation_list"];
        
        int j = 0;
        for(int i=0; i<elements.size(); i++)
        {
            ofxJSONElement element = elements[i]["recommendation"];
            string filename = element["img_path"].asString();
            int itemID = atoi(element["id"].asString().c_str());
            if (!filename.empty() && !getIsItemExist(itemID)) {
                
                item r;
                r.img_path = filename;
                r.location.longitude = atof(element["longitude"].asString().c_str()) ;
                r.location.latitude = atof(element["latitude"].asString().c_str()) ;
                r.itemID = itemID;
                items.push_back(r);
//                cout <<r.img_path << endl;
                imagesList.push_back(r.img_path);
                
                item &s = items.back();
//                cout << s.image.getWidth() << "\t" << s.image.getHeight() << "\t" << setprecision(15) << s.longitude <<"\t" << s.latitude << endl;
                
                
                cout << j << ": " << filename << ", distanceFromCenter: " << MKMetersBetweenMapPoints(MKMapPointForCoordinate(mapKit.getCenterLocation()),MKMapPointForCoordinate(s.location)) << endl;
                j++;
                
            }
        }
	}
    else
    {
		cout  << "Failed to parse JSON" << endl;
	}

}

void testApp::viewRecommendation(string html) {
    UIWebView *webView = [[UIWebView alloc] initWithFrame:CGRectMake(0, 0, ofGetWidth(), ofGetHeight())];
    
    //webView.delegate	= NULL;
    
    
    
//    NSString *path = [[NSBundle mainBundle] pathForResource:@"webViewContent" ofType:@"html"];
//	NSFileHandle *readHandle = [NSFileHandle fileHandleForReadingAtPath:path];
//    
//	NSString *htmlString = [[NSString alloc] initWithData: 
//                            [readHandle readDataToEndOfFile] encoding:NSUTF8StringEncoding];
	
    NSString *htmlString = [NSString stringWithCString:html.c_str() encoding:NSUTF8StringEncoding];
    
	// to make html content transparent to its parent view -
	// 1) set the webview's backgroundColor property to [UIColor clearColor]
	// 2) use the content in the html: <body style="background-color: transparent">
	// 3) opaque property set to NO
	//
	webView.opaque = NO;
	webView.backgroundColor = [UIColor clearColor];
    [webView loadHTMLString:htmlString baseURL:nil];
	[htmlString release];
    
    [ofxiPhoneGetUIWindow() addSubview:webView];
}

void testApp::urlResponse(ofxHttpResponse &response) {
    string url = response.request.name;
    status = response.status;
    
    cout << "urlResponse: " << url <<", status: " << status << endl;
    
    
    map<int,int>::iterator iter = queue.find(response.request.getID());
    
    if (iter!=queue.end()) {

        queue.erase(iter);
        
        switch (iter->second) {
            case REQUEST_TYPE_LOGIN:
                switch (response.status) {
                    case 200: 
                        cookies = response.cookies;
                        bQueryLocation = true;
                        
                        break;
                        
                    case -1:
                        cout << "REQUEST_TYPE_LOGIN: -1, error" << endl;
                        error = response.error;
                        break;
                        
                    default:
                        error = response.error;
                        break;
                }
                break;
                break;
            case REQUEST_TYPE_QUERY:
               
                switch (response.status) {
                    case 200: 
                         processQuery(response.data);
                        break;
                        
                    case -1:
                        cout << "REQUEST_TYPE_QUERY: -1" << endl;
                        error = response.error;
                        break;
                        
                    default:
                        error = response.error;
                        break;
                }
                break;
                
            case REQUEST_TYPE_IMAGE:
                for (list<item>::iterator riter=items.begin();riter!=items.end();riter++) {
                    if (riter->img_path == url) {
                        
                        switch (response.status) {
                            case 200: {
                                ofImage &image = riter->image; 
                                image.setUseTexture(false);
                                image.loadImage(response.data);
                                cout << "imageLoaded:" << url << "\t" << image.getWidth() << "\t" << image.getHeight() << "\tid: " << riter->itemID << endl;
                                
                                float size = MAX(image.getWidth(),image.getHeight());
                                float maxSize = 1024;
                                if (size > maxSize) {
//                                    image.resize(image.getWidth()*maxSize/size, image.getHeight()*maxSize/size);
                                    image.clear();
                                }
                                
                                image.setUseTexture(true);
                                image.reloadTexture();
                                
                            }  break;
                                
                            case -1:
                                cout << "REQUEST_TYPE_IMAGE: -1" << endl;
                                error = response.error;
                                imagesList.push_back(url);
                                break;
                                
                            default:
                                error = response.error;
                                break;
                        }
                        break;
                    }
                    else {
                        cout << url << " is not in the list" << endl;
                    }
                }
                break;
                
            default:
                break;
        }
        
        

    } else {
        status = -1;
        error = "could not find "+url+" in the queue";
    } 
}


//--------------------------------------------------------------
void testApp::update(){
	if (bUpdatingRegion) {
//        cout << mapKit.getScreenCoordinatesForLocation(HOME_LATITUDE, HOME_LONGITUDE) << endl;
        
    }
   
    if (queue.size()<SIMULTANEOUS_CONNECTIONS) {
        
        if (bQueryLocation && !cookies.empty()) {
            bQueryLocation = false;
            queryViewLocation();
            
        }
        
        vector<string>::iterator iter = imagesList.begin();
        
        while (iter!=imagesList.end() && queue.size()<SIMULTANEOUS_CONNECTIONS) {
            queue[ofxLoadURLAsync(*iter)] = REQUEST_TYPE_IMAGE;
            imagesList.erase(iter);
            iter = imagesList.begin();
            
        }
    }

}

//--------------------------------------------------------------
void testApp::draw(){	
    
	glClearColor(0,0,0,0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    

	
    
	
    ofFill();
	
	for (list<item>::iterator iter = items.begin();iter!=items.end();iter++) {
        ofPushMatrix();

        //ofTranslate(mapKit.getScreenCoordinatesForLocation(iter->latitude, iter->longitude));
        ofTranslate(mapKit.getScreenCoordinatesForLocation(iter->location.latitude, iter->location.longitude));
        float blockWidth =  MAP_BLOCK_WIDTH / MAP_RECT_WIDTH * ofGetWidth();
        
        if (iter->image.getWidth()) {

            float scale = blockWidth/max(iter->image.getWidth(),iter->image.getHeight());
            ofScale(scale, scale);
            ofTranslate(-0.5*ofPoint(iter->image.getWidth(),iter->image.getHeight()));
            float margin = 10;
            ofSetHexColor(0xc9dfaf);
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
//	printf("testApp::regionWillChange | animated: %i\n", animated);
    bUpdatingRegion = true;
}

//--------------------------------------------------------------
void testApp::regionDidChange(bool animated){
//	printf("testApp::regionDidChange | animated: %i\n", animated);
    bUpdatingRegion = false;
    bQueryLocation = true;
}

//--------------------------------------------------------------
void testApp::willStartLoadingMap(){
//	printf("testApp::willStartLoadingMap\n");
}

//--------------------------------------------------------------
void testApp::didFinishLoadingMap(){
//	printf("testApp::didFinishLoadingMap\n");
}

//--------------------------------------------------------------
void testApp::errorLoadingMap(string errorDescription){
//	printf("testApp::errorLoadingMap : %s\n", errorDescription.c_str());
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
//    cout << "touchDown" << endl;
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
//    cout << "touchMoved" << endl;
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
//    cout << "touchUp" << endl;
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
//    cout << "touchDoubleTap" << endl;
    
}

//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs& args){
//    cout << "touchCancelled" << endl;
}

