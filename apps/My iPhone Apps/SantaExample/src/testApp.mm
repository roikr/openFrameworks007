
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

#define MAX_NOTES_PER_BUFFER 10


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
	
	events.reserve(MAX_NOTES_PER_BUFFER);
	
	
	
	int bufferSize = 512;
	int nChannels = 2;
	
	video.fps = 25;
	video.numIntroFrames = 4;
	video.numFrames = 35;
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
		c.bDisableNoteOff = xml.getAttribute("card", "noteoff", 1, j);
		c.background->init();
		c.background->load();
		int bpm = xml.getAttribute("card", "bpm", 120, j);
		
		xml.pushTag("card", j);

		for (i=0; i<xml.getNumTags("actor");i++) {
			
			actor a;
			a.x = xml.getAttribute("actor", "x", 0, i) ;
			a.y = xml.getAttribute("actor", "y", 0, i) ;
			a.scale = xml.getAttribute("actor", "scale", 1.0f, i) ;
			a.degree = xml.getAttribute("actor", "degree", 0.0f, i) ;
			a.player = xml.getAttribute("actor", "player", 0, i) ;
//				minX = min(p.x,minX);
//				maxX = max(p.x,maxX);
			c.actors.push_back(a);
		}
		
		for (i=0; i<xml.getNumTags("player");i++) {
			
			player p;
			p.song = new ofxMidiTrack;
			p.song->setup(bufferSize, sampleRate,bpm);
			p.song->loadTrack(ofToDataPath(xml.getAttribute("player", "filename", "", i)));
			p.video = new ofxiVideoPlayer;
			p.video->setup(&video,true,0.5f);
			p.audio = new ofxAudioPlayer;
			p.audio->setup(sampler.getAudioSample(),bufferSize,2); // max instances of sample 
			
			c.players.push_back(p);
			
		}
	
		xml.popTag();
	
		cards.push_back(c);
		prefs.pages.push_back(ofPoint(0,ofGetHeight()*j));
		
	}
							  
	prefs.direction = SLIDER_VERTICAL;
	prefs.bCyclic = true;
	prefs.lastPageSize = ofPoint(0,ofGetHeight());
	slider.setup(1,prefs);	
	bSlide = false;
		
	
	xml.popTag();
	
	citer = cards.begin();
	oiter = cards.end();
	bCardChanged =false;
	
	
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
	grabber.setup(&video,FRONT_CAMERA,0.75);
	
	
	
	songState = SONG_IDLE;
	
	trigger.setAutoThresh(0.1,50);
	//trigger.setThresh(0.15);
	limiter.setup(10, 500, sampleRate, 0.3);
	
	bool bLoaded = magic.load(ofToDataPath("magic.caf"), bufferSize);
	assert(bLoaded);
	bPlaySong = false;
	
	ofSoundStreamSetup(nChannels, 1, this, sampleRate, bufferSize, 2);
	
	grabber.startCamera();
	
	
	
	songVersion = 0;
	bCameraOffset = false;
	
	live();
	
	bNeedDisplay = true;
	bTriggerRecord = false;
	
	//camera->startCapture(); // TODO: remove this - just for testing	
}




