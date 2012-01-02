//
//  ofxiPhoneGameKit.h
//  gameKitExample
//
//  Created by Roee Kremer on 12/1/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#import <Foundation/Foundation.h>
#import <GameKit/GameKit.h>


class ofxiPhoneGameKit;

@interface GameKitDelegate : NSObject<GKPeerPickerControllerDelegate,GKSessionDelegate> {
	NSInteger		gameState;
	GKSession		*gameSession;
	int				gameUniqueID;
	NSString		*gamePeerId;	
	
	ofxiPhoneGameKit *cppGameKit;
}

@property(nonatomic) NSInteger		gameState;
@property(nonatomic, retain) GKSession	 *gameSession;
@property(nonatomic, copy)	 NSString	 *gamePeerId;

- (void)invalidateSession:(GKSession *)session;
- (id)initWithGameKit:(ofxiPhoneGameKit *)gameKit;
- (void)startPicker;
- (void)sendData:(NSData *)data;
@end


#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"

class gkReceiveDataEventArgs : public ofEventArgs {
public:
	ofBuffer data;
	string peer;
	void *context;
};

extern ofEvent<gkReceiveDataEventArgs> gkReceiveDataEvent;

class ofxiPhoneGameKit {
	
public:
	ofxiPhoneGameKit();
	~ofxiPhoneGameKit();
	
	void setup();
	void update();
	void exit();
	
	void startPicker();
	void sendData(const ofBuffer &data);
	
//	void receiveData(ofBuffer &data,string peer,void *context);
	
protected:
	GameKitDelegate *delegate;
	
};



