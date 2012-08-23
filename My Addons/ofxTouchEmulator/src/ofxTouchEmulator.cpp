//
//  ofxTouchEmulator.cpp
//  osxKaiserNav
//
//  Created by Roee Kremer on 7/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxTouchEmulator.h"


//--------------------------------------------------------------
void ofxTouchEmulator::keyPressed(int key){
    bMove = false;
    if (find(buttons.begin(),buttons.end(),key-'1')!=buttons.end()) {
        bMove = true;
        current = key-'1';
    }
}



//--------------------------------------------------------------
ofTouchEventArgs ofxTouchEmulator::mouseDragged(int x, int y) {
//    cout << x << "\t" << y << "\t" << current << endl;
    
    ofTouchEventArgs touch;
    touch.x = x;
    touch.y = y;
    touch.id = current;
    touch.numTouches = buttons.size();
    
    return touch;
    
}

//--------------------------------------------------------------
ofTouchEventArgs ofxTouchEmulator::mousePressed(int x, int y, int button) {
    
    
    buttons.push_back(button);
    if (!bMove) {
        current = button;
        bMove = true;
    }
    
    
    ofTouchEventArgs touch;
    touch.x = x;
    touch.y = y;
    touch.id = button;
    touch.numTouches = buttons.size();
    
    return touch;
    
}

//--------------------------------------------------------------
ofTouchEventArgs ofxTouchEmulator::mouseReleased(int x, int y, int button) {
    
    ofTouchEventArgs touch;
    touch.x = x;
    touch.y = y;
    touch.id = button;
    touch.numTouches = buttons.size();
    
    
    if (bMove && button == current) {
        bMove = false;
    }
    
    
    vector<int>::iterator iter=find(buttons.begin(),buttons.end(),button);
    if (iter!=buttons.end()) {
        buttons.erase(iter);
    }
    
    return touch;
}

bool ofxTouchEmulator::getShouldMove() {
    return bMove;
}