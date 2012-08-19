//
//  ofxDraggable.h
//  imageLoaderExample
//
//  Created by Roee Kremer on 6/14/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"

class ofxDraggable {
  public:
    void setup(int width,int height,ofMatrix4x4 mat);
    void draw();
    void begin();
    void end();
    

    void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);
    
    bool inside(ofTouchEventArgs &touch);
    
    vector<ofTouchEventArgs> touches;
    
    
    ofMatrix4x4 mat;
    
    ofRectangle rect;
    
};
