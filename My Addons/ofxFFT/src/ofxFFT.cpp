//
//  ofxFFT.cpp
//  audioInputExample
//
//  Created by Roee Kremer on 1/31/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxFFT.h"
#include <stdio.h> // for printf
#include <string.h> // for memcpy



void ofxFFT::setup(int numFrames) {
    
    this->numFrames = numFrames;
  
    log2n = log2(numFrames);
    mFFTLength = numFrames / 2;
    
    mSpectrumAnalysis = vDSP_create_fftsetup(log2n, FFT_RADIX2);
    if (mSpectrumAnalysis == NULL) {
        printf("\nFFT_Setup failed to allocate enough memory  for"
               "the real FFT.\n");
        //std::exit(0);
    }
    
    mDspSplitComplex.realp = new float[mFFTLength];
    mDspSplitComplex.imagp = new float[mFFTLength];
    input = new float[numFrames];
    amplitude = new float[mFFTLength];
    power = new float[mFFTLength];
    
    
    mFFTNormFactor = 1.0/(2*numFrames);
    counter = 0;
    bReady = false;
}


    


void ofxFFT::process(float * input,int bufferSize) {
    memcpy(this->input+counter, input, sizeof(float)*bufferSize);
    counter+=bufferSize;
    
    bReady = counter==numFrames;
    
    if (bReady) {
        counter = 0;
        calc();
    } 
}
    
#define ADJUST_0_DB 1.5849e-13

void ofxFFT::calc() {
    
    //Generate a split complex vector from the real data
    vDSP_ctoz((COMPLEX *)input, 2, &mDspSplitComplex, 1, mFFTLength);
    
    //Take the fft and scale appropriately
    vDSP_fft_zrip(mSpectrumAnalysis, &mDspSplitComplex, 1, log2n, FFT_FORWARD);
//    vDSP_fft_zrip(mSpectrumAnalysis, &mDspSplitComplex, 1, log2n, FFT_INVERSE);
    
    vDSP_vsmul(mDspSplitComplex.realp, 1, &mFFTNormFactor, mDspSplitComplex.realp, 1, mFFTLength);
    vDSP_vsmul(mDspSplitComplex.imagp, 1, &mFFTNormFactor, mDspSplitComplex.imagp, 1, mFFTLength);
    
//    /* The output signal is now in a split real form.  Use the  function
//     * vDSP_ztoc to get a split real vector. */
//    vDSP_ztoc(&mDspSplitComplex, 1, (COMPLEX *) output, 2, mFFTLength);
//    
    
    
    //Zero out the nyquist value
    mDspSplitComplex.imagp[0] = 0.0;
    
    //Convert the fft data to dB
    vDSP_zvmags(&mDspSplitComplex, 1, amplitude, 1, mFFTLength);
    
    //In order to avoid taking log10 of zero, an adjusting factor is added in to make the minimum value equal -128dB
    float  mAdjust0DB = ADJUST_0_DB;
    vDSP_vsadd(amplitude, 1, &mAdjust0DB, power, 1, mFFTLength);
    float one = 1;
    vDSP_vdbcon(power, 1, &one, power, 1, mFFTLength, 0);
    
}





