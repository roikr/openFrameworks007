#pragma once

#include "ofMain.h"



int getClosestTripletOnMesh(const ofMesh& objectMesh,const ofMesh& imageMesh, float x, float y, float* distance);

void findHomography(ofPoint src[4], ofPoint dst[4], float homography[16]);
void gaussian_elimination(float *input, int n);

vector<ofVec3f> findBoundingBox(vector<ofVec3f> &input);
vector<ofVec3f> findScreenCorners(vector<ofVec3f> &input,int width,int height,float scale); // scale is model units per pixel
void screenCoordinateSystem(vector<ofVec3f> &input,ofVec3f &origin,ofVec3f &xVec,ofVec3f &yVec);

void ofSaveMatrices(string filename);
void ofLoadMatrices(string filename,ofMatrix4x4 &projMat,ofMatrix4x4 &mvMat);
void ofApplyMatrices(ofMatrix4x4 &projMat,ofMatrix4x4 &mvMat);