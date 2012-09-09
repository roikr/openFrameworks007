//
//  ofxBigImage.cpp
//  emptyExample
//
//  Created by Roee Kremer on 8/1/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxBigImage.h"



void loadTile(string filename,ofImage &image) {
    image.setUseTexture(false);
    image.loadImage(filename+".png");
}

    

void ofxBigImage::setup(string filename,int size,int width,int height) {
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
            tiles.push_back(tile(ofRectangle(j*size, i*size, size, size)));
            loadTile(filename+"/"+basename+'_'+ofToString(i)+'_'+ofToString(j), tiles.back().image);
        }
        tiles.push_back(tile(ofRectangle(numCols*size, i*size, widthRem, size)));
        loadTile(filename+"/"+basename+'_'+ofToString(i)+'_'+ofToString(numCols), tiles.back().image);
    }
    
    for (int j=0; j<numCols ; j++) {
        tiles.push_back(tile(ofRectangle(j*size, numRows*size, size, heightRem)));
        loadTile(filename+"/"+basename+'_'+ofToString(numRows)+'_'+ofToString(j), tiles.back().image);
    }
    
    tiles.push_back(tile(ofRectangle(numCols*size,numRows*size,widthRem,heightRem)));
    loadTile(filename+"/"+basename+'_'+ofToString(numRows)+'_'+ofToString(numCols), tiles.back().image);

    cout << "finish loading: " <<  endl;
    cout << "time: " << ofGetElapsedTimeMillis()-start << endl; 
    
    bLoaded = false;
}



/*
void ofxBigImage::loadImage(string filename,int size) {
    ofImage image;
    image.setUseTexture(false);
    image.loadImage(filename);
    loadImage(image, size);
}


void ofxBigImage::loadImage(ofImage image,int size) {
    
    cout << "image type: " << image.type << endl;
    
    width = image.getWidth();
    height = image.getHeight();
    
    int numCols = floor((float)width/(float)size);
    int numRows = floor((float)height/(float)size);
    int widthRem = width-numCols*size;
    int heightRem = height-numRows*size;
    
    
    for (int i=0; i<numRows; i++) {
        for (int j=0; j<numCols ; j++) {
            tiles.push_back(tile(ofRectangle(j*size, i*size, size, size)));
        }
        tiles.push_back(tile(ofRectangle(numCols*size, i*size, widthRem, size)));
    }
    
    for (int j=0; j<numCols ; j++) {
        tiles.push_back(tile(ofRectangle(j*size, numRows*size, size, heightRem)));
    }
    
    tiles.push_back(tile(ofRectangle(numCols*size,numRows*size,widthRem,heightRem)));
    
    
    for (vector<tile>::iterator iter = tiles.begin();iter!=tiles.end();iter++) {
         
       
        iter->image.allocate(iter->rect.width, iter->rect.height, image.getPixelsRef().getImageType());
        image.getPixelsRef().cropTo(iter->image.getPixelsRef(), iter->rect.x, iter->rect.y, iter->rect.width, iter->rect.height);
        iter->image.update();
        cout << distance(tiles.begin(), iter) << "\ttype: " << iter->image.getPixelsRef().getImageType() <<"\t" << iter->rect.x << "\t" << iter->rect.y << "\t" << iter->rect.width << "\t" << iter->rect.height << endl;
        iter->image.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST); // roikr: this was the trick to boost the fps as alternative to linear filtering...
    }
}
*/

void ofxBigImage::load() {
    for (vector<tile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
        iter->image.setUseTexture(true);
        iter->image.reloadTexture();
    } 
    bLoaded = true;
}
void ofxBigImage::unload() {
    for (vector<tile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
        iter->image.setUseTexture(false);
        iter->image.getTextureReference().clear();
        
    }
    bLoaded = false;
}

void ofxBigImage::draw() {
//    ofTranslate(-width/2, -height/2);
    for (vector<tile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
        iter->image.draw(iter->rect);
    }
}

void ofxBigImage::release() {
    tiles.clear();
}


int ofxBigImage::getWidth() {
    return width;
}

int ofxBigImage::getHeight() {
    return height;
}

bool ofxBigImage::getDidLoad() {
    return bLoaded;
}

