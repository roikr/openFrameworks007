#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxAudioFile.h"
#include "ofxFFT.h"

struct band {
    vector<float> history;
    float energy;
    float mean;
    int first;
    int width;
    float beat;
    int counter;
};

class testApp : public ofBaseApp{

public:

    void setup();
    void update();
    void draw();

    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void audioOut(float * input, int bufferSize, int nChannels);
    
    
    ofSoundStream soundStream;

    int		sampleRate;
    bool 	bPlay;
    
    vector <float> lAudio;
		        
		
    ofxAudioFile audio;
    ofxFFT fft;
    
    vector <float> power;
    vector <float> smoothed;

    
    vector<band> bands;
    int counter;
    float beatDecay;
    
};

#endif
