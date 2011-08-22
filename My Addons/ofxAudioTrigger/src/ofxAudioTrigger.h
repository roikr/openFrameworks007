/*
 *  ofxAudioTrigger.h
 *  iPhoneSantaExample
 *
 *  Created by Roee Kremer on 12/9/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

enum {
	TRIGGER_IDLE,
	TRIGGER_SET,
	TRIGGER_TRIGGERED
};

class ofxAudioTrigger {
	
public:
	ofxAudioTrigger();
	void setup();
	void update();
	void draw();
	
	void resetTrigger();
	void setThresh(float thresh);
	float getThresh();
	void setTrigger();
	int getTriggerState();
	
	void resetMeters();
	
	float getAmplitude();
	float getRmsAmplitude();
	
	float getPeak();
	float getRmsPeak();
	
	void setAutoThresh(float bias,int length);
	void disableAutoTrhresh();
	bool getIsAutoThreshEnabled();
	
	
	
	void audioReceived( float * input, int bufferSize);
	
protected:
	
	int state;
	float thresh;
	
	float amplitude;
	float rmsAmplitude;
	float peak;
	float rmsPeak;
	
	vector<float> samples;
	vector<float>::iterator currentSample;
	bool bAutoThresh;
	float bias;
	
	
	
};


