#pragma once

#include "ofMain.h"





ofVec3f ofWorldToScreen(ofVec3f world);
ofVec3f ofScreenToWorld(ofVec3f screen);
ofMesh getProjectedMesh(const ofMesh& mesh);
ofVec3f getClosestPointOnMesh(const ofMesh& mesh, float x, float y, int* choice = NULL, float* distance = NULL);

void exportPlyCloud(string filename, ofMesh& cloud);

