//
//  ofxiMail.h
//  beenThere
//
//  Created by Roee Kremer on 8/19/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"

enum {
    OFXIMAIL_SENDING_CANCELED,
    OFXIMAIL_SAVED,
    OFXIMAIL_SEND,
    OFXIMAIL_FAILED,
    OFXIMAIL_NOT_SEND
};


extern ofEvent<int> ofxiMailEvent;

template<class T>
void ofxRegisterMailNotification(T * obj){
	ofAddListener(ofxiMailEvent,obj,&T::mailComposer);
}

template<class T>
void ofxUnregisterMailNotification(T * obj){
	ofRemoveListener(ofxiMailEvent,obj,&T::mailComposer);
}

@class MyDelegateObject;

struct attachmentStruct {
    attachmentStruct(ofBuffer &data,string mimetype,string filename):data(data),mimetype(mimetype),filename(filename) {};
    ofBuffer data;
    string mimetype;
    string filename;
};

struct mailStruct {
    string subject;
    string body;
    vector<string> toRecipients;
    vector<string> ccRecipients;
    vector<string> bccRecipients;
    vector<attachmentStruct> attachments;
    
};

class ofxiMail {
public:
    void setup();
    void exit();
    bool getCanSendMail();
    void sendMail(mailStruct mail);
//    void composerFinished(int result);
    
    
private:
    MyDelegateObject *delegate;
    
};
