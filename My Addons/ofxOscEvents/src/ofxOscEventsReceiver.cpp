//
//  ofxOscEventsReceiver.cpp
//  oscReceiveExample
//
//  Created by Roee Kremer on 3/20/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//


#include "ofxOscEventsReceiver.h"

#include <iostream>

void ofxOscEventsReceiver::setup(int port ) {
    
    
    
    // listen on the given port
	cout << "listening for osc messages on port " << port << "\n";
	receiver.setup( port );
    
    ofAddListener(ofEvents.update, this, &ofxOscEventsReceiver::update);
    
	
}


//--------------------------------------------------------------
void ofxOscEventsReceiver::update(ofEventArgs& event){
    
	
    
	// check for waiting messages
	while( receiver.hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );
        
		
        if ( m.getAddress() == "/keyPressed" )
		{
			ofNotifyKeyPressed(m.getArgAsInt32( 0 ));
            
		} else if ( m.getAddress() == "/keyReleased" )
		{
			ofNotifyKeyReleased(m.getArgAsInt32( 0 ));
            
		} else if ( m.getAddress() == "/mouseMoved" )
		{
			ofNotifyMouseMoved(m.getArgAsInt32( 0 ),m.getArgAsInt32( 1 ));
            
		} else if ( m.getAddress() == "/mouseDragged" )
		{
			ofNotifyMouseDragged(m.getArgAsInt32( 0 ),m.getArgAsInt32( 1 ), m.getArgAsInt32( 2 ));
            
		} else if ( m.getAddress() == "/mousePressed" )
		{
			ofNotifyMousePressed(m.getArgAsInt32( 0 ),m.getArgAsInt32( 1 ), m.getArgAsInt32( 2 ));
            
		} else if ( m.getAddress() == "/mouseReleased" )
		{
			ofNotifyMouseReleased(m.getArgAsInt32( 0 ),m.getArgAsInt32( 1 ), m.getArgAsInt32( 2 ));
            
		} else {
            
        }
        
        ofNotifyEvent(gotOscMessage, m, this);
	}
}


