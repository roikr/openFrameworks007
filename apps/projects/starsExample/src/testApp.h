#pragma once

#include "ofMain.h"

#define NUM_CHANNELS 2

struct star {
    ofVec3f pos;
    int startTime;
    int group;
    float angle;
    float randomPhase;
};

struct channel {
   vector<float> input,envelope,power;
    
   float amplitude;
};

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void audioIn(float * input, int bufferSize, int nChannels);

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    vector<star> stars;
    vector<channel> channels;
    
    ofSoundStream soundStream;
    
    
    int bufferCounter;
    int drawCounter;
    
    float ga;
    float gr;
    
    bool bDebug;
		
};
