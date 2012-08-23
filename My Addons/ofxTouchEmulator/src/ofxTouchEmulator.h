//
//  ofxTouchEmulator.h
//  osxKaiserNav
//
//  Created by Roee Kremer on 7/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"

class ofxTouchEmulator {
    
public:
    ofxTouchEmulator():bMove(false) {};
    
    void keyPressed  (int key);
    ofTouchEventArgs mouseDragged(int x, int y);
    ofTouchEventArgs mousePressed(int x, int y, int button);
    ofTouchEventArgs mouseReleased(int x, int y, int button);
    
    bool getShouldMove();
    
private:
        
    vector<int> buttons;
    int current;
    bool bMove;
    
    
};