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






void ofxiFacebook::login(vector<string> permissions) {
    

    [[FBSession activeSession] closeAndClearTokenInformation];
    
    NSLog(@"Logged out of facebook");
    NSHTTPCookie *cookie;
    NSHTTPCookieStorage *storage = [NSHTTPCookieStorage sharedHTTPCookieStorage];
    for (cookie in [storage cookies])
    {
        NSString* domainName = [cookie domain];
        NSRange domainRange = [domainName rangeOfString:@"facebook"];
        if(domainRange.length > 0)
        {
            [storage deleteCookie:cookie];
        }
    }
    
    
    [FBSession setActiveSession:[[FBSession alloc] initWithPermissions:convertPermissions(permissions)]];

    [[FBSession activeSession] openWithBehavior:FBSessionLoginBehaviorForcingWebView completionHandler:^(FBSession *session,FBSessionState status, NSError *error) {
     
        NSLog(@"login::completionHandler, open: %i, status: %i",[session isOpen],status);
        
        ofxFBEventArgs args;

        
        switch (status) {
                /*! One of two initial states indicating that no valid cached token was found */
            case FBSessionStateCreated:
                NSLog(@"FBSessionStateCreated");
                
                break;
                
                /*! One of two initial session states indicating that a cached token was loaded;
                 when a session is in this state, a call to open* will result in an open session,
                 without UX or app-switching*/
            case FBSessionStateCreatedTokenLoaded:
                NSLog(@"FBSessionStateCreatedTokenLoaded");
                break;
                
                /*! One of three pre-open session states indicating that an attempt to open the session
                 is underway*/
            case FBSessionStateCreatedOpening:
                NSLog(@"FBSessionStateCreatedOpening");
                break;
                
                /*! Open session state indicating user has logged in or a cached token is available */
            case FBSessionStateOpen:
                NSLog(@"FBSessionStateOpen");
                args.action = FACEBOOK_ACTION_LOGIN;
                args.status = FACEBOOK_SUCCEEDED;
                args.message = "logged in";
                ofNotifyEvent(ofxFacebookEvent, args);
                break;
                
                /*! Open session state indicating token has been extended */
            case FBSessionStateOpenTokenExtended:
                NSLog(@"FBSessionStateOpenTokenExtended");
                break;
                
                /*! Closed session state indicating that a login attempt failed */
            case FBSessionStateClosedLoginFailed: // NSError obj w/more info
                NSLog(@"FBSessionStateClosedLoginFailed");
                args.action = FACEBOOK_ACTION_LOGIN;
                args.status = FACEBOOK_FAILED;
                args.message = ofxNSStringToString([error description]);
                ofNotifyEvent(ofxFacebookEvent, args);
                break;
                
                /*! Closed session state indicating that the session was closed, but the users token
                 remains cached on the device for later use */
            case FBSessionStateClosed:
                NSLog(@"FBSessionStateClosed");
                args.action = FACEBOOK_ACTION_LOGOUT;
                args.status = FACEBOOK_SUCCEEDED;
                args.message = "logged out";
                break;
                
            default:
                break;
        }
    }];
}



void ofxiFacebook::postImage(ofImage &image,string message) {
    
       
    NSString *path = [NSTemporaryDirectory() stringByAppendingPathComponent:@"temp.png"];
////    image.resize(200, 100);
//    
    image.saveImage(ofxNSStringToString(path));
    
    
    UIImage *img = [UIImage imageWithContentsOfFile:path];
//     UIImage *img = [UIImage imageNamed:@"Icon.png"];
    
//    UIImage *img =[UIImage imageWithData:[NSData dataWithBytes:image.getPixels() length:image.getPixelsRef().size()]];
    
    cout << img.size.width << " " << img.size.height << endl;
    
    NSString *graphPath = @"me/photos";
    NSMutableDictionary *parameters = [[NSMutableDictionary alloc] init];
    [parameters setObject:img forKey:@"picture"];
    NSString *str = [NSString stringWithCString:message.c_str() encoding:NSUTF8StringEncoding];;
    [parameters setObject:str forKey:@"message"];
    
    FBRequest *photoUploadRequest = [[[FBRequest alloc] initWithSession:[FBSession activeSession]
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
        
        NSLog(@"postImage::startWithCompletionHandler");
        
        ofxFBEventArgs args;
        args.action = FACEBOOK_ACTION_POST_IMAGE;
        
        if (error) {
            args.status = FACEBOOK_FAILED;
            args.message = ofxNSStringToString([error description]);
        } else {
            args.status = FACEBOOK_SUCCEEDED;
            NSDictionary *resultDict = (NSDictionary *)result;
            args.message = ofxNSStringToString([NSString stringWithFormat:@"Successfully posted.\nPost ID: %@", 
                                                [resultDict valueForKey:@"id"]]);
        }
        
        ofNotifyEvent(ofxFacebookEvent, args);
        
    }];
    
}

void ofxiFacebook::getMe() {
    FBRequest *me = [[FBRequest alloc] initWithSession:[FBSession activeSession]
                                             graphPath:@"me"];
    [me startWithCompletionHandler:^(FBRequestConnection *connection, 
                                     // we expect a user as a result, and so are using FBGraphUser protocol
                                     // as our result type; in order to allow us to access first_name and
                                     // birthday with property syntax
                                     NSDictionary<FBGraphUser> *user,
                                     NSError *error) {
        
        NSLog(@"getMe::startWithCompletionHandler");
        
        ofxFBEventArgs args;
        args.action = FACEBOOK_ACTION_GET_ME;
        
        if (error) {
            args.status = FACEBOOK_FAILED;
            args.message = ofxNSStringToString([error description]);
        } else {
            args.status = FACEBOOK_SUCCEEDED;
            args.message = ofxNSStringToString(user.id);
        }
        
        ofNotifyEvent(ofxFacebookEvent, args);
        
    }];
}


bool ofxiFacebook::getIsLoggedIn() {
    return [[FBSession activeSession] isOpen];
}

string ofxiFacebook::getAccessToken() {
    return  ofxNSStringToString([[FBSession activeSession] accessToken]);
}

void ofxiFacebook::logout() {
    [[FBSession activeSession] closeAndClearTokenInformation];
}

