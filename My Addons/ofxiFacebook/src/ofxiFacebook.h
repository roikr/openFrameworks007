//
//  ofxiFacebook.h
//  facebookExample
//
//  Created by Roee Kremer on 7/27/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
#import <FacebookSDK/FacebookSDK.h>

enum {
    FACEBOOK_ERROR,
    FACEBOOK_TOKEN_EXIST,
    FACEBOOK_LOGGED_IN,
    FACEBOOK_IMAGE_POSTED
};

class ofxFBEventArgs {
public:    
    string message; 
    int status;
};



extern ofEvent<ofxFBEventArgs> ofxFacebookEvent;

template<class T>
void ofxRegisterFacebookNotification(T * obj){
	ofAddListener(ofxFacebookEvent,obj,&T::facebookEvent);
}


template<class T>
void ofxUnregisterFacebookNotification(T * obj){
	ofRemoveListener(ofxFacebookEvent,obj,&T::facebookEvent);
}

class ofxiFacebook {
public:
    void setup(bool bSSO,vector<string> permissions=vector<string>());
    void gotFocus();
    void launchedWithURL(string url);
       
    void login();
    void logout();
    void postImage(ofImage &image);
    
    bool getIsLoggedIn();
    string getAccessToken();
    
    void exit();
    
    
      
private:
    FBSession *session;
    vector<string> permissions;
    bool bSSO;
};