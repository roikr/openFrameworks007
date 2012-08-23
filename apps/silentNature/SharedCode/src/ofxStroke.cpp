//
//  ofxStroke.cpp
//  brushStrokesExample
//
//  Created by Roee Kremer on 8/16/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxStroke.h"

vector<ofVec2f> catmullRomPatch(vector<ofVec2f> &curveVertices,int curveResolution) {
    
    vector<ofVec2f> points;
    
	if (curveVertices.size() == 4){
        
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
    }
    
    
    return points;
}



void ofxStroke::setup(float spacing) {
    curve.clear();
    this->spacing = spacing;
}

void ofxStroke::addPatch(vector<ofVec2f> ctrl) {
    float dist = (ctrl[2]-ctrl[1]).length();
    vector<ofVec2f> pnts = catmullRomPatch(ctrl,dist/spacing);
    curve.insert(curve.end(), pnts.begin(), pnts.end());
}

vector<ofVec2f> &ofxStroke::getCurve() {
    return curve;
}




