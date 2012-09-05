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
    FACEBOOK_ACTION_RETRIEVE_SESSION,
    FACEBOOK_ACTION_LOGIN,
    FACEBOOK_ACTION_LOGOUT,
    FACEBOOK_ACTION_POST_IMAGE,
    FACEBOOK_ACTION_GET_ME
};

enum {
    FACEBOOK_FAILED,
    FACEBOOK_SUCEEDED
};

class ofxFBEventArgs {
public:    
    string message; 
    int action;
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
    ofxiFacebook():session(nil) {};
    void setup(vector<string> permissions=vector<string>());
    void exit();
    void gotFocus();
    void launchedWithURL(string url);
       
    void login(vector<string> permissions=vector<string>());
    void logout();
    void postImage(ofImage &image);
    void getMe();
    
    bool getIsLoggedIn();
    string getAccessToken();
    
   
      
private:
    FBSession *session;
};