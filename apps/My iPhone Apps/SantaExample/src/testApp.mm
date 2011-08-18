
#include "testApp.h"
#include "Constants.h"
#include "ofSoundStream.h"
#include "ofxXmlSettings.h"



#include "ofxiVideoPlayer.h"
#include "ofxAudioPlayer.h"

#include <iostream>
#include "ofMain.h"

#include "ofMainExt.h"

#include "ofxiPhoneExtras.h"


//--------------------------------------------------------------
void testApp::setup(){	
	
	
	// register touch events
	ofRegisterTouchEvents(this);
	
	//If you want a landscape oreintation 
	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
	// initialize the accelerometer
	//ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	//ofxiPhoneAlerts.addListener(this);
	
	ofBackground(255,255,255);
	ofSetFrameRate(60);
	
	
	
	
	int bufferSize = 512;
	int nChannels = 2;
	
	video.fps = 25;
	video.numIntroFrames = 4;
	video.numFrames = 25;
	video.sampleLength = 1000*video.numFrames/video.fps;
	
	video.widthFraction = 1.0;
	video.heightFraction = 1.0;
	video.textureWidth = 128;
	video.textureHeight = 256;
	video.bHorizontal = true;
	video.bFlipHoriznotal = false;
	
	
	sampleRate 			= 44100;
	
	sampler.setup(bufferSize,sampleRate * video.sampleLength / (1000 * bufferSize));
	
	
	//video->sample				= new float[video->numBuffers*video->bufferSize];
	//memset(video->sample, 0, video->numBuffers*video->bufferSize * sizeof(float));
	
	song.setupForSave(bufferSize);
	
	ofxXmlSettings xml;
	
	bool loaded = xml.loadFile("cards.xml");
	assert(loaded);
	
	
	int i;
	
	xml.pushTag("cards");
	
	
	sliderPrefs prefs;
	
	for (int j=0; j<xml.getNumTags("card");j++) {
		card c;
		
//			int maxX,minX;
//			maxX = 0;
//			minX = ofGetWidth();
		
		c.background = new ofxRKTexture;
		c.background->setup(ofToDataPath(xml.getAttribute("card", "image", "", j)));
		c.background->init();
		c.background->load();
		
		xml.pushTag("card", j);

		for (i=0; i<xml.getNumTags("actor");i++) {
			
			actor a;
			a.x = xml.getAttribute("actor", "x", 0, i) ;
			a.y = xml.getAttribute("actor", "y", 0, i) ;
			a.scale = xml.getAttribute("actor", "scale", 1.0f, i) ;
			a.degree = xml.getAttribute("actor", "degree", 0.0f, i) ;
			a.pan = (float)a.x/(float)(ofGetWidth());
			
//				minX = min(p.x,minX);
//				maxX = max(p.x,maxX);
			c.actors.push_back(a);
		}
	
		xml.popTag();
	
		cards.push_back(c);
		prefs.pages.push_back(ofPoint(0,ofGetHeight()*j));
		
	}
							  
	prefs.direction = SLIDER_VERTICAL;
	slider.setup(1,prefs);	
	bSlide = false;
		
	xml.pushTag("players");
	
	for (i=0; i<xml.getNumTags("player");i++) {
		
		player p;
		p.song.setup(bufferSize, sampleRate);
		p.song.loadTrack(ofToDataPath(xml.getAttribute("player", "filename", "", i)));
		p.video = new ofxiVideoPlayer;
		p.video->setup(&video,true);
		p.audio = new ofxAudioPlayer;
		p.audio->setup(sampler.getAudioSample(),bufferSize);
		players.push_back(p);
	}
	
	xml.popTag();
	
	xml.popTag();
	
	citer = cards.begin();
	
	
	
//	for (vector<player>::iterator iter=players.begin(); iter!=players.end(); iter++) {
//		iter->pan = (float)(iter->x - minX)/(float)(maxX-minX);
//		cout << iter-> pan << endl;
//	}
	
	
//	lAudio	= new float[video.audio.getBufferSize()];
//	rAudio	= new float[video.audio.getBufferSize()];
//	buffer = new float[bufferSize*nChannels];
	
	sample.nChannels = 2;
	sample.numFrames = bufferSize;
	sample.buffer	= new float[sample.numFrames];
	
//	camera = new ofxiVideoGrabber;
	grabber.setup(&video,FRONT_CAMERA);
	
	
	
	songState = SONG_IDLE;
	bRecording = false;
	trigger.setAutoThresh(0.1,50);
	//trigger.setThresh(0.15);
	
	ofSoundStreamSetup(nChannels, 1, this, sampleRate, bufferSize, 2);
	
	grabber.startCamera();
	
	
	
	songVersion = 0;
	
	//camera->startCapture(); // TODO: remove this - just for testing	
}




