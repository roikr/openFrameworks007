//
//  ofxOscEventsReceiver.h
//  oscReceiveExample
//
//  Created by Roee Kremer on 3/20/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef oscReceiveExample_ofxOscEventsReceiver_h
#define oscReceiveExample_ofxOscEventsReceiver_h

#include "ofEvents.h"
#include "ofxOsc.h"

// void gotOscMessage(ofxOscMessage &m);
// ofAddListener(receiver.gotOscMessage, this, &testApp::gotOscMessage);

class ofxOscEventsReceiver {
public:
    
    /// receive messages at port
	void setup(int port );
    void update(ofEventArgs& event);  
    
    
    ofxOscReceiver receiver;  
    ofEvent<ofxOscMessage> gotOscMessage;
    
};




#endif
