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

ofEvent<ofxFBEventArgs> ofxFacebookEvent;

void ofxiFacebook::setup(vector<string> permissions) {
    
    // create a fresh session object
    if (permissions.empty()) {
        session = [[FBSession alloc] init];
    } else {
        // @"publish_actions", @"user_photos"
        
        NSMutableArray *perms = [NSMutableArray arrayWithCapacity:permissions.size()];
        for (vector<string>::iterator iter=permissions.begin();iter!=permissions.end();iter++) {
            [perms addObject:ofxStringToNSString(*iter)];
            cout << *iter << endl;
        }
        
        session = [[[FBSession alloc] initWithPermissions:perms] retain];
    }
        

    
    // if we don't have a cached token, a call to open here would cause UX for login to
    // occur; we don't want that to happen unless the user clicks the login button, and so
    // we check here to make sure we have a token before calling open
    
    if (session.state == FBSessionStateCreatedTokenLoaded) {
        // even though we had a cached token, we need to login to make the session usable
        [session openWithCompletionHandler:^(FBSession *session, 
                                                         FBSessionState status, 
                                                         NSError *error) {
            // we recurse here, in order to update buttons and labels
            ofxFBEventArgs args;
            args.message = "token already created, make session usable";
            ofNotifyEvent(ofxFacebookEvent, args);
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
            ofxFBEventArgs args;
            args.message = "logged in";
            ofNotifyEvent(ofxFacebookEvent, args);

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

void ofxiFacebook::postImage(ofImage &image) {
    
       
    NSString *path = [NSTemporaryDirectory() stringByAppendingPathComponent:@"temp.png"];
//    image.resize(200, 100);
    
    image.saveImage(ofxNSStringToString(path));
    
    UIImage *img = [UIImage imageWithContentsOfFile:path];
//     UIImage *img = [UIImage imageNamed:@"Icon.png"];
    
    cout << img.size.width << " " << img.size.height << endl;
    
    NSString *graphPath = @"me/photos";
    NSMutableDictionary *parameters = [[NSMutableDictionary alloc] init];
    [parameters setObject:img forKey:@"picture"];
    
    FBRequest *photoUploadRequest = [[[FBRequest alloc] initWithSession:session
                                                   graphPath:graphPath
                                                  parameters:parameters
                                                  HTTPMethod:@"POST"]
                          autorelease];
    
    [parameters release];
    
    
   
//    // Build the request for uploading the photo
//    FBRequest *photoUploadRequest = [FBRequest requestForUploadPhoto:img];
//    
//   
    [photoUploadRequest startWithCompletionHandler:^(FBRequestConnection *connection, id result, NSError *error) { 
//        NSLog(@"%@",[connection.urlResponse.allHeaderFields description]);
        
        NSString *alertMsg;
        NSString *alertTitle;
        if (error) {
            alertMsg = error.description;
            alertTitle = @"Error";
        } else {
            NSDictionary *resultDict = (NSDictionary *)result;
            alertMsg = [NSString stringWithFormat:@"Successfully posted.\nPost ID: %@", 
                       [resultDict valueForKey:@"id"]];
            alertTitle = @"Success";
        }
        
        ofxFBEventArgs args;
        args.message = ofxNSStringToString(alertTitle)+"\n"+ofxNSStringToString(alertMsg);
        ofNotifyEvent(ofxFacebookEvent, args);
        
    }];
    
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