//--------------------------------------------------------------
void testApp::update()
{
			

	slider.update();
	
	switch (songState) {
		case SONG_IDLE: 
			for (vector<player>::iterator iter=players.begin(); iter!=players.end(); iter++) { 
				iter->video->update();
			}
			
			
			break;
		case SONG_PLAY:
			for (vector<player>::iterator iter=players.begin(); iter!=players.end(); iter++) { 
				iter->video->update();
			}
			if (! getIsPlaying()) {
				
				setSongState(SONG_IDLE);
				
				//bNeedDisplay = true;
			}
			break;
		case SONG_RENDER_AUDIO:
		case SONG_CANCEL_RENDER_AUDIO:
			if (! getIsPlaying()) {
				
				songState = SONG_IDLE;
				
								
				//bNeedDisplay = true;
			}
			break;
			
		case SONG_RENDER_VIDEO:
			
			if  (currentBlock / totalBlocks >= 1.0) {
				setSongState(SONG_IDLE);
				//songState = SONG_IDLE; // TODO: check why not notifying players...
				//bNeedDisplay = true;
			}
			
			
			break;
			
			
		default:
			break;
	}
	
}


//--------------------------------------------------------------
void testApp::draw()
{
	
	
	grabber.render(ofPoint((grabber.getCameraWidth()-video.textureWidth)/2,(grabber.getCameraHeight()-video.textureHeight)/2));
	
	
	
//	if (players.front().video->getIsPlaying()) {
//		players.front().video->draw();
//	} else {
//		camera->draw();
//	}

	
//	if (camera->getState()==CAMERA_CAPTURING || camera->getState()==CAMERA_RECORDING) {
//		camera->draw();
//	} else {
//		players.front().video->draw();
//	}
//	
//	
//	
//	trigger.draw();
//	
//	return;
	
	
	
	ofBackground(0, 0, 0);
	ofSetColor(255,255,255,255);
	
	
	slider.transform();
	
	vector<card>::iterator cbegin,cend;
	cbegin = citer;
	cend = citer;
	
	if (slider.getIsDragging() || slider.getIsAnimating()) {
		if (citer>cards.begin()) {
			cbegin=citer-1;
		}
		
		if(cards.size()>1 && citer<cards.end()-1) {
			cend=citer+1;
		}
	}
	
	vector<player>::iterator piter;
	vector<actor>::iterator aiter;
	vector<card>::iterator iter;
	
	
	
	for (iter=cbegin; iter<=cend; iter++) {
		
		int y = distance(cards.begin(), iter) *ofGetHeight();
		//cout << y <<endl;
		
		for (piter=players.begin(),aiter=iter->actors.begin(); piter!=players.end(); piter++,aiter++)  {
			ofPushMatrix();
			//glLoadIdentity();
			
			
			ofTranslate(aiter->x, aiter->y+y, 0);
			ofRotate(aiter->degree);
			ofScale(aiter->scale,aiter->scale,1.0);
			
			//ofTranslate(i % 2 * ofGetWidth()/2, (int)(i / 2) * ofGetHeight() /2);
			//ofScale(0.5, 0.5, 1);
			
			if (grabber.getState()==CAMERA_CAPTURING || grabber.getState()==CAMERA_RECORDING) {
				grabber.draw();
			} else {
				piter->video->draw();
			}
			
			//if (iter->video->getIsPlaying()) {
	//			iter->video->draw();
	//		} else {
	//			camera->draw();
	//		}
			ofPopMatrix();
		
		}
		
		ofEnableAlphaBlending();
		
		ofPushMatrix();
		ofTranslate(0, y); 
		iter->background->draw(0, 0);
		ofPopMatrix();
		
		ofDisableAlphaBlending();
	}
		
	trigger.draw();
	
				
}

