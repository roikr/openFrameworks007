#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    int width =ofGetWidth()/2;
    int height = ofGetHeight()/2;
    
    image.allocate(width, height, OF_IMAGE_COLOR);
    
    buffers.push_back(new int[width*height]);
    buffers.push_back(new int[width*height]);
    memset(buffers.front(), 0, width*height);
    memset(buffers.back(), 0, width*height);
    
    counter = 0;
}

//--------------------------------------------------------------
void testApp::update(){
    
    int *buffer0= buffers[counter % buffers.size()];
    int *buffer1 = buffers[(counter+1) % buffers.size()];
    
    int width = image.getWidth();
    int height = image.getHeight();
   
    
    for (int i=width;i<width*(height-1);i++) {
        int pixel = (buffer0[i-1] + buffer0[i+1] + buffer0[i+width] + buffer0[i-width])/2-buffer1[i];
        pixel -= pixel >> 20;
        buffer1[i]  = pixel;
    }
    
    ofColor color;
    
    for (int y=0;y<image.height; y++) {
        for (int x=0; x<image.width; x++) {
            color.setHsb(150, 100, ofClamp(buffer1[y*width+x], 0, 255));
            image.getPixelsRef().setColor(x, y, color);
        }
    }
        
    
    
    image.update();
    
    counter++;
}

//--------------------------------------------------------------
void testApp::draw(){
    image.draw(0, 0, ofGetWidth(), ofGetHeight());
    
    
    
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    int ix = x * image.getWidth()/ofGetWidth();
    int iy = y*image.getHeight()/ofGetHeight();
    
    
    buffers[counter % buffers.size()][iy*(int)image.getWidth()+ix]= 1500;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}