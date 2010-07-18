#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "cv.h"
#include "cvaux.h"


class testApp : public ofBaseApp{
	
public:
	
	void setup();
	void update();
	void draw();
	
	void keyPressed  (int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void resized(int w, int h);
	
	
	ofVideoGrabber       vidGrabber;
	
	ofxCvColorImage colorImg;
	int imgHeight, imgWidth, imgDrawHeight, imgDrawWidth;
	
	ofxCvGrayscaleImage hueImg;
	ofxCvGrayscaleImage satImg;
	ofxCvGrayscaleImage briImg;
	color grn;
	unsigned char *  colorTrackedPixelsGreen;
	ofTexture trackedTextureGreen;	
};

#endif