#include "testApp.h"
#include "ofMainExt.h"

#include "ofxXmlSettings.h"


//--------------------------------------------------------------
void testApp::setup(){	
	ofDisableDataPath();
	// register touch events
	ofxRegisterMultitouch(this);
		
	//ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);

	counter = 0;
	ofSetCircleResolution(50);
	ofBackground(0,0,0);
	bSmooth = false;
	
	
	//image.loadImage("esb2.jpg");
	//texture.load(ofToResourcesPath("nyc.pvr"));
	//pincher.setup(ofPoint(0,-140),0.7,pincherPrefs(480,320,ofRectangle(0,0,700,700),0.5,1.5));
	//pincher.setup(ofPoint(0,0),0.3,pincherPrefs(480,320,ofRectangle(0,0,1628,1085),0.1,15));
	//pincher.setup(ofPoint(0,0),0.3,pincherPrefs(480,320,ofRectangle(0,0,2912,4368),0.1,15));
	//pincher.setup(ofPoint(0,0),0.3,pincherPrefs(320,480,ofRectangle(0,0,728,1092),0.1,10));
	pincher.setup(ofPoint(0,0),0.3,pincherPrefs(320,480,ofRectangle(0,0,1365,2048),0.234,10));
	
	//pincher.setup(ofPoint(0,0),0.5,pincherPrefs(480,320,ofRectangle(0,0,700,700),0.5,1));
	ofSetFrameRate(60);
	
	int bufferSize = 512;
	int sampleRate = 44100;
	
	lAudio	= new float[bufferSize];
	rAudio	= new float[bufferSize];
	
	song.setup(bufferSize, sampleRate);
	song.loadTrack(ofToResourcesPath("wedding_march_01.mid"));
	
	ofxXmlSettings xml;
	
	bool loaded = xml.loadFile(ofToResourcesPath("doron.xml"));
	assert(loaded);
	
	
	int i;
	
	xml.pushTag("doron");
	
	xml.pushTag("layers");
	for (i=0; i<xml.getNumTags("layer");i++) {
		
		int z = xml.getAttribute("layer", "z", 0, i) ;
		
		if (!z) {
			background.load(ofToResourcesPath(xml.getAttribute("layer","filename","",i)));
			continue;
		} 
		
		layer l;
		l.x = xml.getAttribute("layer", "x", 0, i) ;
		l.y = xml.getAttribute("layer", "y", 0, i) ;
		l.scale = xml.getAttribute("layer", "scale", 1.0f, i) ;
		l.degree = xml.getAttribute("layer", "degree", 0.0f, i) ;
		l.texture = new ofxiTexture;
		l.texture->load(ofToResourcesPath(xml.getAttribute("layer","filename","",i)));
		
		layers.push_back(l);
	}
	
	xml.popTag();
	
	map<string,int> bases;
	
	xml.pushTag("videos");
	ofxiVideoLoader loader;
	loader.setup();

	for (i=0; i<xml.getNumTags("video");i++) {
		
		ofxiPhoneVideo *video = new ofxiPhoneVideo;
		
		string name = xml.getAttribute("video","name","",i);
		
		video->bufferSize = bufferSize;
		video->fps = 24;
		video->numIntroFrames = 0;
		video->sampleRate 			= sampleRate;
		
		video->widthFraction = 1.0;
		video->heightFraction = 1.0;
		video->textureWidth = 128;
		video->textureHeight = 256;
		
		
		
		
		video->firstFrame = 0;
		
		//video->bHorizontal = false;
		
		string filename = xml.getAttribute("video","filename","",i);
		video->numFrames =  xml.getAttribute("video","numFrames",0,i); // iter->first->textures.size(); 
		video->sampleLength = 1000*video->numFrames/video->fps;
		printf("loading %s, numFrames: %i\n",filename.c_str(),video->numFrames);
		loader.addVideo(video);
		loader.loadAudio(video, ofToResourcesPath(filename));
		video->audio.normalize();
				
		videos[name] = video;
		bases[name] = xml.getAttribute("video","base",60,i);
	}
	
	loader.loadVideo(ofToResourcesPath("doron.mov"));
	
	xml.popTag();
	
	
	
	xml.pushTag("actors");
	for (i=0; i<xml.getNumTags("actor");i++) {
		
				
		actor a;
		a.x = xml.getAttribute("actor", "x", 0, i) ;
		a.y = xml.getAttribute("actor", "y", 0, i) ;
		a.scale = xml.getAttribute("actor", "scale", 1.0f, i) ;
		a.degree = xml.getAttribute("actor", "degree", 0.0f, i) ;
		a.midi  = xml.getAttribute("actor", "note", 60, i);
		
		string videoName = xml.getAttribute("actor", "video", "", i);
		ofxiPhoneVideo *video = videos[videoName];
		
		a.base = bases[videoName];
		
		
		map<int,note>::iterator iter = notes.find(a.midi);
	
		
		assert(video);
		
		if (iter==notes.end())  {
			note n;
			
			n.base = a.base;
			n.player = new ofxiVideoPlayer;
			n.player->setup(video,false);
			n.pan = 0.5;
			notes[a.midi] = n;
			//a.pan = (float)a.x/(float)(ofGetWidth());
			
			//				minX = min(p.x,minX);
			//				maxX = max(p.x,maxX);
		}
		
		a.player = notes[a.midi].player;
		actors.push_back(a);
	}
		
		
		
	xml.popTag();
	
	
	
		
		
	xml.popTag();
	
	
	ofSoundStreamSetup(2, 0, this, sampleRate, bufferSize, 2);
	
	bSongPlaying = false;
	
}

