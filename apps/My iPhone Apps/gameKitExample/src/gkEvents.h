/*
 * ofxEventsAddon.h
 *
 *  Created on: 17-dic-2008
 *      Author: art
 */

#ifndef OFXEVENTSADDON_H_
#define OFXEVENTSADDON_H_

#include "ofMain.h"
#include "ofEvents.h"


class gkReceiveDataEventArgs : public ofEventArgs {
public:
	ofBuffer data;
	string peer;
	void *context;
};

class gkEvents{
public:
	gkEvents(){
//	    counter = 0;
	}

	/*
	void enable(){
	    ofAddListener(ofEvents.update, this, &gkEvents::update);
	}

	void disable(){
	    ofRemoveListener(ofEvents.update, this, &gkEvents::update);
	}

	void update(ofEventArgs & args){
		counter+=0.3;
		int intCounter=(int)counter;
		ofNotifyEvent(newFloatEvent,counter,this);
		ofNotifyEvent(newIntEvent,intCounter,this);
	}
	 */

	ofEvent<gkReceiveDataEventArgs> gkReceiveDataEvent;
//	ofEvent<int> newIntEvent;

private:
//	float counter;
};
#endif /* OFXEVENTSADDON_H_ */
