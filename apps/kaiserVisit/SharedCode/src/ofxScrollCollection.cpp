//
//  ofxScrollCollection.cpp
//  scrollCollectionExample
//
//  Created by Roee Kremer on 6/19/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxScrollCollection.h"

void ofxScrollCollection::setup(int width,int height,int hMargin,int vMargin) {
    this->width = width;
    this->height = height;
    this->hMargin = hMargin;
    this->vMargin = vMargin;
}

void ofxScrollCollection::addItem(string filename) {
    ofImage img;
    images.push_back(img);
    images.back().loadImage(filename);
}

void ofxScrollCollection::draw() {
    
    float w =  width-2*hMargin;
    
    int y=vMargin;
    for (vector<ofImage>::reverse_iterator iter=images.rbegin(); iter!=images.rend(); iter++) {
        if (iter->getWidth()>0) {
            float h = iter->getHeight()/iter->getWidth()*w;
            iter->draw(hMargin,y, w, h);
            y+=h+vMargin;
            cout << y << endl;
        }
    }
}

void ofxScrollCollection::touchDown(ofTouchEventArgs &touch) {
    
}

void ofxScrollCollection::touchMoved(ofTouchEventArgs &touch){
    
}

void ofxScrollCollection::touchUp(ofTouchEventArgs &touch){
    
}

void ofxScrollCollection::touchDoubleTap(ofTouchEventArgs &touch){
    
}

void ofxScrollCollection::touchCancelled(ofTouchEventArgs &touch){
    
}