//--------------------------------------------------------------
void testApp::update()
{
	if (!bCameraOffset && grabber.getIsReady()) {
		ofPoint offset = ofPoint((grabber.getCameraWidth()-video.textureWidth)/2,(grabber.getCameraHeight()-video.textureHeight)/2);
		grabber.setOffset(offset);
		bCameraOffset = true;
	}		

	slider.update();
	
	if (bCardChanged && (ofGetElapsedTimeMillis() - delayStart)>SONG_SWITCH_DELAY) {
		bCardChanged = false;
		switch (getSongState()) {
			case SONG_IDLE: 
			case SONG_PLAY: {
				setSongState(SONG_PLAY);
			} break;
			default:
				break;
		}
	}
	
	switch (songState) {
		case SONG_IDLE: 
			for (vector<player>::iterator iter=citer->players.begin(); iter!=citer->players.end(); iter++) { 
				iter->video->update();
			}
			
			
			break;
		case SONG_PLAY:
			for (vector<player>::iterator iter=citer->players.begin(); iter!=citer->players.end(); iter++) { 
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
			
//		case SONG_RENDER_VIDEO:
//			
//			if  (currentBlock / totalBlocks >= 1.0) {
//				setSongState(SONG_IDLE);
//				//songState = SONG_IDLE; // TODO: check why not notifying players...
//				//bNeedDisplay = true;
//			}
//			
//			
//			break;
			
			
		default:
			break;
	}
	
	if (bTriggerRecord && !getIsPlaying()) { // getSongState() == SONG_IDLE
		bTriggerRecord = false;
		state = STATE_RECORD;
		bNeedDisplay = true;
		trigger.setTrigger();
		trigger.resetMeters();
		
		
		songVersion++;
		//camera->setTrigger(thresh);
		//camera->startRecording();
		//cout << "Start recording" << endl;
	}
	
	
}


//--------------------------------------------------------------

void testApp::render()
{
	if (getSongState() == SONG_IDLE) {
		grabber.render();
		
	}
}

void testApp::drawCard(vector<card>::iterator iter) {
	
	
	for (vector<actor>::iterator aiter=iter->actors.begin(); aiter!=iter->actors.end(); aiter++)  {
		vector<player>::iterator piter=iter->players.begin()+aiter->player;
		ofPushMatrix();
		//glLoadIdentity();
		
		
		ofTranslate(aiter->x, aiter->y, 0);
		ofRotate(aiter->degree);
		ofScale(aiter->scale,aiter->scale,1.0);
		
		//ofTranslate(i % 2 * ofGetWidth()/2, (int)(i / 2) * ofGetHeight() /2);
		//ofScale(0.5, 0.5, 1);
		
		if (grabber.getIsReady() && (grabber.getState()==CAMERA_CAPTURING || grabber.getState()==CAMERA_RECORDING || video.textures.empty())) {
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
//	ofTranslate(0, y); 
	iter->background->draw(0, 0);
	ofPopMatrix();
	
	ofDisableAlphaBlending();
}

void testApp::draw()
{
	
	
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
	
	ofPushMatrix();
	slider.transform();
	
	
	if (slider.getIsDragging() || slider.getIsAnimating()) {
		ofPushMatrix();
		ofTranslate(0, -ofGetHeight(), 0);
		drawCard(cards.end()-1);
		ofPopMatrix();
		
		for (vector<card>::iterator iter=cards.begin(); iter!=cards.end(); iter++) {
			ofPushMatrix();
			ofTranslate(0, distance(cards.begin(), iter) *ofGetHeight(), 0);
			drawCard(iter);
			ofPopMatrix();
		}
		
		ofPushMatrix();
		ofTranslate(0, ofGetHeight()*cards.size(), 0);
		drawCard(cards.begin());
		ofPopMatrix();
		
	} else {
		ofPushMatrix();
		ofTranslate(0, distance(cards.begin(), citer) *ofGetHeight(), 0);
		drawCard(citer);
		ofPopMatrix();
	}


		
	ofPopMatrix();
//	trigger.draw();

}

void testApp::renderVideo(){
	
	ofBackground(0, 0, 0);
	ofSetColor(255,255,255,255);
	ofPushMatrix();
	
	vector<card>::iterator iter = citer;
	

	for (vector<actor>::iterator aiter=iter->actors.begin(); aiter!=iter->actors.end(); aiter++)  {
		vector<player>::iterator piter=iter->players.begin()+aiter->player;	
		ofPushMatrix();
		ofTranslate(aiter->x, aiter->y, 0);
		ofRotate(aiter->degree);
		ofScale(aiter->scale,aiter->scale,1.0);
		piter->video->draw();
		ofPopMatrix();
	}
	
	ofEnableAlphaBlending();
	ofPushMatrix();
	ofTranslate(0, 0); 
	iter->background->draw(0, 0);
	ofPopMatrix();
	ofDisableAlphaBlending();
	ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::exit() {
	grabber.exit();
}

void testApp::suspend() {
	setSongState(SONG_IDLE);
	grabber.suspend();
}

void testApp::resume() {
	live(); // roikr: double live on startup ?
}


void testApp::more() {
	slider.next();
	testSlider();
	
}


void testApp::live() {
	state = STATE_LIVE;
	bNeedDisplay = true;
//	grabber.releaseVideo();
	grabber.startCapture();
}

void testApp::record() {
	
	bTriggerRecord = true;
}



void testApp::preview() {

/*
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
*/
}

//void testApp::playIntro() {
//	for (vector<player>::iterator iter=players.begin(); iter!=players.end(); iter++)  {		
//		
//		iter->video->playIntro();
//		
//	}
//	
//}

bool testApp::getIsPlaying() {
	for (vector<player>::iterator iter=citer->players.begin(); iter!=citer->players.end(); iter++)  {		
		if (iter->audio->getNumPlaying() || iter->song->getIsPlaying()) {
			return true;
		} 
	}
	return false;
}


int testApp::getState() {
	return state;
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
		
		for (vector<player>::iterator piter=citer->players.begin(); piter!=citer->players.end(); piter++)  {
			duration = max(duration, piter->song->getDuration());
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
			for (vector<player>::iterator iter=citer->players.begin(); iter!=citer->players.end(); iter++)  {
				iter->song->stop();
				iter->audio->stop();
				iter->video->playIntro();
				
			}
			break;
		case SONG_PLAY:
		case SONG_RENDER_AUDIO:
		case SONG_RENDER_VIDEO:
			if (!(grabber.getState()==CAMERA_CAPTURING || grabber.getState()==CAMERA_RECORDING || video.textures.empty())) {
				for (vector<player>::iterator iter=citer->players.begin(); iter!=citer->players.end(); iter++)  {
					iter->song->play();
				}
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
		
		/*
		switch ((int)(4*touch.y/(ofGetHeight()+1))) {
			case 0:
				
				record();
				
				
				break;
			case 1:
				setSongState(SONG_IDLE);
				//preview();
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
		 */
		
	} else if (touch.x < 30) {
		if (trigger.getIsAutoThreshEnabled()) {
			trigger.setAutoThresh((1-(touch.y/ofGetHeight()))/4, 50);
		} else {
			trigger.setThresh(1-(touch.y/ofGetHeight()));
		}
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
	/*
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
	 */
	
	if (bSlide) {
		bSlide = false;
		slider.touchUp(touch.x, touch.y,touch.id);
		testSlider();		
	}

}

void testApp::testSlider() {
	
	
	switch (getSongState()) {
		case SONG_IDLE: 
		case SONG_PLAY: {
			vector<card>::iterator iter = cards.begin()+slider.getCurrentPage();
			if (citer!=iter) {
				setSongState(SONG_IDLE);
				songVersion++;
				oiter = citer;
				citer = iter;
				bCardChanged = true;
				delayStart = ofGetElapsedTimeMillis();
			}
		} break;
		default:
			break;
	}

}


//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
	if (touch.x < 30) {
		if (!trigger.getIsAutoThreshEnabled()) {
			trigger.setAutoThresh((1-(touch.y/ofGetHeight()))/4, 50);
		} else {
			trigger.disableAutoTrhresh();
			trigger.setThresh(1-(touch.y/ofGetHeight()));
		}
		
	} 
}
	


void testApp::audioReceived( float * input, int bufferSize, int nChannels ) {
	
	trigger.audioReceived(input,bufferSize);
	
	
	
	if (trigger.getTriggerState() == TRIGGER_TRIGGERED) {
		trigger.resetTrigger();
		grabber.record();
		sampler.record();
		
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
			magic.play();
			bPlaySong = true;
			
		}
	}
}



//static float maxSpeed = 1.0f;
//static float minSpeed = 1.0f;
//static int maxEvents = 0;


void testApp::audioRequested( float * output, int bufferSize, int nChannels ) {
	
	memset(output, 0, bufferSize*sizeof(float)*nChannels);
	
	for (vector<player>::iterator piter=citer->players.begin(); piter!=citer->players.end() ; piter++)  {
		
		switch (songState) {
			case SONG_PLAY:
			case SONG_RENDER_AUDIO: {
				
				piter->song->process(events); // allocate events and reserve enough space to avoid reallocations here
				
//				if (events.size()>maxEvents) {
//					maxEvents = events.size();
//					printf("player: %i\tmaxEvents: %i\n",distance(citer->players.begin(),piter),maxEvents);
//				}
				
				for (vector<event>::iterator niter=events.begin(); niter!=events.end(); niter++) {
					if (niter->bNoteOn) {
//						printf("player: %i, ", distance(players.begin(),piter));
						float pan = 0.5f;
						
						sampleInstance si;
						si.speed = exp((float)(niter->note-60)/12.0*log(2.0));
//						float speed = si.speed;
//						
//						if (speed>maxSpeed || speed<minSpeed) {
//							maxSpeed= max(speed,maxSpeed);
//							minSpeed = min(speed,minSpeed);
//							printf("player: %i\tnote: %i\tmin: %f\t max: %f\n",distance(citer->players.begin(),piter),niter->note,minSpeed,maxSpeed);
//						}
						
						float volume = (float)niter->velocity / 127* 1.0f;// /players.size();
						
						si.retrigger = true;
						
						for (vector<actor>::iterator aiter=citer->actors.begin(); aiter!=citer->actors.end() ; aiter++)  {
							if (aiter->player  == distance(citer->players.begin(), piter)) {
								pan = (float)aiter->x/(float)(ofGetWidth());
								if (songState == SONG_PLAY) { 
									piter->video->play(si.speed);
								}
							}
							
						}
						
						//piter->video->play(niter->note,niter->velocity);
					
						si.left = volume * (1 - pan);
						si.right = volume * pan;
						piter->audio->trigger(si);
					} else if (citer->bDisableNoteOff) {
						piter->audio->stop();
					}
				}
				
				events.clear();
				
			} break;
				
			default:
				break;
		}
		
		piter->audio->mixChannel(output,0,nChannels);//,1.0f/players.size()*leftScale);
		piter->audio->mixChannel(output,1,nChannels);//,1.0f/players.size()*rightScale);
		//piter->video->mix(rAudio, bufferSize,1.0f/players.size()*rightScale);
		piter->audio->postProcess();
	}
	
	if (oiter!=cards.end()) {
		bool bPlaying = false;
		for (vector<player>::iterator piter=oiter->players.begin(); piter!=oiter->players.end() ; piter++)  {
			
			piter->audio->mixChannel(output,0,nChannels);
			piter->audio->mixChannel(output,1,nChannels);
			piter->audio->postProcess();
			bPlaying |= piter->audio->getNumPlaying() || piter->song->getIsPlaying();
		}		
		
		if (!bPlaying) {
			oiter=cards.end();
		}
	}
	
	
	if (bPlaySong) {
		if (magic.getNumPlaying()) {
			magic.mixChannel(output, 0, nChannels);
			magic.mixChannel(output, 1, nChannels);
			magic.postProcess();
		} else {
			bPlaySong = false;
			state = STATE_PLAY;
//			bNeedDisplay = true;
			setSongState(SONG_PLAY);
		}
	}

	
	limiter.audioProcess(output,bufferSize,nChannels);
	
	
//	memcpy(output, buffer, bufferSize*sizeof(float)*nChannels);
		
	
//	for (int i = 0; i < bufferSize; i++){
//		output[i*nChannels] = lAudio[i];// * gain;
//		output[i*nChannels + 1] = rAudio[i];// * gain;
//	}
	
}


void testApp::renderAudio() {
	
	setSongState(SONG_RENDER_AUDIO);
	
	cout << "renderAudio started" << endl;
	
	song.openForSave(ofToDocumentsPath("temp.caf"));
	
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
		for (vector<player>::iterator piter=citer->players.begin(); piter!=citer->players.end(); piter++) {
			vector<event> events;
			piter->song->process(events);
			
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

	for (vector<player>::iterator piter=citer->players.begin(); piter!=citer->players.end(); piter++) {
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



