#include "testApp.h"
#import <QuartzCore/QuartzCore.h>
#include "cJSON.h"
#import "EventRouteWindow.h"


#define HOME_LATITUDE		32.081725
#define HOME_LONGITUDE		34.772147

#define MAP_BLOCK_WIDTH     1500.0

#define SIMULTANEOUS_CONNECTIONS 5

#define MAP_RECT_WIDTH      mapKit.getMKMapView().visibleMapRect.size.width

const string HOST_NAME = "107.21.224.181";
const string USER_ID = "549453367";
const string FB_ACCESS_TOKEN = "AAACtapZAgifcBAG1nd96WupL2vU103yrSCsmUA9KX0ElPYISwqhYcAco7W3BSh8NSha0qkDvMjp3xfUE2W1AHA3apk2rmlIYHstZAMwQZDZD";

enum  {
    REQUEST_TYPE_LOGIN,
    REQUEST_TYPE_QUERY,
    REQUSET_TYPE_THUMB,
    REQUEST_TYPE_IMAGE,
    REQUEST_TYPE_RECOMMENDATION,
    REQUSET_TYPE_UPLOAD_IMAGE,
    REQUSET_TYPE_POST_RECOMMENDATION
    
};

enum {
    PENNER_BLACK
};

enum {
    STATE_MAP,
    STATE_CAMERA,

};


//--------------------------------------------------------------
void testApp::setup(){	
 
    [ofxiPhoneGetGLParentView() removeFromSuperview];
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
//	ofRegisterTouchEvents(this);
    ofxRegisterURLNotification(this);

	//ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
	ofEnableSmoothing();
    
    ofxiPhoneSetGLViewTransparent(true);
    ofxiPhoneSetGLViewUserInteraction(false);
    
    cout << "of: " << ofGetWidth() <<"\t" << ofGetHeight() << endl;
    
    mapKit.open();
    
    coreLocation.startLocation();
    // set initial position to specified coordinates, and cover 1000 Km x 1000 Km area
    userLocation = getUserLocation();
    cout << userLocation.latitude << "\t" << userLocation.longitude << endl;
//    mapKit.setRegionWithMeters(HOME_LATITUDE, HOME_LONGITUDE, 1000, 1000);
    bStartLocation = true;
    
    
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
	mapKit.setShowUserLocation(false);
	mapKit.setType(OFXIPHONE_MAPKIT_MAP); // OFXIPHONE_MAPKIT_HYRBID
    //	mapKit.setAllowZoom(false);
    //	mapKit.setAllowScroll(false);
    //	mapKit.setAllowUserInteraction(false);

//    [ofxiPhoneGetUIWindow() addSubview:[[MKMapView alloc] initWithFrame:[[UIScreen mainScreen] bounds]]];
//    [ofxiPhoneGetUIWindow() bringSubviewToFront:ofxiPhoneGetGLView()];
    ofxiPhoneSendGLViewToFront();
    
    bUpdatingRegion = false;
    bQueryLocation = false;
    
    logo.loadImage("logo.png");
    
    string url = "http://"+HOST_NAME+"/mobile/start/"+USER_ID+"/"+FB_ACCESS_TOKEN;
    queue[ofxLoadURLAsync(ofxHttpRequest(url,url))] = REQUEST_TYPE_LOGIN;
    
    bSelected = false;
    bRecommendation = false;
    
    state = STATE_MAP;
    bStartCamera = false;
    bStopCamera = false;
    cameraXform.setup(ofVec2f(ofGetWidth(),ofGetHeight())*0.5,0.01,0);

}


