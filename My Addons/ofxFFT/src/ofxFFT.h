//
//  ofxFFT.h
//  audioInputExample
//
//  Created by Roee Kremer on 1/31/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once


#include <Accelerate/Accelerate.h>



class ofxFFT {

public:
    void setup(int numFrames);
    void process(float * input,int bufferSize);
    void calc();
    
    int numFrames;

    COMPLEX_SPLIT   mDspSplitComplex;
    FFTSetup        mSpectrumAnalysis;
    int log2n;
    int mFFTLength;
    float mFFTNormFactor;
    
    float *input;
    float *amplitude;
    float *power;
    
    int counter;
    bool bReady;
    
};