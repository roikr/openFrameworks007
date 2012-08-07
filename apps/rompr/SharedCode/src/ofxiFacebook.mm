//
//  ofxiFacebook.cpp
//  facebookExample
//
//  Created by Roee Kremer on 7/27/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxiFacebook.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"

void ofxiFacebook::setup() {
    
    // create a fresh session object
    session = [[[FBSession alloc] init] retain];
    
    // if we don't have a cached token, a call to open here would cause UX for login to
    // occur; we don't want that to happen unless the user clicks the login button, and so
    // we check here to make sure we have a token before calling open
    
    if (session.state == FBSessionStateCreatedTokenLoaded) {
        // even though we had a cached token, we need to login to make the session usable
        [session openWithCompletionHandler:^(FBSession *session, 
                                                         FBSessionState status, 
                                                         NSError *error) {
            // we recurse here, in order to update buttons and labels
            cout << "token already created, make session usable" << endl;
        }];
    }
    
}


void ofxiFacebook::gotFocus() {
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
    
    // FBSample logic
    // this means the user switched back to this app without completing a login in Safari/Facebook App
    if (session.state == FBSessionStateCreatedOpening) {
        // BUG: for the iOS 6 preview we comment this line out to compensate for a race-condition in our
        // state transition handling for integrated Facebook Login; production code should close a
        // session in the opening state on transition back to the application; this line will again be
        // active in the next production rev
        //[self.session close]; // so we close our session and start over
    }
}

void ofxiFacebook::launchedWithURL(string url) {
//    cout << "ofxiFacebook::launchedWithURL: " << url << endl;
    [session handleOpenURL:[NSURL URLWithString:ofxStringToNSString(url)]];
}


void ofxiFacebook::login() {
    if (!getIsLoggedIn()) {
        if (session.state != FBSessionStateCreated) {
            // Create a new, logged out session.
            [session release];
            session = [[[FBSession alloc] init] retain];
        }
        
        // if the session isn't open, let's open it now and present the login UX to the user
        [session openWithCompletionHandler:^(FBSession *session, 
                                                         FBSessionState status, 
                                                         NSError *error) {
            // and here we make sure to update our UX according to the new session state
            cout << "logged in" << endl;
        }];
    }

}

void ofxiFacebook::logout() {
    if (getIsLoggedIn()) {
        // if a user logs out explicitly, we delete any cached token information, and next
        // time they run the applicaiton they will be presented with log in UX again; most
        // users will simply close the app or switch away, without logging out; this will
        // cause the implicit cached-token login to occur on next launch of the application
        [session closeAndClearTokenInformation];
    }
}

bool ofxiFacebook::getIsLoggedIn() {
    return session.isOpen;
}

string ofxiFacebook::getAccessToken() {
    return getIsLoggedIn() ? ofxNSStringToString(session.accessToken) : "";
}

void ofxiFacebook::exit() {
    // FBSample logic
    // if the app is going away, we close the session if it is open
    // this is a good idea because things may be hanging off the session, that need 
    // releasing (completion block, etc.) and other components in the app may be awaiting
    // close notification in order to do cleanup
    [session close];
    [session release];
}

