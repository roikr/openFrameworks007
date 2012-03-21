//
//  ofxOscEvent.h
//  oscReceiveExample
//
//  Created by Roee Kremer on 3/20/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef oscReceiveExample_ofxOscEvent_h
#define oscReceiveExample_ofxOscEvent_h

#include "ofMain.h"
#include "ofEvents.h"
#include "ofxOscMessage.h"

class ofxOscEvent{

public:
	ofxOscEvent(ofxOscMessage& message):oscMessage(message){}
	
    
};


#endif