//--------------------------------------------------------------
void testApp::exit() {
	grabber.exit();
}


void testApp::record() {
	trigger.setTrigger();
	trigger.resetMeters();
	grabber.startCapture();
	
	songVersion++;
	//camera->setTrigger(thresh);
	//camera->startRecording();
	//cout << "Start recording" << endl;
}

void testApp::preview() {
	
	vector<player>::iterator piter;
	vector<actor>::iterator aiter;
	
	int major[3] = {0,4,3};

	
	for (piter=players.begin(),aiter=citer->actors.begin(); piter!=players.end() ; piter++,aiter++)  {
		
		
		sampleInstance si;
		si.speed = exp((float)(major[distance(players.begin(),piter)])/12.0*log(2.0));
		float volume = 1.0f/players.size();
		si.left = volume * (1 - aiter->pan);
		si.right = volume * aiter->pan;
		si.retrigger = true;
		
		//piter->video->play(niter->note,niter->velocity);
		piter->video->play(si.speed);
		
		piter->audio->trigger(si);
					
		
		
		
		
	}
	
	
	
}

void testApp::playIntro() {
	for (vector<player>::iterator iter=players.begin(); iter!=players.end(); iter++)  {		
		
		iter->video->playIntro();
		
	}
	
}

bool testApp::getIsPlaying() {
	for (vector<player>::iterator iter=players.begin(); iter!=players.end(); iter++)  {		
		if (iter->video->getIsPlaying() || iter->song.getIsPlaying()) {
			return true;
		} 
	}
	return false;
}




int  testApp::getSongState() { 
	return songState;
}

void testApp::setSongState(int songState) {
	
		
	// song is valid and can Overwritten only when FINISHING RECORD
//	if (this->songState==SONG_RECORD && songState!=SONG_RECORD) {
//		songVersion++;
//	}
	
//	if (this->songState==SONG_RENDER_VIDEO && songState!=SONG_RENDER_VIDEO) {
//		currentFrame =(ofGetElapsedTimeMillis()-startTime)  / 40;
//	}
	
	this->songState = songState;
	
	if (songState == SONG_RENDER_AUDIO || songState == SONG_RENDER_VIDEO) {
		duration = 0;
		
		for (vector<player>::iterator piter=players.begin(); piter!=players.end(); piter++)  {
			duration = max(duration, piter->song.getDuration());
		}

	}		
	
	if (songState == SONG_RENDER_VIDEO) { 
		currentBlock = 0;
	}
	
//	for (int i=0;i<3;i++) {
//		player[i].setSongState(songState);
//	}
	
	
	
	switch (songState) {
		case SONG_IDLE:
		case SONG_RENDER_AUDIO_FINISHED:
		case SONG_CANCEL_RENDER_AUDIO:
			songState = SONG_IDLE;
			for (vector<player>::iterator iter=players.begin(); iter!=players.end(); iter++)  {
				iter->song.stop();
			}
			break;
		case SONG_PLAY:
		case SONG_RENDER_AUDIO:
		case SONG_RENDER_VIDEO:
			for (vector<player>::iterator iter=players.begin(); iter!=players.end(); iter++)  {
				iter->song.play();
			}
			break;
		default:
			break;
	}
	
	bNeedDisplay = true;
}


