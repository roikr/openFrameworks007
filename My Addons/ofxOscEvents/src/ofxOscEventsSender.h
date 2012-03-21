//
//  ofxOscEventsSender.h
//  oscSenderExample
//
//  Created by Roee Kremer on 3/20/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef oscSenderExample_ofxOscEventsSender_h
#define oscSenderExample_ofxOscEventsSender_h

#include "ofEvents.h"
#include "ofxOsc.h"

class ofxOscEventsSender {
public:
    
    /// send messages to hostname and port
	void setup(string hostname, int port );
    void sendMessage( ofxOscMessage& message );
	
    
    void keyPressed(ofKeyEventArgs& event);
    void keyReleased(ofKeyEventArgs& event);
    void mouseMoved(ofMouseEventArgs& event);
    void mouseDragged(ofMouseEventArgs& event);
    void mousePressed(ofMouseEventArgs& event);
    void mouseReleased(ofMouseEventArgs& event);
    
    ofxOscSender sender;
    
};


#endif
