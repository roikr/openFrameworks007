//
//  ofxBigImage.cpp
//  emptyExample
//
//  Created by Roee Kremer on 8/1/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxBigImage.h"

#ifdef TARGET_OPENGLES

void ofxBigImage::loadImage(string filename,int size,int width,int height) {
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
            tiles.back().texture.load(filename+"/"+basename+'_'+ofToString(i)+'_'+ofToString(j)+".pvr");
        }
        tiles.push_back(tile(ofRectangle(numCols*size, i*size, widthRem/float(size), 1)));
        tiles.back().texture.load(filename+"/"+basename+'_'+ofToString(i)+'_'+ofToString(numCols)+".pvr");
    }
    
    for (int j=0; j<numCols ; j++) {
        tiles.push_back(tile(ofRectangle(j*size, numRows*size, 1, heightRem/float(size))));
        tiles.back().texture.load(filename+"/"+basename+'_'+ofToString(numRows)+'_'+ofToString(j)+".pvr");
    }
    
    tiles.push_back(tile(ofRectangle(numCols*size,numRows*size,widthRem/float(size),heightRem/float(size))));
    tiles.back().texture.load(filename+"/"+basename+'_'+ofToString(numRows)+'_'+ofToString(numCols)+".pvr");

    cout << "finish loading: " <<  endl;
    cout << "time: " << ofGetElapsedTimeMillis()-start << endl; 
}

void ofxBigImage::draw() {
    //    ofTranslate(-width/2, -height/2);
    
    for (vector<tile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
        ofPushMatrix();
        ofTranslate(iter->rect.x, iter->rect.y);
        iter->texture.draw(iter->rect.width,iter->rect.height);
        ofPopMatrix();
    }
}

void ofxBigImage::release() {
    for (vector<tile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
        iter->texture.release();
    }
    tiles.clear();
}

#else
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


void ofxBigImage::draw() {
//    ofTranslate(-width/2, -height/2);
    for (vector<tile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
        iter->image.draw(iter->rect);
    }
}

void ofxBigImage::release() {
    tiles.clear();
}
#endif

int ofxBigImage::getWidth() {
    return width;
}

int ofxBigImage::getHeight() {
    return height;
}

bool ofxBigImage::getDidLoad() {
    return !tiles.empty();
}

