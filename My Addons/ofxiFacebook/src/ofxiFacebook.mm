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
#import <FacebookSDK/FBSessionTokenCachingStrategy.h>

ofEvent<ofxFBEventArgs> ofxFacebookEvent;

NSMutableArray *convertPermissions(vector<string> permissions) {
    NSMutableArray *perms = [NSMutableArray arrayWithCapacity:permissions.size()];
    for (vector<string>::iterator iter=permissions.begin();iter!=permissions.end();iter++) {
        [perms addObject:ofxStringToNSString(*iter)];
        cout << *iter << endl;
    }
    return perms;
}

void ofxiFacebook::setup(vector<string> permissions) {
    
    if (session && session.state == FBSessionStateCreatedOpening) {
        [session close];
        [session release];
        session = nil;
    }

    
    // @"publish_actions", @"user_photos"
    session = [[[FBSession alloc] initWithPermissions:convertPermissions(permissions)] retain];
    
    // if we don't have a cached token, a call to open here would cause UX for login to
    // occur; we don't want that to happen unless the user clicks the login button, and so
    // we check here to make sure we have a token before calling open
    
    if (session.state == FBSessionStateCreatedTokenLoaded) {
        // even though we had a cached token, we need to login to make the session usable
        // roikr: i.e. login without ux
        [session openWithCompletionHandler:^(FBSession *session, 
                                                         FBSessionState status, 
                                                         NSError *error) {
            
            
            ofxFBEventArgs args;
            args.action = FACEBOOK_ACTION_RETRIEVE_SESSION;
            
            if (error) {
                args.status = FACEBOOK_FAILED;
                args.message = ofxNSStringToString([error description]);
            } else {
                args.status = FACEBOOK_SUCEEDED;
                args.message = "token already created, make session usable";
            }
            
            ofNotifyEvent(ofxFacebookEvent, args);
            
        }];
    } else {
//        if (session.state != FBSessionStateCreated) {
                
        // if the session isn't open, let's open it now and present the login UX to the user
        [session openWithCompletionHandler:^(FBSession *session, 
                                             FBSessionState status, 
                                             NSError *error) {
            ofxFBEventArgs args;
            args.action = FACEBOOK_ACTION_LOGIN;
            
            if (error) {
                args.status = FACEBOOK_FAILED;
                args.message = ofxNSStringToString([error description]);
            } else {
                args.status = FACEBOOK_SUCEEDED;
                args.message = "logged in";
            }
            
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
    if (session!=nil && session.state == FBSessionStateCreatedOpening) {
        // BUG: for the iOS 6 preview we comment this line out to compensate for a race-condition in our
        // state transition handling for integrated Facebook Login; production code should close a
        // session in the opening state on transition back to the application; this line will again be
        // active in the next production rev
        [session close]; // so we close our session and start over
        [session release];
        session = nil;
    }
}

void ofxiFacebook::launchedWithURL(string url) {
    cout << "ofxiFacebook::launchedWithURL: " << url << endl;
    [session handleOpenURL:[NSURL URLWithString:ofxStringToNSString(url)]];
}


void ofxiFacebook::login(vector<string> permissions) {
    if (session==nil || ![session isOpen]) {
        
        session = [[[FBSession alloc] initWithPermissions:convertPermissions(permissions)] retain];
        NSLog(@"session open: %i, state: %i",[session isOpen],session.state);
        [session openWithBehavior:FBSessionLoginBehaviorForcingWebView completionHandler:^(FBSession *session, 
                                                                                                    FBSessionState status, 
                                                                                                    NSError *error) {
            ofxFBEventArgs args;
            args.action = FACEBOOK_ACTION_LOGIN;
            
            if (error) {
                args.status = FACEBOOK_FAILED;
                args.message = ofxNSStringToString([error description]);
            } else {
                args.status = FACEBOOK_SUCEEDED;
                args.message = "logged in";
            }
            
            ofNotifyEvent(ofxFacebookEvent, args);
        }];
        
    }

}

void ofxiFacebook::logout() {
    if (session) {
        // if a user logs out explicitly, we delete any cached token information, and next
        // time they run the applicaiton they will be presented with log in UX again; most
        // users will simply close the app or switch away, without logging out; this will
        // cause the implicit cached-token login to occur on next launch of the application
        [session closeAndClearTokenInformation];
        [session release];
        session = nil;
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
        
        
        ofxFBEventArgs args;
        args.action = FACEBOOK_ACTION_POST_IMAGE;
        
        if (error) {
            args.status = FACEBOOK_FAILED;
            args.message = ofxNSStringToString([error description]);
        } else {
            args.status = FACEBOOK_SUCEEDED;
            NSDictionary *resultDict = (NSDictionary *)result;
            args.message = ofxNSStringToString([NSString stringWithFormat:@"Successfully posted.\nPost ID: %@", 
                                                [resultDict valueForKey:@"id"]]);
        }
        
        ofNotifyEvent(ofxFacebookEvent, args);
        
    }];
    
}

void ofxiFacebook::getMe() {
    FBRequest *me = [[FBRequest alloc] initWithSession:session
                                             graphPath:@"me"];
    [me startWithCompletionHandler:^(FBRequestConnection *connection, 
                                     // we expect a user as a result, and so are using FBGraphUser protocol
                                     // as our result type; in order to allow us to access first_name and
                                     // birthday with property syntax
                                     NSDictionary<FBGraphUser> *user,
                                     NSError *error) {
        
        ofxFBEventArgs args;
        args.action = FACEBOOK_ACTION_GET_ME;
        
        if (error) {
            args.status = FACEBOOK_FAILED;
            args.message = ofxNSStringToString([error description]);
        } else {
            args.status = FACEBOOK_SUCEEDED;
            args.message = ofxNSStringToString(user.id);
        }
        
        ofNotifyEvent(ofxFacebookEvent, args);
        
    }];
}


bool ofxiFacebook::getIsLoggedIn() {
    return session ? session.isOpen : false;
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
    if (session!=nil) {
        [session close];
        [session release];
    }
}

