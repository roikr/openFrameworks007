//
//  ofxOscEventsSender.cpp
//  oscSenderExample
//
//  Created by Roee Kremer on 3/20/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxOscEventsSender.h"

#include <iostream>


void ofxOscEventsSender::setup(string hostname, int port ) {
    sender.setup(hostname, port);
    
    ofAddListener(ofEvents.keyPressed, this, &ofxOscEventsSender::keyPressed);
    ofAddListener(ofEvents.mousePressed, this, &ofxOscEventsSender::mousePressed);
    ofAddListener(ofEvents.mouseReleased, this, &ofxOscEventsSender::mouseReleased);
    ofAddListener(ofEvents.mouseDragged, this, &ofxOscEventsSender::mouseDragged);

}

void ofxOscEventsSender::sendMessage( ofxOscMessage& message ) {
    sender.sendMessage(message);
}


void ofxOscEventsSender::keyPressed(ofKeyEventArgs& event) {
    cout << event.key << endl;
    ofxOscMessage m;
	m.setAddress( "/keyPressed" );
    m.addIntArg( event.key);
	sender.sendMessage( m );
}

void ofxOscEventsSender::keyReleased(ofKeyEventArgs& event) {
    ofxOscMessage m;
	m.setAddress( "/keyReleased" );
    m.addIntArg( event.key);
	sender.sendMessage( m );
}

void ofxOscEventsSender::mouseMoved(ofMouseEventArgs& event) {
    ofxOscMessage m;
	m.setAddress( "/mouseMoved" );
	m.addIntArg( event.x );
	m.addIntArg( event.y );
	sender.sendMessage( m );
}

void ofxOscEventsSender::mouseDragged(ofMouseEventArgs& event) {
    ofxOscMessage m;
	m.setAddress( "/mouseDragged" );
	m.addIntArg( event.x );
	m.addIntArg( event.y );
    m.addIntArg( event.button);
	sender.sendMessage( m );

}

void ofxOscEventsSender::mousePressed(ofMouseEventArgs& event) {
    ofxOscMessage m;
	m.setAddress( "/mousePressed" );
	m.addIntArg( event.x );
	m.addIntArg( event.y );
    m.addIntArg( event.button);
	sender.sendMessage( m );
}

void ofxOscEventsSender::mouseReleased(ofMouseEventArgs& event) {
    ofxOscMessage m;
	m.setAddress( "/mouseReleased" );
	m.addIntArg( event.x );
	m.addIntArg( event.y );
    m.addIntArg( event.button);
	sender.sendMessage( m );
}