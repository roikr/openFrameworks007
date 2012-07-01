#include "testApp.h"
#import <QuartzCore/QuartzCore.h>
#include "cJSON.h"
#import "EventRouteWindow.h"


#define HOME_LATITUDE		32.081725
#define HOME_LONGITUDE		34.772147

#define MAP_BLOCK_WIDTH     1500.0

#define SIMULTANEOUS_CONNECTIONS 15

#define MAP_RECT_WIDTH      mapKit.getMKMapView().visibleMapRect.size.width

const string HOST_NAME = "107.21.224.181";
const string USER_ID = "549453367";
const string FB_ACCESS_TOKEN ="AAACtapZAgifcBALkk9e7p6RYZAYm6hkZCXKub5tdRrD9DyBASA2oQx0xipZBpZBHwL4jIGZA5WjiZBlugZB5QQ6ak7fz0JLyKV8LxDx1sijoWwZDZD";

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
    
    string url = "http://"+HOST_NAME+"/mobile/start/"+USER_ID+"/"+FB_ACCESS_TOKEN;
    queue[ofxLoadURLAsync(ofxHttpRequest(url,url))] = REQUEST_TYPE_LOGIN;
    
    selected = items.end();

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
//    queue[ofxLoadURLAsync(ofxHttpRequest("http://"+HOST_NAME+"/recommendations/json/1",response.cookies))] = REQUEST_TYPE_QUERY;
    
    ofxMapKitLocation location=mapKit.getCenterLocation();
    MKMapRect rect = mapKit.getMKMapView().visibleMapRect;
    double meters = MKMetersBetweenMapPoints(rect.origin, MKMapPointMake(MKMapRectGetMaxX(rect), MKMapRectGetMaxY(rect)));
    
    for (list<item>::iterator iter = items.begin();iter!=items.end();iter++) {
        if (MKMetersBetweenMapPoints(MKMapPointForCoordinate(mapKit.getCenterLocation()),MKMapPointForCoordinate(iter->location))> 2*meters) {
            iter = items.erase(iter);
        }
    }
    
    int distance = round(meters/1000.0);
    string url = "http://"+HOST_NAME+"/mobile/recommendations/circle/"+ofToString(location.latitude,6)+"/"+ofToString(location.longitude,6)+"/"+ofToString(distance);
    cout << "viewDiagonal: " << meters << "\t" << distance << "\t" << url << endl;;
    queue[ofxLoadURLAsync(ofxHttpRequest(url,cookies))] = REQUEST_TYPE_QUERY;
}



void testApp::processQuery(ofBuffer &query) {
    cJSON *result;
    result=cJSON_Parse(query.getText().c_str());
	if ( result )
    {
//		cout << result.getRawString() << endl;
        cJSON *elements = cJSON_GetObjectItem(result, "user_recommendation_list");
        
        int j = 0;
        for(int i=0; i<cJSON_GetArraySize(elements); i++)
        {
            cJSON *element = cJSON_GetObjectItem(cJSON_GetArrayItem(elements, i),"recommendation");
            cJSON *r_id = cJSON_GetObjectItem(element,"id");
            cJSON *longitude = cJSON_GetObjectItem(element,"longitude");
            cJSON *latitude = cJSON_GetObjectItem(element,"latitude");
            cJSON *img_path = cJSON_GetObjectItem(element,"img_path");
            
//            cout << i << "\t" << r_id->type << "\t" << longitude->type << "\t" << latitude->type << "\t" << img_path->type << endl;
//             cout << i << "\t" << r_id->valuestring << "\t" << longitude->valuestring << "\t" << latitude->valuestring << "\t" << img_path->valuestring << endl;
//            if (r_id->type==cJSON_Number && longitude->type==cJSON_Number && latitude->type == cJSON_Number && img_path->type == cJSON_String ) {
//                cout << i << "\t" << r_id->string << "\t" << longitude->string << "\t" << latitude->string << "\t" << img_path->string << endl;
//            }
            
            int itemID = atoi(r_id->valuestring);
            
            
                
            if (!getIsItemExist(itemID)) {
                item r;
                r.img_path = img_path->valuestring;
                r.location.longitude = longitude->type == cJSON_Number ? longitude->valuedouble : atof(longitude->valuestring);
                r.location.latitude = latitude->type == cJSON_Number ? latitude->valuedouble : atof(latitude->valuestring);
                r.itemID = itemID;
                if (r.location.longitude && r.location.latitude && !r.img_path.empty()) {
                    cout << i << "\t" << j << "\t" << r.itemID << "\t" << r.location.longitude << "\t" << r.location.latitude << "\t" << r.img_path << endl;
//                    cout << j << MKMetersBetweenMapPoints(MKMapPointForCoordinate(mapKit.getCenterLocation()),MKMapPointForCoordinate(r.location)) << endl;
                    items.push_back(r);
                    selected = items.end(); // roikr: assume that it can't happend while easing
                    imagesList.push_back(r.img_path);
                    j++;
                }
                
            }
        }
        
        cJSON_Delete(result);
	}
    else
    {
		cout  << "Failed to parse JSON" << endl;
	}

}

