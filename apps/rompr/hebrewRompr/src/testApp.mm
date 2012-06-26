#include "testApp.h"
#include "ofxURLFileLoader.h"
#include "ofxJSONElement.h"

//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	//If you want a landscape oreintation 
	//iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
	ofBackground(127,127,127);
    
    string url = "http://107.21.224.181/mobile/start/549453367/AAACtapZAgifcBAEGBY8KYPyxlFMSZAjdhE3jd2EEQSLF9W3RnaiPYuuFiIhOW1sj3B94d01SJJlUzIPGCyE8JkBLWMEpRxxnkM2M2TXAZDZD";
    
    ofxHttpResponse response = ofxLoadURL(url);
    
    //    cout << response.data << endl;
    
    //    for (vector<pair<string, string> >::iterator iter=response.cookies.begin();iter!=response.cookies.end();iter++) {
    //        cout << iter->first << ": " << iter->second << endl;
    //    }
    
//    string recommendationsUrl = "http://107.21.224.181/recommendations/json/1";
//    string recommendationsUrl = "http://107.21.224.181/mobile/recommendations/json/1";
    string recommendationsUrl = "http://107.21.224.181/mobile/recommendations/circle/32.081725/34.772147/1";
    
    ofxHttpRequest recommendationsRequest(recommendationsUrl, response.cookies);
    
    ofxHttpResponse recommendationsResponse = ofxLoadURL(recommendationsRequest);
    
    
	
    
    ofxJSONElement result;
    
	// Now parse the JSON
	bool parsingSuccessful = result.parse(recommendationsResponse.data.getText());
	if ( parsingSuccessful )
    {
		cout << result.getRawString() << endl;
        
        ofxJSONElement elements = result["user_recommendation_list"];
        
        
        
        for(int i=0; i<elements.size(); i++)
        {
            ofxJSONElement element = elements[i]["recommendation"];
            //cout << element["title"].asString().c_str()<< endl;
            UIAlertView *view = [[UIAlertView alloc] initWithTitle:[NSString stringWithCString:element["title"].asString().c_str() encoding:NSUTF8StringEncoding] message:@"test" delegate:nil cancelButtonTitle:@"cancel" otherButtonTitles:nil];
            [view show];
            [view release];
            //cout << element["title"].asString().c_str() << endl;
            
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

}

//--------------------------------------------------------------
void testApp::draw(){
	
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