int testApp::getSongVersion() {
	return songVersion;
}

void testApp::soundStreamStart() {
	ofSoundStreamStart();
}

void testApp::soundStreamStop() {
	ofSoundStreamStop();
}




//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	//touchDown(touch.x, touch.y, touch.id);
	if (touch.x > ofGetWidth()-30) {
		
		switch ((int)(4*touch.y/(ofGetHeight()+1))) {
			case 0:
				record();
				break;
			case 1:
				preview();
				break;
				
			case 2:
				setSongState(SONG_PLAY);
				break;
				
			case 3:
				ofSoundStreamStop();
				renderAudio();
				ofSoundStreamStart();
				break;
			default:
				break;
		}  
		
	} else if (touch.x < 30) {
		trigger.setThresh(1-(touch.y/ofGetHeight()));
	} else {
		bSlide = true;
		slider.touchDown(touch.x, touch.y,touch.id);
	}

	
	
	
}



//--------------------------------------------------------------

void testApp::touchMoved(ofTouchEventArgs &touch)	{	
	//touchMoved(touch.x, touch.y, touch.id);
	if (bSlide) {
		slider.touchMoved(touch.x, touch.y,touch.id);
	}
	
}





//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){	
	//touchUp(touch.x, touch.y, touch.id);
	if (touch.x > ofGetWidth()-30) {
		
		switch ((int)(4*touch.y/(ofGetHeight()+1))) {
			case 0:
				
				break;
			case 1:
				
				break;
				
			case 2:
				setSongState(SONG_IDLE);
				break;
			default:
				
				break;
		}  
		
	} 
	
	if (bSlide) {
		slider.touchUp(touch.x, touch.y,touch.id);
		citer=cards.begin()+slider.getCurrentPage();
		bSlide = false;
		
	}

}



//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
	
}
	


void testApp::audioReceived( float * input, int bufferSize, int nChannels ) {
	
	trigger.audioReceived(input,bufferSize);
	
	
	
	if (trigger.getTriggerState() == TRIGGER_TRIGGERED) {
		trigger.resetTrigger();
		grabber.record();
		sampler.record();
		bRecording = true;
	}
	
	//compressor.audioProcess(input,bufferSize);
	
//	camera->audioReceived(input, bufferSize);
//	
//	if (bRecording && !sampler.getIsRecording()) {
//		bRecording = false;
//		playIntro();
//		//gain = 0.6f/trigger.getRmsPeak();
//		//printf("rms: %1.2f, players: %i, gain: %1.2f\n", trigger.getRmsPeak(),(int)players.size(),gain);
//	}
	
	if (sampler.getIsRecording()) {
		sampler.audioReceived(input,bufferSize);
		if (!sampler.getIsRecording()) {
			sampler.normalize();
			bRecording = false;
		}
	}
}





void testApp::audioRequested( float * output, int bufferSize, int nChannels ) {
	
	memset(output, 0, bufferSize*sizeof(float)*nChannels);
	
	vector<player>::iterator piter;
	vector<actor>::iterator aiter;
	
	for (piter=players.begin(),aiter=citer->actors.begin(); piter!=players.end() ; piter++,aiter++)  {
		
		switch (songState) {
			case SONG_PLAY:
			case SONG_RENDER_AUDIO: {
				vector<event> events;
				piter->song.process(events);
				
				for (vector<event>::iterator niter=events.begin(); niter!=events.end(); niter++) {
					if (niter->bNoteOn) {
						printf("player: %i, ", distance(players.begin(),piter));
						sampleInstance si;
						si.speed = exp((float)(niter->note-60)/12.0*log(2.0));
						float volume = (float)niter->velocity / 127* 1.0f/players.size();
						si.left = volume * (1 - aiter->pan);
						si.right = volume * aiter->pan;
						si.retrigger = true;
						
						//piter->video->play(niter->note,niter->velocity);
						piter->video->play(si.speed);
						
						piter->audio->trigger(si);
					}
				}
				
			} break;
				
			default:
				break;
		}
		
		
		
		piter->audio->mixChannel(output,0,nChannels);//,1.0f/players.size()*leftScale);
		piter->audio->mixChannel(output,1,nChannels);//,1.0f/players.size()*rightScale);
		//piter->video->mix(rAudio, bufferSize,1.0f/players.size()*rightScale);
		piter->audio->postProcess();
	}
	
	
	
//	memcpy(output, buffer, bufferSize*sizeof(float)*nChannels);
		
	
//	for (int i = 0; i < bufferSize; i++){
//		output[i*nChannels] = lAudio[i];// * gain;
//		output[i*nChannels + 1] = rAudio[i];// * gain;
//	}
	
}


