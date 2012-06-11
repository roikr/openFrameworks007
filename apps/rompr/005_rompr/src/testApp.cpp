#include "testApp.h"
#include "ofxURLFileLoader.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	franklinBook.loadFont("frabk.ttf", 32);
	franklinBook.setLineHeight(36);


	string url = "http://107.21.224.181/mobile/start/549453367/AAACtapZAgifcBAEGBY8KYPyxlFMSZAjdhE3jd2EEQSLF9W3RnaiPYuuFiIhOW1sj3B94d01SJJlUzIPGCyE8JkBLWMEpRxxnkM2M2TXAZDZD";
    
    ofxHttpResponse response = ofxLoadURL(url);
    
    //cout << response.data << endl;
    
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
        
        ofxJSONElement recommendations = result["user_recommendation_list"];
        
        
        
        for(int i=0; i<recommendations.size(); i++)
        {
            ofxJSONElement recommendation = recommendations[i]["recommendation"];
            string filename = recommendation["img_path"].asString();
            cout << i << ": " << filename << endl;
            if (!filename.empty()) {
                
                ofImage image;
                images.push_back(image);
                
                ofxHttpRequest imageRequest(filename, filename, response.cookies);
                
                ofxHttpResponse imageResponse = ofxLoadURL(imageRequest);
                
                images.back().loadImage(imageResponse.data);
                //images.back().update();
                
                
                
                cout << images.back().getWidth() << "x" << images.back().getHeight() << endl;;
                
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

}

//--------------------------------------------------------------
void testApp::draw() {
	
	// See this page for all of the ways to access data in a Json::Value
	// http://jsoncpp.sourceforge.net/class_json_1_1_value.html	

//	ofSetHexColor(0x00FF00);
	
//	int n=0;
//	ofxJSONElement recommendations = result["user_recommendation_list"];
//    
//	for(int i=0; i<recommendations.size(); i++)
//	{
//		string message = recommendations[i]["title"].asString();
//		franklinBook.drawString(message, 10, 40*i+40);
//	}
    
    ofSetColor(255);
    
    
    ofPushMatrix();
    ofScale(0.2, 0.2);
    int x = 0;
    for (vector<ofImage>::iterator iter = images.begin(); iter!=images.end(); iter++) {
        iter->draw(x,0);
        x+=iter->getWidth();
    }
    ofPopMatrix();   
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){ 
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}
