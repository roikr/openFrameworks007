#pragma once

#include "ofMain.h"

enum {
    THRESHOLD_MIN,
    THRESHOLD_MAX,
    THRESHOLD_TRIGGER_ON,
    THRESHOLD_TRIGGER_OFF,
    THRESHOLD_NUM
};


struct channel {
   vector<float> input,limiterVec,peakVec,envelopeVec;
    
    float peak;
    float envelope;
    
    vector<float>thresholds;
    bool bTriggered;
    ofVec3f pos;

};

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void audioIn(float * input, int bufferSize, int nChannels);
    
        void save();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    

    vector<channel> channels;
    
    ofSoundStream soundStream;
    
    
    int bufferCounter;
    int drawCounter;
    
    float ga;
    float gr;
    
    bool bDebug;
    
    int curThresh;
    bool bSetThresh;
    
    vector<int*> buffers;
    ofImage image;
    int counter;
		
};
