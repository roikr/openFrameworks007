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

class ofxiFacebook {
public:
    void setup();
    void gotFocus();
    void launchedWithURL(string url);
       
    void login();
    void logout();
    
    bool getIsLoggedIn();
    string getAccessToken();
    
    void exit();
    
    
      
private:
    FBSession *session;
};