//--------------------------------------------------------------
void testApp::update(){
	//image.update();
	for (vector<actor>::iterator iter=actors.begin(); iter!=actors.end(); iter++) { 
		iter->player->update();
	}
	
	counter = counter + 0.033f;
	if (pincher.getIsAnimating()) {
		pincher.update((float)(ofGetElapsedTimeMillis() - animStart)/250.0);
	}
}

//--------------------------------------------------------------
void testApp::draw(){

	pincher.transform();
	
	//image.draw(0,0);
	background.draw();
	
	
	for (vector<actor>::iterator iter=actors.begin(); iter!=actors.end(); iter++) { 
		ofPushMatrix();
		ofTranslate(iter->x, iter->y, 0);
		ofScale(iter->scale, iter->scale, 1);
		iter->player->draw();
		ofPopMatrix();
	}
	
	//ofEnableAlphaBlending();
	 
	
	for (vector<layer>::iterator iter=layers.begin(); iter!=layers.end(); iter++) { 
		ofPushMatrix();
		ofTranslate(iter->x, iter->y, 0);
		ofScale(iter->scale, iter->scale, 1);
		iter->texture->draw();
		ofPopMatrix();
	}
	
	
	//ofDisableAlphaBlending();
	
	//--------------------------- rectangles
//	ofFill();
//	for (int i = 0; i < 200; i++){
//		ofSetColor((int)ofRandom(0,255),(int)ofRandom(0,255),(int)ofRandom(0,255));
//		ofRect(ofRandom(250,350),ofRandom(250,350),ofRandom(10,20),ofRandom(10,20));
//	}
//	ofSetColor(0x000000);
//	ofDrawBitmapString("rectangles", 275,500);
//
//
//	pincher.draw();

}



void testApp::touchDown(int x, int y, int id) {
	
	pincher.touchDown(x, y, id);	
	
	ofPoint trans = pincher.touchToPoint(x, y);
//	printf("x: %.0f, y: %.0f\n",trans.x,trans.y);
	
	for (vector<actor>::iterator iter=actors.begin(); iter!=actors.end(); iter++) { 
		if (trans.x > iter->x && trans.x < iter->x+12 && trans.y>iter->y && trans.y<iter->y+18) {
			
			iter->player->play( 60+iter->midi-iter->base,127 );
			printf("x: %.0f, y: %.0f, note: %i\n",trans.x,trans.y,iter->midi);
		}
	}
		
	
	//player.play(60,127);
	bSmooth = true;
	

}


void testApp::touchMoved(int x, int y, int id) {
	
	pincher.touchMoved(x, y, id);
	
	float minX = (pincher.touchToPoint(0, 0)).x;
	float maxX = (pincher.touchToPoint(ofGetWidth(), 0)).x;

	printf("min: %.0f, max: %.0f\n",minX,maxX);
	
	//for (vector<actor>::iterator iter=actors.begin(); iter!=actors.end(); iter++) { 
//		iter->pan = (min(maxX, max(minX,(float)iter->x)) - minX)/ (maxX-minX);
//	}
	
}

void testApp::touchUp(int x, int y, int id) {
	bSmooth = false;
	
		
	pincher.touchUp(x, y, id);
	
}

void testApp::touchDoubleTap(int x, int y, int id) {
//	if (!pincher.getIsAnimating()) {
//		animStart = ofGetElapsedTimeMillis();
//		pincher.touchDoubleTap(x,y,id);
//	}
	
	ofPoint trans = pincher.touchToPoint(x, y);
	for (vector<actor>::iterator iter=actors.begin(); iter!=actors.end(); iter++) { 
		if (trans.x > iter->x && trans.x < iter->x+12 && trans.y>iter->y && trans.y<iter->y+18) {
			return;
		}
	}
	
	if (!bSongPlaying) {
		bSongPlaying = true;
		song.play();
	} else {
		if (song.getIsPlaying()) {
			song.stop();
			
		}
		bSongPlaying = false;
	}

	
}


void testApp::audioRequested( float * output, int bufferSize, int nChannels ) {
	memset(lAudio, 0, bufferSize*sizeof(float));
	memset(rAudio, 0, bufferSize*sizeof(float));
	
	
	
	if (bSongPlaying) {
	
		vector<event> events;
		song.process(events);
	
		for (vector<event>::iterator eiter=events.begin(); eiter!=events.end(); eiter++) {
			if (eiter->bNoteOn) {
				map<int,note>::iterator niter = notes.find(60+(eiter->note)%12);
				if (niter!=notes.end()) {
					
					niter->second.player->play(60+eiter->note-niter->second.base,127);
					
										
				}
			}
		}
	}
	
	float numPoly = 9; //(float)actors.size()
	
	for (map<int,note>::iterator iter=notes.begin(); iter!=notes.end(); iter++) { 
		//float pan = 0.5;
		
		float leftScale = 1 - iter->second.pan;
		float rightScale = iter->second.pan;
		iter->second.player->mix(lAudio, bufferSize,leftScale/numPoly);
		iter->second.player->mix(rAudio, bufferSize,rightScale/numPoly);
		iter->second.player->preProcess();
	}
	
	
	
	for (int i = 0; i < bufferSize; i++){
		output[i*nChannels] = lAudio[i];// * gain;
		output[i*nChannels + 1] = rAudio[i];// * gain;
	}
	
}