void testApp::queryViewLocation() {
//    ofxMapKitLocation location;
//    location.latitude = HOME_LATITUDE;
//    location.longitude = HOME_LONGITUDE;
//    int distance = 1;
//    queue[ofxLoadURLAsync(ofxHttpRequest("http://"+HOST_NAME+"/recommendations/json/1",response.cookies))] = REQUEST_TYPE_QUERY;
    
    ofxMapKitLocation location=mapKit.getCenterLocation();
    MKMapRect rect = mapKit.getMKMapView().visibleMapRect;
    double meters = MKMetersBetweenMapPoints(rect.origin, MKMapPointMake(MKMapRectGetMaxX(rect), MKMapRectGetMaxY(rect)))/2;
    

    
    int distance = round(meters/1000.0);
    string url = "http://"+HOST_NAME+"/mobile/recommendations/circle/"+ofToString(location.latitude,6)+"/"+ofToString(location.longitude,6)+"/"+ofToString(MAX(1.0,distance)); // roikr: minimum distance for query
//    cout << "viewDiagonal: " << meters << "\t" << distance << "\t" << url << endl;;
    
    
    // remove previous unloaded queries from queue
    map<int,int>::iterator iter= queue.begin();
    while (iter!=queue.end()) {
        if (iter->second == REQUEST_TYPE_QUERY) {
            queue.erase(iter);
            iter = queue.begin();
        } else {
            iter++;
        }
    }
    
    queue[ofxLoadURLAsync(ofxHttpRequest(url,cookies))] = REQUEST_TYPE_QUERY;
}



void testApp::processQuery(ofBuffer &query) {
    cJSON *result;
    result=cJSON_Parse(query.getText().c_str());
	if ( result )
    {
//		cout << result.getRawString() << endl;
        
        cJSON *elements = cJSON_GetObjectItem(result, "user_recommendation_list");
        
        vector<item> newItems;
        
        int j = 0;
        for(int i=0; i<cJSON_GetArraySize(elements); i++)
        {
            cJSON *element = cJSON_GetObjectItem(cJSON_GetArrayItem(elements, i),"recommendation");
            cJSON *r_id = cJSON_GetObjectItem(element,"id");
            cJSON *longitude = cJSON_GetObjectItem(element,"longitude");
            cJSON *latitude = cJSON_GetObjectItem(element,"latitude");
            cJSON *img_path = cJSON_GetObjectItem(element,"thumb_path");
            
//            cout << i << "\t" << r_id->type << "\t" << longitude->type << "\t" << latitude->type << "\t" << img_path->type << endl;
//             cout << i << "\t" << r_id->valuestring << "\t" << longitude->valuestring << "\t" << latitude->valuestring << "\t" << img_path->valuestring << endl;
//            if (r_id->type==cJSON_Number && longitude->type==cJSON_Number && latitude->type == cJSON_Number && img_path->type == cJSON_String ) {
//                cout << i << "\t" << r_id->string << "\t" << longitude->string << "\t" << latitude->string << "\t" << img_path->string << endl;
//            }
            
           
        
            
                
            
            item r;
            r.img_path = img_path->valuestring;
            r.location.longitude = longitude->type == cJSON_Number ? longitude->valuedouble : atof(longitude->valuestring);
            r.location.latitude = latitude->type == cJSON_Number ? latitude->valuedouble : atof(latitude->valuestring);
            MKMapPoint mapPoint = MKMapPointForCoordinate(r.location);
            r.gridPos = ofVec2f(floor(mapPoint.x/MAP_BLOCK_WIDTH),floor(mapPoint.y/MAP_BLOCK_WIDTH));
//            cout << r.gridPos << endl;
            r.itemID = atoi(r_id->valuestring);
            if (r.location.longitude && r.location.latitude && !r.img_path.empty()) {
//                cout << i << "\t" << j << "\t" << r.itemID << "\t" << r.location.longitude << "\t" << r.location.latitude << "\t" << r.img_path << endl;
//                    cout << j << MKMetersBetweenMapPoints(MKMapPointForCoordinate(mapKit.getCenterLocation()),MKMapPointForCoordinate(r.location)) << endl;
                newItems.push_back(r);
                
//                    imagesList.push_back(r.img_path);
                j++;
            }
                
            
        }
        
        cJSON_Delete(result);
        updateItems(newItems);
	}
    else
    {
		cout  << "Failed to parse JSON" << endl;
	}

}

