#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxAudioFile.h"
#include "ofxFFT.h"


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
    void audioIn(float * input, int bufferSize, int nChannels);
    void processAudio(float * input, int bufferSize);
    
    
    ofSoundStream soundStream;

    int		sampleRate;
    bool 	bPlay;
    
    vector <float> input;
		        
		
    ofxAudioFile audio;
    ofxFFT fft;
    
    vector <float> power;
    vector <float> spectrum;
    vector <float> spectralFlux;
    vector <float>::iterator sfiter;
    
    vector <float> smoothed;

    
    
    int counter;
    float beatDecay;
    
    bool bInput;
};

#endif
