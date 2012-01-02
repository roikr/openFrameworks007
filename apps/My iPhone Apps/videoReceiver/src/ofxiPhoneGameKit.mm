//
//  ofxiPhoneGameKit.m
//  gameKitExample
//
//  Created by Roee Kremer on 12/1/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "ofxiPhoneGameKit.h"
//#include "gkEvents.h"

ofEvent<gkReceiveDataEventArgs> gkReceiveDataEvent;

//
// various states the game can get into
//
typedef enum {
	kStateStartGame,
	kStatePicker,
	kStateMultiplayer,
//	kStateMultiplayerCointoss,
	kStateMultiplayerReconnect
} gameStates;



// GameKit Session ID for app
#define kTankSessionID @"gktank"

#define kMaxTankPacketSize 1024

@implementation GameKitDelegate
@synthesize gameState,gameSession, gamePeerId;

- (id)initWithGameKit:(ofxiPhoneGameKit *)gameKit {
	if (self = [super init]) {
		gameSession = nil;
		gamePeerId = nil;
		self.gameState = kStateStartGame;
		cppGameKit = gameKit;
	}
	
	return self;
	
	
}


- (void)dealloc { 
	
	// cleanup the session
	[self invalidateSession:self.gameSession];
	self.gameSession = nil;
	self.gamePeerId = nil;
	
	[super dealloc];
}

#pragma mark -
#pragma mark Peer Picker Related Methods

-(void)startPicker {
	self.gameState = kStatePicker;
	GKPeerPickerController*		picker;
	
	picker = [[GKPeerPickerController alloc] init]; // note: picker is released in various picker delegate methods when picker use is done.
	picker.delegate = self;
	[picker show]; // show the Peer Picker
}

- (void)sendData:(NSData *)data {
	BOOL bReliable = NO;
	//NSString *message = @"hello" ; //[NSString stringWithFormat:@"sendData to: %@",[gameSession displayNameForPeer:gamePeerId]];
	//[gameSession sendData:[message dataUsingEncoding:NSASCIIStringEncoding] toPeers:[NSArray arrayWithObject:gamePeerId] withDataMode:bReliable ? GKSendDataReliable : GKSendDataUnreliable error:nil];
	[gameSession sendData:data toPeers:[NSArray arrayWithObject:gamePeerId] withDataMode:bReliable ? GKSendDataReliable : GKSendDataUnreliable error:nil];
}

#pragma mark GKPeerPickerControllerDelegate Methods

- (void)peerPickerControllerDidCancel:(GKPeerPickerController *)picker { 
	// Peer Picker automatically dismisses on user cancel. No need to programmatically dismiss.
    
	// autorelease the picker. 
	picker.delegate = nil;
    [picker autorelease]; 
	
	// invalidate and release game session if one is around.
	if(self.gameSession != nil)	{
		[self invalidateSession:self.gameSession];
		self.gameSession = nil;
	}
	
	// go back to start mode
	self.gameState = kStateStartGame;
} 

/*
 *	Note: No need to implement -peerPickerController:didSelectConnectionType: delegate method since this app does not support multiple connection types.
 *		- see reference documentation for this delegate method and the GKPeerPickerController's connectionTypesMask property.
 */

//
// Provide a custom session that has a custom session ID. This is also an opportunity to provide a session with a custom display name.
//
- (GKSession *)peerPickerController:(GKPeerPickerController *)picker sessionForConnectionType:(GKPeerPickerConnectionType)type { 
	GKSession *session = [[GKSession alloc] initWithSessionID:kTankSessionID displayName:nil sessionMode:GKSessionModePeer]; 
	return [session autorelease]; // peer picker retains a reference, so autorelease ours so we don't leak.
}

- (void)peerPickerController:(GKPeerPickerController *)picker didConnectPeer:(NSString *)peerID toSession:(GKSession *)session { 
	// Remember the current peer.
	self.gamePeerId = peerID;  // copy
	
	// Make sure we have a reference to the game session and it is set up
	self.gameSession = session; // retain
	self.gameSession.delegate = self; 
	[self.gameSession setDataReceiveHandler:self withContext:NULL];
	
	// Done with the Peer Picker so dismiss it.
	[picker dismiss];
	picker.delegate = nil;
	[picker autorelease];
	
	// Start Multiplayer game by entering a cointoss state to determine who is server/client.
	self.gameState = kStateMultiplayer;
    
} 

