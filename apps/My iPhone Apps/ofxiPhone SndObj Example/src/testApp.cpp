#include "AudioDefs.h" // must define PI before of
#include "testApp.h"
#include "ofxXmlSettings.h"

#include "Player.h"

#include "ofxRKUtils.h"

//--------------------------------------------------------------
void testApp::setup(){	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	// touch events will be sent to myTouchListener
	ofxMultiTouch.addListener(this);
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	// initialize all of the Thing particles
	//for(int i=0; i<NUM_POINTS; i++) balls[i].init();
	
	
	string inputfn = "ohYeahPlastic.xml"; 
	string outputfn = "ohYeahPlastic.wav";
	
	
	cout << endl;
	cout << "input file: " << inputfn << endl;
	cout << "output file: " << outputfn << endl;
	
	
	ofxXmlSettings xml;
	bool loaded = xml.loadFile("dead.xml");
	
	assert(loaded);
	if (!loaded)
		std::exit(0);
	
	Mixer mix;
	
	Player players[3];
	
	
	
	xml.pushTag("dead");
	xml.pushTag("guitar",0);
	players[0].setup(&xml,0,&mix);
	xml.popTag();
	
	xml.pushTag("vocal",0);
	players[1].setup(&xml,0,&mix);
	xml.popTag();
	
	xml.pushTag("drum",0);
	players[2].setup(&xml,0,&mix);	
	xml.popTag();
	xml.popTag();
	
	
	ofxXmlSettings midiXml;
	
	//if (bUseArgs) 
	//	ofDisableDataPath();
	
	midiXml.loadFile(inputfn);
	
	//ofEnableDataPath();
	
	midiXml.pushTag("MIDIFile");
	int ticksPerBeat = midiXml.getValue("TicksPerBeat", 96);
	for (int i=0; i<3; i++) {
		midiXml.pushTag("Track", i);
		players[i].setMidiTrack(&midiXml);
		midiXml.popTag();
	}
	midiXml.popTag();
	
	
	int currentFrame = 0;
	string filename = ofToDocumentsPath(outputfn);
	
	SndWave outputfile((char*)filename.c_str(), OVERWRITE);
	outputfile.SetOutput(1, &mix);
	
	bool bExit;
	
	do {
		int tick = currentFrame * DEF_VECSIZE * ticksPerBeat * (120 / 60) / (int) DEF_SR;
		//cout << "tick: " << tick << endl;
		bExit = true;
		for (int i=0; i<3; i++) {
			bExit = bExit && players[i].getIsSongDone();
			if (!bExit)
				break;
		}
		
		if (!bExit) {
			for (int i=0; i<3; i++) 
				players[i].update(tick);
			mix.DoProcess();
			outputfile.Write();
			
		}
		currentFrame++;
	} while (!bExit);		
	
	for (int i=0; i<3; i++) {
		players[i].exit();
	}
}


//--------------------------------------------------------------
void testApp::update()
{
	for(int i=0; i<NUM_POINTS; i++) balls[i].update();
}

//--------------------------------------------------------------
void testApp::draw()
{
	for(int i=0; i<NUM_POINTS; i++) balls[i].draw();
}

void testApp::exit() {
	printf("exit()\n");
}

//--------------------------------------------------------------
void testApp::touchDown(float x, float y, int touchId, ofxMultiTouchCustomData *data){
	printf("touch %i down at (%i,%i)\n", touchId, x,y);
	balls[touchId].moveTo(x, y);
}
//--------------------------------------------------------------
void testApp::touchMoved(float x, float y, int touchId, ofxMultiTouchCustomData *data){
	printf("touch %i moved at (%i,%i)\n", touchId, x,y);
	balls[touchId].moveTo(x, y);
}
//--------------------------------------------------------------
void testApp::touchUp(float x, float y, int touchId, ofxMultiTouchCustomData *data){
	printf("touch %i up at (%i,%i)\n", touchId, x,y);
}
//--------------------------------------------------------------
void testApp::touchDoubleTap(float x, float y, int touchId, ofxMultiTouchCustomData *data){
	printf("touch %i double tap at (%i,%i)\n", touchId, x,y);
}
//--------------------------------------------------------------
void testApp::lostFocus()
{
	cout<<"lostFocus"<<endl;
}
//--------------------------------------------------------------
void testApp::gotFocus()
{
	cout<<"gotFocus"<<endl;
}
//--------------------------------------------------------------
void testApp::gotMemoryWarning()
{
	cout<<"MemoryWarning"<<endl;
}