void testApp::showRecommendation(string html) {
    UIView *view = [[UIView alloc] initWithFrame:CGRectMake(0, 0, ofGetWidth(), ofGetHeight())];
    view.opaque = NO;
	view.backgroundColor = [UIColor clearColor];
    view.alpha = 1.0f;
    UIView *blackView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, ofGetWidth(), ofGetHeight())];
    blackView.backgroundColor = [UIColor whiteColor];
    blackView.alpha = 0.0;
    //black.opaque = YES;
    
    UIWebView *webView = [[UIWebView alloc] initWithFrame:CGRectMake(0, ofGetHeight(), ofGetWidth(), ofGetHeight())];
    
    [view addSubview:webView];
    [view insertSubview:blackView belowSubview:webView];
	webView.opaque = NO;
	webView.backgroundColor = [UIColor clearColor];
    
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

    [webView loadHTMLString:htmlString baseURL:nil];
//	[htmlString release];
    
    [ofxiPhoneGetUIWindow() addSubview:view];
     
    dispatch_async(dispatch_get_main_queue(), ^{
        [UIView animateWithDuration:1.0 delay:0.0 options: UIViewAnimationOptionTransitionNone | UIViewAnimationOptionCurveEaseOut 
                         animations:^{
                             blackView.alpha = 0.5;
                             CGRect frame = webView.frame;
                             frame.origin.y = 0;
                             webView.frame = frame;
                             
                                                          
                         } 
                         completion:NULL];
    });
    
    
    [blackView release];
    [webView release];
    [view release];
}

void testApp::hideRecommendation() {
    for (UIView *view in [ofxiPhoneGetUIWindow() subviews]) {
        if (view!=ofxiPhoneGetGLView() && view!=mapKit.getMKMapView() ) {
            UIView *blackView = [view.subviews objectAtIndex:0];
            UIWebView *webView = [view.subviews objectAtIndex:1];
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [UIView animateWithDuration:1.0 delay:0.0 options: UIViewAnimationOptionTransitionNone | UIViewAnimationOptionCurveEaseOut 
                                 animations:^{
                                     blackView.alpha = 0.0;
                                     CGRect frame = webView.frame;
                                     frame.origin.y = ofGetHeight();
                                     webView.frame = frame;
                                     
                                     
                                 } 
                                 completion:^(BOOL finished){ 
                                     [view removeFromSuperview];
                                     
                                 }
                 ];
            });
            
            break;
        }
    }
}

