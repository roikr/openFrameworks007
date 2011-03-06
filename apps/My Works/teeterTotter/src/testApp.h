#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"



class testApp : public ofBaseApp {
	public:

		void setup();
		void update();
		void draw();
		void exit();
	
		void drawPointCloud();

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

		ofxKinect kinect;

		ofxCvColorImage		colorImg;
		ofxCvGrayscaleImage 	grayImage;
		ofxCvContourFinder 	contourFinder;
		
		
		bool				drawPC;
		int					angle;
		int 				pointCloudRotationY;
	
		
		void saveImage();
		
		vector<ofxVec3f> points;
		vector<ofxVec3f> normals;
		
		int mode;
		int displayMode;
	
		ofRectangle roi;
		
};