list<item>::iterator testApp::findItem(int itemID) {
    
    for (list<item>::iterator iter=items.begin(); iter!=items.end(); iter++) {
        if (itemID == iter->itemID) {
            return iter;
        }
    }
    
    return items.end();
}

class compare { // simple comparison function
public:
    bool operator()(const ofVec2f a,const ofVec2f b) const { return (a.y - b.y)<0 ? true : (a.x - b.x)<0; } // returns a<b
};

void testApp::updateItems(vector<item> newItems) {
//    for (list<item>::iterator iter = items.begin();iter!=items.end();iter++) {
//        if (MKMetersBetweenMapPoints(MKMapPointForCoordinate(mapKit.getCenterLocation()),MKMapPointForCoordinate(iter->location))> 2*meters) {
//            iter = items.erase(iter);
//        }
//    }
    
    map <ofVec2f,int,compare > grid;
    
   
    vector<item>::iterator niter;
    list<item>::iterator iter;
    
     // pass 1: remove items from list which are not appear in the new items vector
    
    int removed=0;
    
    iter=items.begin();
    while (iter!=items.end()) {
        
        for (niter=newItems.begin(); niter!=newItems.end(); niter++) {
            if (niter->itemID == iter->itemID) {
                break;
            }
        }
        if (niter==newItems.end()) {
            iter = items.erase(iter);
            removed++;
        } else {
            iter++;
        }
    }
    
    // pass 2: add new items in the vector to the list
    
    int added=0;
    for (niter=newItems.begin(); niter!=newItems.end(); niter++) {
        for (iter=items.begin(); iter!=items.end(); iter++) {
            if (niter->itemID == iter->itemID) {
                break;
            }
        }
        if (iter==items.end()) {
            items.push_back(*niter);
            added++;
        }
    }
    
    cout << "updateItems: " << newItems.size() << " parsed, " << removed <<  " items removed, " << added << " items added, " << items.size() << " totla " << endl ;
    

    if (added || removed) {
        // pass 3: add items without downloaded images and which are not in the queue to downloads
        grid.clear();
        downloads.clear();
        for (iter=items.begin(); iter!=items.end(); iter++) {
            if (!iter->image.bAllocated() && !iter->bQueued) {
                downloads.push_back(iter->itemID);
            }
            
            if (grid.count(iter->gridPos)) {
                grid[iter->gridPos]++;
            } else {
                grid[iter->gridPos] = 1;
            }
            
        }
        
        for (map<ofVec2f,int>::iterator miter=grid.begin();miter!=grid.end();miter++) {
            cout << miter->first << "\t" << miter->second  << endl;
            
            
            int i=0;
            for (iter=items.begin(); iter!=items.end(); iter++) {
                
                if (iter->gridPos == miter->first) {
                    iter->count = miter->second;
                    iter->angle = i*360/miter->second;
                    i++;
                }
            }
            
        }
        
        for (iter=items.begin(); iter!=items.end(); iter++) {
            cout << iter->count << "\t" << iter->angle <<  endl;
        }
        

    }
        
 

}



void testApp::calcItems() {
    float blockWidth =  MAP_BLOCK_WIDTH / MAP_RECT_WIDTH * ofGetWidth();
    MKMapRect mapRect = mapKit.getMKMapView().visibleMapRect;
    
    for (list<item>::iterator iter=items.begin(); iter!=items.end(); iter++) {
        ofVec2f pos;
        
        
        float width = iter->image.bAllocated() ? iter->image.getWidth() : logo.getWidth();
        float height = iter->image.bAllocated() ? iter->image.getHeight() : logo.getHeight();
        float scale = blockWidth/max(width,height);
        
        if (iter->count>1) {
            scale *= 0.5 ;
            pos = iter->gridPos*MAP_BLOCK_WIDTH - ofVec2f(mapRect.origin.x,mapRect.origin.y);
            pos = ofVec2f(pos.x*ofGetWidth()/mapRect.size.width, pos.y*ofGetHeight()/mapRect.size.height)* [[UIScreen mainScreen] scale] +0.5*blockWidth*ofVec2f(cos(iter->angle*PI/180.0),sin(iter->angle*PI/180.0));
        } else {
            pos = getScreenCoordinatesForLocation(iter->location);
        }
        iter->rect.setFromCenter(pos, width*scale, height*scale);
    }
}

