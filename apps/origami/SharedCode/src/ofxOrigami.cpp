//
//  ofxOrigami.cpp
//  foldingFirst
//
//  Created by Roee Kremer on 6/11/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxOrigami.h"

ofColor randomHue() {
    ofColor color;
    color.setHsb(255.0/15.0*((int)ofRandom(255)%15), 180, 150);
    return color;
}

void ofxOrigami::setup() {
    face f;
    f.color = randomHue();
    f.vertices.push_back(ofVec2f(100,100));
    f.vertices.push_back(ofVec2f(300,100));
    f.vertices.push_back(ofVec2f(300,300));
    f.vertices.push_back(ofVec2f(100,300));
    faces.push_back(f);
    
    numCuts = 0;
}

/*
 Determine the intersection point of two line segments
 Return FALSE if the lines don't intersect
 http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/pdb.c
 */

#define EPS 0.0000001f

bool lineIntersect(ofVec2f p0,ofVec2f p1,ofVec2f p2,ofVec2f p3,ofVec2f &q) 

{
    
    float mua,mub;
    float denom,numera,numerb;
    
    denom  = (p3.y-p2.y) * (p1.x-p0.x) - (p3.x-p2.x) * (p1.y-p0.y);
    numera = (p3.x-p2.x) * (p0.y-p2.y) - (p3.y-p2.y) * (p0.x-p2.x);
    numerb = (p1.x-p0.x) * (p0.y-p2.y) - (p1.y-p0.y) * (p0.x-p2.x);
    
    /* Are the line coincident? */
    if (ABS(numera) < EPS && ABS(numerb) < EPS && ABS(denom) < EPS) {
        q.x = (p0.x + p1.x) / 2;
        q.y = (p0.y + p1.y) / 2;
        return(true);
    }
    
    /* Are the line parallel */
    if (ABS(denom) < EPS) {
        q.x = 0;
        q.y = 0;
        return(false);
    }
    
    /* Is the intersection along the the segments */
    mua = numera / denom;
    mub = numerb / denom;
    if (mua < 0 || mua > 1 || mub < 0 || mub > 1) {
        q.x = 0;
        q.y = 0;
        return(false);
    }
    
    q.x = p0.x + mua * (p1.x - p0.x);
    q.y = p0.y + mua * (p1.y - p0.y);
    return(true);
}


void ofxOrigami::split(ofVec2f p0,ofVec2f p1) {
    ofVec2f perp = (p1-p0).perpendicular();
    float d = perp.dot(p0);
    
    int counter = 0;
    
    for ( list<face>::iterator fiter=faces.begin();fiter!=faces.end();fiter++) {
        
        vector<pair<int,ofVec2f> > intersections;
        
        // roikr: need to check passing on a vertex
        for (int i=0;i<fiter->vertices.size();i++) {
            vector<ofVec2f>::iterator iter=fiter->vertices.begin()+i;
            vector<ofVec2f>::iterator niter = fiter->vertices.begin()+((i+1) % fiter->vertices.size());
            
            
            float p = perp.dot(*iter)-d;
            float q = perp.dot(*(niter))-d;
            //            cout << "(" << iter->x << "," << iter->y << "):\t" << p << "," << q << endl;
            if (ofSign(p)!=ofSign(q)) {
                
                ofVec2f li;
                if (lineIntersect(p0, p1, *iter, *niter, li)) {
                    intersections.push_back(make_pair(i,li));
                    //                    cout << i << endl; 
                }
                
                
            }
        }
        
        
        if (!intersections.empty()) {
            counter++;
            pair<int,ofVec2f> li0,li1;
            li0 = intersections.front();
            li1 = intersections.back();
            
            face f0;
            f0.vertices.push_back(li0.second);
            for (int i=li0.first+1; i<=li1.first; i++) {
                f0.vertices.push_back(*(fiter->vertices.begin()+(i%fiter->vertices.size())));
            }
            f0.vertices.push_back(li1.second);
            f0.color = fiter->color;
            f0.cuts = fiter->cuts;
            f0.cuts.push_back(cut(numCuts+1,li0.second,li1.second));
            
            
            face f1;
            f1.vertices.push_back(li1.second);
            for (int i=li1.first+1; i<=li0.first+fiter->vertices.size(); i++) {
                f1.vertices.push_back(*(fiter->vertices.begin()+(i%fiter->vertices.size())));
            }
            f1.vertices.push_back(li0.second);
            f1.color = randomHue();
            f1.cuts = fiter->cuts;
            f1.cuts.push_back(cut(numCuts+1,li1.second,li0.second));
            
            fiter = faces.erase(fiter);
            fiter = faces.insert(fiter, f0);
            fiter++;
            fiter = faces.insert(fiter, f1);
            
        } 
    }
    
    if (counter) {
        numCuts++;
    }
    
    
}

void ofxOrigami::draw() {
    ofPushStyle();
    ofFill();
    for (list<face>::iterator fiter=faces.begin();fiter!=faces.end();fiter++) {
        ofBeginShape();
        ofSetColor(fiter->color.r, fiter->color.g, fiter->color.b);
        
        for (vector<ofVec2f>::iterator iter=fiter->vertices.begin();iter!=fiter->vertices.end();iter++) {
            ofVertex(iter->x,iter->y);
        }
        
        ofEndShape();
    }
    ofPopStyle();
}

void ofxOrigami::dump() {
    for (list<face>::iterator fiter=faces.begin();fiter!=faces.end();fiter++) {
        cout << "face: " << distance(faces.begin(), fiter) << endl;
        
        for (vector<cut>::iterator iter=fiter->cuts.begin();iter!=fiter->cuts.end();iter++) {
            cout << "\tcut: " << iter->index <<"\t(" << iter->p0.x <<","<< iter->p0.y<<")\t(" << iter->p1.x <<","<<iter->p1.y<<")"<<endl;
        }
       
    }

    
}
