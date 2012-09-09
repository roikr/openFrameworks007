//
//  ofxiBigImage.cpp
//  kaiserNav
//
//  Created by Roee Kremer on 9/9/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxiBigImage.h"



void load(string filename,ofxiTexture &texture) {
    texture.load(ofToDataPath(filename+".png"),OFX_TEXTURE_TYPE_PNG);
}
//#else
//void load(string filename,ofImage &image) {
//    image.setUseTexture(false);
//    image.loadImage(filename+".png");
//}
//#endif


void ofxiBigImage::loadImage(string filename,int size,int width,int height) {
    this->size = size;
    this->width = width;
    this->height = height;
    
    string basename=ofSplitString(filename, "/").back();
    cout << "start loading" << endl;
    int start = ofGetElapsedTimeMillis();
    
    int numCols = floor((float)width/(float)size);
    int numRows = floor((float)height/(float)size);
    float widthRem = width-numCols*size;
    float heightRem = height-numRows*size;
    
    
    for (int i=0; i<numRows; i++) {
        for (int j=0; j<numCols ; j++) {
            tiles.push_back(tile(ofRectangle(j*size, i*size, 1, 1)));
            load(filename+"/"+basename+'_'+ofToString(i)+'_'+ofToString(j), tiles.back().texture);
        }
        tiles.push_back(tile(ofRectangle(numCols*size, i*size, widthRem/float(size), 1)));
        load(filename+"/"+basename+'_'+ofToString(i)+'_'+ofToString(numCols), tiles.back().texture);
    }
    
    for (int j=0; j<numCols ; j++) {
        tiles.push_back(tile(ofRectangle(j*size, numRows*size, 1, heightRem/float(size))));
        load(filename+"/"+basename+'_'+ofToString(numRows)+'_'+ofToString(j), tiles.back().texture);
    }
    
    tiles.push_back(tile(ofRectangle(numCols*size,numRows*size,widthRem/float(size),heightRem/float(size))));
    load(filename+"/"+basename+'_'+ofToString(numRows)+'_'+ofToString(numCols), tiles.back().texture);
    
    cout << "finish loading: " <<  endl;
    cout << "time: " << ofGetElapsedTimeMillis()-start << endl; 
}

void ofxiBigImage::draw() {
    //    ofTranslate(-width/2, -height/2);
    
    for (vector<tile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
        ofPushMatrix();
        ofTranslate(iter->rect.x, iter->rect.y);
        
        iter->texture.draw(iter->rect.width,iter->rect.height);
        
        ofPopMatrix();
    }
}

void ofxiBigImage::release() {
    for (vector<tile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
        
        iter->texture.release();
        
    }
    tiles.clear();
}


int ofxiBigImage::getWidth() {
    return width;
}

int ofxiBigImage::getHeight() {
    return height;
}

bool ofxiBigImage::getDidLoad() {
    return !tiles.empty();
}