void testApp::showRecommendation(string html) {
    bRecommendation = true;
    UIView *view = [[UIView alloc] initWithFrame:CGRectMake(0, 0, ofGetWidth(), ofGetHeight())];
    view.opaque = NO;
	view.backgroundColor = [UIColor clearColor];
    view.alpha = 1.0f;
    UIView *blackView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, ofGetWidth(), ofGetHeight())];
    blackView.backgroundColor = [UIColor whiteColor];
    blackView.alpha = 0.0;
    //black.opaque = YES;
    
    UIWebView *webView = [[UIWebView alloc] initWithFrame:CGRectMake(0, -ofGetHeight(), ofGetWidth(), ofGetHeight())];
    
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
    
//    cout << html << endl;
	
    NSString *htmlString = [NSString stringWithCString:html.c_str() encoding:NSUTF8StringEncoding];
    
	// to make html content transparent to its parent view -
	// 1) set the webview's backgroundColor property to [UIColor clearColor]
	// 2) use the content in the html: <body style="background-color: transparent">
	// 3) opaque property set to NO
	//

    [webView loadHTMLString:htmlString baseURL:[NSURL URLWithString:ofxStringToNSString("http://"+HOST_NAME)]];
//	[htmlString release];
    
    [ofxiPhoneGetUIWindow() addSubview:view];
     
    dispatch_async(dispatch_get_main_queue(), ^{
        [UIView animateWithDuration:1.0 delay:0.0 options: UIViewAnimationOptionTransitionNone | UIViewAnimationOptionCurveEaseOut 
                         animations:^{
                             blackView.alpha = 0.0;
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
                [UIView animateWithDuration:0.5 delay:0.0 options: UIViewAnimationOptionTransitionNone | UIViewAnimationCurveEaseInOut 
                                 animations:^{
                                     blackView.alpha = 0.0;
                                     CGRect frame = webView.frame;
                                     frame.origin.y = ofGetHeight();
                                     webView.frame = frame;
                                     
                                     
                                 } 
                                 completion:^(BOOL finished){ 
                                     [view removeFromSuperview];
                                     bRecommendation = false;
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
    
//    cout << "urlResponse: " << url <<", status: " << status << endl;
    
    
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
                        while (riter!=items.end() && riter->img_path != url ) {
                            riter++;
                        }
                        
                        if (riter!=items.end()) {
                            ofImage &image = riter->image; 
                            image.setUseTexture(false);
                            image.loadImage(response.data);
//                            cout << "imageLoaded:" << url << "\t" << image.getWidth() << "\t" << image.getHeight() << "\tid: " << riter->itemID << endl;
                            
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
                        
                    case REQUEST_TYPE_RECOMMENDATION: {
                        int itemID = atoi((url.substr(url.find_last_of('/')+1)).c_str());
                        if (bSelected && !bRecommendation && itemID == selectedID ) {
                            showRecommendation(response.data.getText());
                        }
                    } break;
                        
                    case REQUSET_TYPE_UPLOAD_IMAGE: {
                        string image_path;
                        cJSON *result = cJSON_Parse(response.data.getText().c_str());
                        if ( result )
                        {
                            cJSON *path = cJSON_GetObjectItem(result,"image_path");
                            image_path = path->valuestring;
                            
                        }
                        
//                        cout << "image_path: " << image_path << endl;
                        vector <pair<string,string> > nvc;
                        std::map<string,string> files;
                        nvc.push_back(make_pair("mode", "new"));
                        nvc.push_back(make_pair("id", "0"));
                        nvc.push_back(make_pair("title", "mobile test"));
                        nvc.push_back(make_pair("free_text", "testing mobile camera"));
                        nvc.push_back(make_pair("creation_date", ""));
                        nvc.push_back(make_pair("modification_date", ""));
                        nvc.push_back(make_pair("category", "1"));
                        nvc.push_back(make_pair("age_from", ""));
                        nvc.push_back(make_pair("age_to", ""));
                        nvc.push_back(make_pair("location", "mobile_location"));
                        nvc.push_back(make_pair("location_longitude",ofToString(imageLocation.longitude,15)));
                        nvc.push_back(make_pair("location_latitude", ofToString(imageLocation.latitude,15)));
                        nvc.push_back(make_pair("location_accuracy", "true"));
                        nvc.push_back(make_pair("uploaded_image_path", image_path));
                        
                        
                        queue[ofxLoadURLAsync(ofxHttpRequest("http://"+HOST_NAME+"/recommendation/post", nvc, files,cookies))] = REQUSET_TYPE_POST_RECOMMENDATION;
                        
                    } break;
                        
                    case REQUSET_TYPE_POST_RECOMMENDATION:
                        image.clear();
                        bQueryLocation = true;
                        cout << "posted " << endl;
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

ofVec2f testApp::getScreenCoordinatesForLocation(ofxMapKitLocation location) {
    CGPoint cgPoint = [mapKit.getMKMapView() convertCoordinate:location toPointToView:nil];
    
	return ofVec2f(cgPoint.x, cgPoint.y) *[[UIScreen mainScreen] scale] ;
}

ofxMapKitLocation testApp::getUserLocation() {
//    return mapKit.getMKMapView().userLocation.location.coordinate;
    
    return CLLocationCoordinate2DMake(coreLocation.getLatitude(),coreLocation.getLongitude());
}

//--------------------------------------------------------------
void testApp::update(){
	if (bUpdatingRegion) {
//        cout << mapKit.getScreenCoordinatesForLocation(HOME_LATITUDE, HOME_LONGITUDE) << endl;
                
    }
    
    if (bStartLocation) {
        
        if (userLocation.latitude!=getUserLocation().latitude || userLocation.longitude!=getUserLocation().longitude) {
        
            bStartLocation = false;
            mapKit.setRegionWithMeters(getUserLocation().latitude, getUserLocation().longitude, 1000, 1000);
        }
    }
   
    if (queue.size()<SIMULTANEOUS_CONNECTIONS) {
        
        if (bQueryLocation && !cookies.empty()) {
            bQueryLocation = false;
            queryViewLocation();
            
        }
        
        vector<int>::iterator diter = downloads.begin();
        
        while (diter!=downloads.end() && queue.size()<SIMULTANEOUS_CONNECTIONS) {
            
            list<item>::iterator iter;
            for (iter=items.begin();iter!=items.end();iter++) {
                if (*diter == iter->itemID) {
                    break;
                }
            }
            
            if (iter!=items.end()) {
                queue[ofxLoadURLAsync(iter->img_path)] = REQUEST_TYPE_IMAGE;
                iter->bQueued = true;
                
            } else {
                cout << "update: would not download itemID: " << *diter << endl;
                
            }
            
            diter = downloads.erase(diter);
            
        }
    }
    
    if (bSelected) {
        imageXform.update();
        
        if (bDeselect && !imageXform.getIsEasing()) {
            bDeselect = false;
            bSelected = false;
        }
    }
    
    
    
    cam.update();
    
    if (bStartCamera) {
        cameraXform.update();
        if (!cameraXform.getIsEasing()) {
            bStartCamera = false;
            cam.preview();
            ofxRegisterVolumeButtonsNotification(this);
            volumeButtons.start();
        }
    }
    
    if (bStopCamera) {
        cameraXform.update();
        if (!cameraXform.getIsEasing()) {
            cam.stop();
            bStopCamera = false;
        }
    } 
    
    calcItems(); 
    penner.update();

}


//--------------------------------------------------------------
void testApp::draw(){	
    
	glClearColor(0,0,0,0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    

//    nearestID = min_element(items.begin(), items.end(), compare_items) ->itemID;

    
    ofFill();
    
    
    
    ofVec2f center = 0.5*ofVec2f(ofGetWidth(),ofGetHeight());
    float blockWidth =  MAP_BLOCK_WIDTH / MAP_RECT_WIDTH * ofGetWidth();

    ofSetHexColor(0xFFFFFF);
    list<item>::iterator nearest = items.begin();
    
    for (list<item>::iterator iter = items.begin();iter!=items.end();iter++) {
        if ((iter->rect.getCenter()-center).squareLength()<(nearest->rect.getCenter()-center).squareLength()) {
            nearest = iter;
        }
        if (!bSelected || iter->itemID!=selectedID) {
           if (iter->image.bAllocated()) {
                iter->image.draw(iter->rect);
            } else {
              
                ofEnableAlphaBlending();
                logo.draw(iter->rect);
                ofDisableAlphaBlending();
            }
        } 
        
    }

    
    ofSetHexColor(0xFFFFFF);
    if (nearest!=items.end() && (!bSelected  || nearest->itemID!=selectedID)) {
        if (nearest->image.bAllocated()) {
            nearest->image.draw(nearest->rect);
        } else {
            ofEnableAlphaBlending();
            logo.draw(nearest->rect);
            ofDisableAlphaBlending();
        }
    }
    
        
    
           
    if (image.isAllocated()) {
        ofPushMatrix();
        ofTranslate(getScreenCoordinatesForLocation(imageLocation));
        
        float scale = blockWidth/max(image.getWidth(),image.getHeight());
        ofScale(scale, scale);
        ofTranslate(-0.5*ofPoint(image.getWidth(),image.getHeight()));
        float margin = 10;
        ofSetHexColor(0xc9dfaf);
        ofRect(-margin, -margin, image.getWidth()+2*margin, image.getHeight()+2*margin);
        ofSetHexColor(0xFFFFFF);
        image.draw(0, 0);
        
        ofPopMatrix();
    } 
    
    
    
    ofPushMatrix();
    ofTranslate(getScreenCoordinatesForLocation(getUserLocation()));
    
    float scale = 0.25*blockWidth/max(logo.getWidth(),logo.getHeight());
    ofScale(scale, scale);
    ofTranslate(-0.5*ofPoint(logo.getWidth(),logo.getHeight()));
    ofSetHexColor(0xFFFFFF);
    ofEnableAlphaBlending();
    logo.draw(0, 0);
    ofDisableAlphaBlending();
    
    ofPopMatrix();
    
    if (bSelected) {
        ofSetColor(0, 0,0,150);
        ofEnableAlphaBlending();
        ofRect(0, penner.getParam(PENNER_BLACK), ofGetWidth(), ofGetHeight());
        ofDisableAlphaBlending();
        ofSetColor(255);
        list<item>::iterator iter = findItem(selectedID);
        if (iter!=items.end()) {
            imageXform.begin();
            ofTranslate(ofVec2f(iter->image.getWidth(), iter->image.getHeight())*-0.5);
            iter->image.draw(0, 0);
            imageXform.end();
            
            
        }
        
    }

    
    if (bStartCamera || cam.getIsPlaying()) {
    
        cameraXform.begin();

        
        
        ofEnableAlphaBlending();
        ofSetColor(0, 0, 0,200);
        
        ofRect(-ofGetHeight()/2,-ofGetWidth()/2,ofGetHeight(),ofGetWidth());
        ofDisableAlphaBlending();
        
        ofSetColor(255, 255, 255,255);
        
        if (cam.getIsFrameVisible()) {
            float width = ofGetHeight(); 
            float height = width/cam.getWidth()*cam.getHeight() ;
            cam.draw(ofRectangle(-width/2,-height/2,width,height), ofRectangle(0,0,1,1));
        } 
        
        cameraXform.end();
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
    cout << "touchDown" << endl;
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
    cout << "touchDoubleTap" << endl;
    float blockWidth =  MAP_BLOCK_WIDTH / MAP_RECT_WIDTH * ofGetWidth();
    
    switch (state) {
        case STATE_MAP: 
            if (bSelected) {
                if (!bDeselect) {
                    bDeselect = true;
                    list<item>::iterator iter = findItem(selectedID);
                    if (iter!=items.end()) {
                        float scale = blockWidth/max(iter->image.getWidth(),iter->image.getHeight())*(iter->count>1 ? 0.5 : 1.0);
                        imageXform.start(EASE_OUT_QUAD,iter->rect.getCenter(),scale);
                        penner.start(PENNER_BLACK,EASE_OUT_QUAD, 0,ofGetHeight());
                    }
                    
                    hideRecommendation();
                }
                
            } else {
                bDeselect = false;
                list<item>::iterator iter;
                for (iter = items.begin();iter!=items.end();iter++) {

                    
                    if (iter->rect.inside(ofVec2f(touch.x,touch.y))) {
                        //                cout << "found: " << iter->itemID << endl;
                        bSelected = true;
                        selectedID = iter->itemID;
                       
                        float targetScl = MIN(ofGetWidth()/iter->image.getWidth(),ofGetHeight()/iter->image.getHeight());
                        float scale = blockWidth/max(iter->image.getWidth(),iter->image.getHeight())*(iter->count>1 ? 0.5 : 1.0);
                        imageXform.setup(iter->rect.getCenter(),scale);
                        imageXform.start(EASE_OUT_QUAD, ofVec2f(ofGetWidth(),ofGetHeight())*0.5,targetScl);
                        penner.start(PENNER_BLACK, EASE_OUT_QUAD, ofGetHeight(), 0);
                        
                        string url = "http://"+HOST_NAME+"/mobile/recommendation/"+ofToString(iter->itemID);
                        cout << url << endl;
                        queue[ofxLoadURLAsync(ofxHttpRequest(url,cookies))] = REQUEST_TYPE_RECOMMENDATION;
                        
                        break;
                    }
                    
                }
                
                if (iter ==items.end()) {
                    
                    bStartCamera = true;
                    ofVec2f pos();
                    
                    cameraXform.start(EASE_OUT_QUAD, ofVec2f(ofGetWidth(),ofGetHeight())*0.5,1.0,90);
                    state = STATE_CAMERA;
                    mapKit.setRegionWithMeters(getUserLocation().latitude, getUserLocation().longitude, 1000, 1000);
                }
                
                
            }
            break;
            
        case STATE_CAMERA: {
            
            cameraXform.start(EASE_OUT_QUAD, ofVec2f(ofGetWidth(),ofGetHeight()),0.01,0);
            ofxUnregisterVolumeButtonsNotification(this);
            volumeButtons.stop();
            
            bStopCamera = true;
            state = STATE_MAP;
        }   break;
        default:
            
            break;
    }
}

void testApp::volumeButtonPressed(int &button) {
    ofxRegisterStillCameraNotification(this);
    cam.snap();
}

void testApp::pictureTaken(ofImage &image) {
    ofxUnregisterStillCameraNotification(this);
//    cout << image.getWidth() << "\t" << image.getHeight() << endl;
    
    image.saveImage(ofxNSStringToString([NSString stringWithFormat:@"%@%@", NSTemporaryDirectory(), @"output.jpg"]));
    imageLocation = getUserLocation();
    this->image = image;
 
    cameraXform.start(EASE_OUT_QUAD, ofVec2f(ofGetWidth(),ofGetHeight()),0.01,0);
    ofxUnregisterVolumeButtonsNotification(this);
    volumeButtons.stop();
    bStopCamera = true;
    state = STATE_MAP;

    
    vector <pair<string,string> > nvc;
    std::map<string,string> files;
    
    files["qqfile"]=ofxNSStringToString([NSString stringWithFormat:@"%@%@", NSTemporaryDirectory(), @"output.jpg"]);
    
    queue[ofxLoadURLAsync(ofxHttpRequest("http://"+HOST_NAME+"/ajax/upload_image", nvc, files, cookies))] = REQUSET_TYPE_UPLOAD_IMAGE;   
    
}

//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs& args){
//    cout << "touchCancelled" << endl;
}