void testApp::urlResponse(ofxHttpResponse &response) {
    string url = response.request.name;
    status = response.status;
    
    cout << "urlResponse: " << url <<", status: " << status << endl;
    
    
    map<int,int>::iterator iter = queue.find(response.request.getID());
    
    if (iter!=queue.end()) {

        switch (response.status) {
            case 200: 
            case 302:
                switch (iter->second) {
                    case REQUEST_TYPE_LOGIN:
                        cookies = response.cookies;
//                        bQueryLocation = true; // roikr: no need to query because there is still query waiting from startup region change
                        break;
                        
                    case REQUEST_TYPE_QUERY:
                        processQuery(response.data);
                        break;
                        
                    case REQUEST_TYPE_IMAGE: {
                        list<item>::iterator riter=items.begin();
                        while (riter->img_path != url &&  riter!=items.end()) {
                            riter++;
                        }
                        
                        if (riter!=items.end()) {
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
                            
                            break;
                            
                            
                        }
                        else {
                            cout << url << " is not in the list" << endl;
                        }
                        
                    } break;
                        
                    case REQUEST_TYPE_RECOMMENDATION:
                        if (selected != items.end() && !bDeselect) {
                            showRecommendation(response.data.getText());
                        }
                        break;
                        
                    default:
                        break;
                    }
                
                break;
                
            case -1:
                cout << "requestType: " << iter->second << ", status:  -1, error: " << response.error << endl;
                error = response.error;
                // imagesList.push_back(url);
                break;
                
            default:
                error = response.error;
                cout << "requestType: " << iter->second << ", status:  -1, error: " << response.error << endl;
                break;
        }
        
    
        
        
        
        queue.erase(iter);
    } else {
        status = -1;
        error = "could not find "+url+" in the queue";
        cout << error << endl;
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
    
    if (selected!=items.end()) {
        ease.update();
        
        if (bDeselect && !ease.getIsEasing()) {
            bDeselect = false;
            selected = items.end();
        }
    }

}

//--------------------------------------------------------------
void testApp::draw(){	
    
	glClearColor(0,0,0,0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    

	
    
	
    ofFill();
	
	for (list<item>::iterator iter = items.begin();iter!=items.end();iter++) {
        if (iter!=selected) {
        
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
    
    if (selected!=items.end()) {
        ease.begin();
        ofTranslate(ofVec2f(selected->image.getWidth(), selected->image.getHeight())*-0.5);
        selected->image.draw(0, 0);
        ease.end();
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
    float blockWidth =  MAP_BLOCK_WIDTH / MAP_RECT_WIDTH * ofGetWidth();
    if (selected != items.end()) {
        if (!bDeselect) {
            bDeselect = true;
            list<item>::iterator iter = selected;
            ofVec2f pos = mapKit.getScreenCoordinatesForLocation(iter->location.latitude, iter->location.longitude);
            ofRectangle rect;
            float scale = blockWidth/max(iter->image.getWidth(),iter->image.getHeight());
            rect.setFromCenter(pos, iter->image.getWidth()*scale, iter->image.getHeight()*scale);
            
            
            float srcScl = MIN(ofGetWidth()/iter->image.getWidth(),ofGetHeight()/iter->image.getHeight());
            ease.setup(EASE_OUT_QUAD, values(ofVec2f(ofGetWidth(),ofGetHeight())*0.5,srcScl,0),values(pos,scale,0));
            hideRecommendation();
        }
        
    } else {
        bDeselect = false;
        
        for (list<item>::iterator iter = items.begin();iter!=items.end();iter++) {
            ofVec2f pos = mapKit.getScreenCoordinatesForLocation(iter->location.latitude, iter->location.longitude);
            ofRectangle rect;
            float scale = blockWidth/max(iter->image.getWidth(),iter->image.getHeight());
            rect.setFromCenter(pos, iter->image.getWidth()*scale, iter->image.getHeight()*scale);
            
            if (rect.inside(ofVec2f(touch.x,touch.y))) {
//                cout << "found: " << iter->itemID << endl;
                selected = iter;
                float targetScl = MIN(ofGetWidth()/iter->image.getWidth(),ofGetHeight()/iter->image.getHeight());
                ease.setup(EASE_OUT_QUAD, values(pos,scale,0), values(ofVec2f(ofGetWidth(),ofGetHeight())*0.5,targetScl,0));
                
                
                string url = "http://"+HOST_NAME+"/mobile/recommendation/"+ofToString(iter->itemID);
                cout << url << endl;
                queue[ofxLoadURLAsync(ofxHttpRequest(url,cookies))] = REQUEST_TYPE_RECOMMENDATION;
                
                break;
            }
            
        }
        
        
    }
    
}

//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs& args){
//    cout << "touchCancelled" << endl;
}

