//
//  ofxStroke.cpp
//  brushStrokesExample
//
//  Created by Roee Kremer on 8/16/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxStroke.h"

void catmullRomPatch(vector<ofVec2f > curveVertices,int curveResolution,vector<ofVec2f> &points) {
    
    float x0 = curveVertices[0].x;
    float y0 = curveVertices[0].y;
    
    float x1 = curveVertices[1].x;
    float y1 = curveVertices[1].y;
    
    float x2 = curveVertices[2].x;
    float y2 = curveVertices[2].y;
    
    float x3 = curveVertices[3].x;
    float y3 = curveVertices[3].y;
    
    float t,t2,t3;
    float x,y;
    
//    cout << curveResolution << endl;
    
    
    for (int i = 0; i < curveResolution; i++){
        
        t 	=  (float)i / (float)(curveResolution-1);
        t2 	= t * t;
        t3 	= t2 * t;
        
        x = 0.5f * ( ( 2.0f * x1 ) +
                    ( -x0 + x2 ) * t +
                    ( 2.0f * x0 - 5.0f * x1 + 4 * x2 - x3 ) * t2 +
                    ( -x0 + 3.0f * x1 - 3.0f * x2 + x3 ) * t3 );
        
        y = 0.5f * ( ( 2.0f * y1 ) +
                    ( -y0 + y2 ) * t +
                    ( 2.0f * y0 - 5.0f * y1 + 4 * y2 - y3 ) * t2 +
                    ( -y0 + 3.0f * y1 - 3.0f * y2 + y3 ) * t3 );
        
        //			z = 0.5f * ( ( 2.0f * z1 ) +
        //                        ( -z0 + z2 ) * t +
        //                        ( 2.0f * z0 - 5.0f * z1 + 4 * z2 - z3 ) * t2 +
        //                        ( -z0 + 3.0f * z1 - 3.0f * z2 + z3 ) * t3 );
        
        points.push_back(ofVec2f(x,y));
    }
    
    
    
    return points;
}


void ofxStroke::setup(float spacing) {
    this->spacing = spacing;
    
}

void ofxStroke::clear() {
    curve.clear();
    velocities.clear();
    touches.clear();
    touches2.clear();
}

void ofxStroke::touch(ofVec2f pos) {
        
    
    touches.push_back(pos);
    
    if (touches.size()==3) {
        touches2.push_back(touches[2]-touches[1]);
    }
    
    if (touches.size()==1) {
        touches2.push_back(ofVec2f(30,30));
        
        curve.push_back(pos);
        velocities.push_back(touches2.front());
    }
    
    if (touches.size()==2) {
        ofVec2f diff(touches[1]-touches[0]);
        touches2.push_back(diff);
        if (diff.length()<spacing) {
            curve.push_back(touches[1]);
            velocities.push_back(diff);
            
            touches.pop_front();
            touches2.pop_front();
        } 
    }
    
    
    
    if (touches.size()==4) {
        touches2.push_back(touches[3]-touches[2]);
        int resolution = ceil((touches[2]-touches[1]).length()/spacing);
        catmullRomPatch(vector<ofVec2f>(touches.begin(),touches.end()),resolution,curve);
        catmullRomPatch(vector<ofVec2f>(touches2.begin(),touches2.end()), resolution, velocities);
        touches.pop_front();
        touches2.pop_front();
    }
    
    
    
    if (touches.size()==3) {
        ofVec2f diff(touches[2]-touches[1]);
        if (diff.length()<spacing) {
            curve.push_back(touches[1]);
            curve.push_back(touches[2]);
            velocities.push_back(touches[1]-touches[0]);
            velocities.push_back(diff);
            
            touches.pop_front();
            touches.pop_front();
            touches2.pop_front();
            touches2.pop_front();
        }
    }
    
//    cout << touches.size() << "\t" << touches2.size() << endl;;
    
}





