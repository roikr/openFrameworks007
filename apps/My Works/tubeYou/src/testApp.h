#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxQtVideoSaver.h"

#define RECORD_MODE 0
#define PLAY_MODE 1

#define RECEIVE_PORT 12345

#define HOST "localhost"
#define SEND_PORT 12346

#define GROUPS_NUMBER 4
#define OUTPUTS_NUMBER 4

#define FRAMES_NUMBER 500
#define FRAME_RATE 15


#define CAM_WIDTH 320
#define CAM_HEIGHT 240
#define TOTAL_PIXELS (CAM_WIDTH * CAM_HEIGHT * 3)

struct videoGroup {
	string name;
	string message;
	ofVideoPlayer video;
	
	
};

struct videoOutput {

	videoOutput() {};
	videoOutput(float x0,float y0,float x1,float y1,int group) : x(x0),y(y0),width(x1-x0),height(y1-y0),group(group) {}
	videoOutput(const videoOutput &output) : x(output.x),y(output.y),width(output.width),height(output.height),group(output.group) {}
	
	float x;
	float y;
	float width;
	float height;
	int group;
};
	


class testApp : public ofBaseApp{
	
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void resized(int w, int h);
	
		ofVideoGrabber 		vidGrabber;
		
		float x,y;
		int px,py;
		
		videoGroup groups[GROUPS_NUMBER];
		videoOutput preview;
		videoOutput outputs[OUTPUTS_NUMBER];

		int startTime;
		int lastFrameNumber;

		int recordGroup;
		int recordVideo;
		int mode;
		string folder;
		
		bool bRecording;
	
		ofxOscReceiver	receiver;
		ofxOscSender sender;
	
		ofxQtVideoSaver		saver;
	
		ofImage text[3],background,icon,overlay,rec_overly;
		ofVideoPlayer bar;
		int currentText;
		int barFrame;

};

#endif	