#pragma mark -
#pragma mark Session Related Methods

//
// invalidate session
//
- (void)invalidateSession:(GKSession *)session {
	if(session != nil) {
		[session disconnectFromAllPeers]; 
		session.available = NO; 
		[session setDataReceiveHandler: nil withContext: NULL]; 
		session.delegate = nil; 
	}
}

#pragma mark Data Send/Receive Methods

/*
 * Getting a data packet. This is the data receive handler method expected by the GKSession. 
 * We set ourselves as the receive data handler in the -peerPickerController:didConnectPeer:toSession: method.
 */
- (void)receiveData:(NSData *)data fromPeer:(NSString *)peer inSession:(GKSession *)session context:(void *)context { 
//	NSString *message = [[NSString alloc] initWithData:data encoding:NSASCIIStringEncoding];
//	
//	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"receiveData" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
//	[alert show];
//	[alert release];
	
	
	gkReceiveDataEventArgs receiveDataEventArg;
	receiveDataEventArg.data.set((const char *)[data bytes],[data length]);
	receiveDataEventArg.peer = ofxNSStringToString(peer);
	receiveDataEventArg.context = context;
//	ofBuffer buffer((const char *)[data bytes],[data length]);
//	cppGameKit->receiveData(buffer, ofxNSStringToString(peer), context);
	ofNotifyEvent(gkReceiveDataEvent, receiveDataEventArg);//
	
//	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"receiveData" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
//	[alert show];
//	[alert release];
	
	
}


#pragma mark GKSessionDelegate Methods

// we've gotten a state change in the session
- (void)session:(GKSession *)session peer:(NSString *)peerID didChangeState:(GKPeerConnectionState)state { 
	
	NSLog(@"peer: %@ didChangeState: %i",[session displayNameForPeer:peerID],state);
	
//	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:[session displayNameForPeer:peerID]  
//													message:[NSString stringWithFormat:@"didChangeState: %u",state] 
//												   delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
//	[alert show];
//	[alert release];
	
	
	if(self.gameState == kStatePicker) {
		return;				// only do stuff if we're in multiplayer, otherwise it is probably for Picker
	}
	
	if(state == GKPeerStateDisconnected) {
		// We've been disconnected from the other peer.
		
		// Update user alert or throw alert if it isn't already up
//		NSString *message = [NSString stringWithFormat:@"Could not reconnect with %@.", [session displayNameForPeer:peerID]];
//		if((self.gameState == kStateMultiplayerReconnect) && self.connectionAlert && self.connectionAlert.visible) {
//			self.connectionAlert.message = message;
//		}
//		else {
//			UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Lost Connection" message:message delegate:self cancelButtonTitle:@"End Game" otherButtonTitles:nil];
//			self.connectionAlert = alert;
//			[alert show];
//			[alert release];
//		}
		
		// go back to start mode
		self.gameState = kStateStartGame; 
	} 
} 


#pragma mark -
#pragma mark Game Logic Methods

//
// setter for gameState property
//
- (void)setGameState:(NSInteger)newState {
	if(newState == kStateStartGame) {
		if(self.gameSession) {
			// invalidate session and release it.
			[self invalidateSession:self.gameSession];
			self.gameSession = nil;
		}
		
	}
	
	gameState = newState;
}



@end


extern ofEvent<ofBuffer> ofGKReceiveDataEvent; 

ofxiPhoneGameKit::ofxiPhoneGameKit() {
	delegate = [[GameKitDelegate alloc] initWithGameKit:this];
}

ofxiPhoneGameKit::~ofxiPhoneGameKit() {
	[delegate release];
}

void ofxiPhoneGameKit::startPicker() {
	[delegate startPicker];
}

void ofxiPhoneGameKit::sendData(const ofBuffer &data) {
	
	[delegate sendData:[NSData	dataWithBytes:(unsigned char*) data.getBinaryBuffer() length:data.size()]];
	
}


//void ofxiPhoneGameKit::receiveData(ofBuffer &data,string peer,void *context) {
//	
//	
//}
