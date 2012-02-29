#pragma once

#include "ofMain.h"





ofVec3f ofWorldToScreen(ofVec3f world);
ofVec3f ofScreenToWorld(ofVec3f screen);
ofMesh getProjectedMesh(const ofMesh& mesh);
ofVec3f getClosestPointOnMesh(const ofMesh& mesh, float x, float y, int* choice = NULL, float* distance = NULL);

void exportPlyCloud(string filename, ofMesh& cloud);

int getClosestTripletOnMesh(const ofMesh& objectMesh,const ofMesh& imageMesh, float x, float y, float* distance);

void findHomography(ofPoint src[4], ofPoint dst[4], float homography[16]);
void gaussian_elimination(float *input, int n);

vector<ofVec3f> findBoundingBox(vector<ofVec3f> &input);
vector<ofVec3f> findScreenCorners(vector<ofVec3f> &input,int width,int height,float scale); // scale is model units per pixel