void testApp::renderAudio() {
	
	setSongState(SONG_RENDER_AUDIO);
	
	cout << "renderAudio started" << endl;
	
	song.openForSave(ofToDocumentsPath("santa.caf"));
	
	currentBlock = 0;
	
	while (getSongState()==SONG_RENDER_AUDIO || getSongState()==SONG_CANCEL_RENDER_AUDIO) {
		
		//update(); // todo move to production for other thread
		audioRequested(sample.buffer,sample.numFrames,sample.nChannels);
		
//		song.saveWithBlocks(lAudio, rAudio);
		song.save(sample.buffer, sample.nChannels);
		currentBlock++;
	}
	
	song.close();	
	
	cout << "renderAudio finished" << endl;
	
	setSongState(SONG_RENDER_AUDIO_FINISHED);
	
	totalBlocks = currentBlock;
	
}

void testApp::seekFrame(int frame) {
	
		
	int reqBlock = (float)frame/25.0f*sampleRate/(float)sampler.getBufferSize();
	
//	for (vector<player>::iterator piter=players.begin(); piter!=players.end(); piter++) {
//		piter->bDidStartPlaying = true;
//	}
	
	for (;currentBlock<reqBlock;currentBlock++) { // TODO: or song finished...
		for (vector<player>::iterator piter=players.begin(); piter!=players.end(); piter++) {
			vector<event> events;
			piter->song.process(events);
			
			for (vector<event>::iterator niter=events.begin(); niter!=events.end(); niter++) {
				if (niter->bNoteOn) {
//					printf("player: %i, ", distance(players.begin(),piter));
					float speed = exp((float)(niter->note-60)/12.0*log(2.0));
					piter->video->play(speed); // ,niter->velocity
//					piter->bDidStartPlaying = true;
					
				}
			}
			
		}
	}

	for (vector<player>::iterator piter=players.begin(); piter!=players.end(); piter++) {
		piter->video->updateFrame();
	}
	
//	for (vector<player>::iterator piter=players.begin(); piter!=players.end(); piter++) {
//		if (piter->bDidStartPlaying) {
//			piter->bDidStartPlaying = false;
//		} else {
//			piter->video->nextFrame();
//		}
//
//	}
	
}




float testApp::getRenderProgress(){
	
	switch (songState) {
		case SONG_RENDER_AUDIO: {
			return (float)currentBlock * (float)sampler.getBufferSize() / (float)sampleRate/duration;
		}	break;
		case SONG_RENDER_VIDEO:
			return (float)currentBlock/(float)totalBlocks;
			break;
		default:
			return 0.0f;
	}
	
}

bool testApp::cameraToggle() {
	return grabber.cameraToggle();
}
//--------------------------------------------------------------
void testApp::lostFocus() {
}

//--------------------------------------------------------------
void testApp::gotFocus() {
}

//--------------------------------------------------------------
void testApp::gotMemoryWarning() {
}

//--------------------------------------------------------------
void testApp::deviceOrientationChanged(int newOrientation){
}



