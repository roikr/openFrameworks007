#include "testApp.h"
#include "stdio.h"

void ProcessCallback(void *cdata) {
	
	testApp *app = reinterpret_cast<testApp*> (cdata);
	
	
	if (app->infile1->Eof()) {
		app->thread.DeleteObj(app->infile1, SNDIO_IN);
		app->thread.DeleteObj(app->input1);
		sleep(1);
		app->mix.DeleteObj(app->input1);
	}
	
		
	
}

void testApp::setup(){	 

	ofBackground(255,0,0);
	
	string inname1= ofToDataPath("NOTE_35.aif");
	string inname2 = ofToDataPath("NOTE_36.aif");
	
	infile1 = new SndAiff(const_cast<char*>(inname1.c_str()), READ);
	
	infile2 = new SndAiff(const_cast<char*>(inname2.c_str()), READ);
	
	input1 = new SndIn(infile1, 1);
	
	input2 = new SndIn(infile2, 1);

	output.SetOutput(2, &mix);
	
	mix.AddObj(input1); 
	mix.AddObj(input1); 
	
	thread.AddObj(infile1,SNDIO_IN);
	thread.AddObj(input1);
	thread.AddObj(infile2,SNDIO_IN);
	thread.AddObj(input2);
	thread.AddObj(&mix);
	thread.AddObj(&output,SNDIO_OUT);
	//thread.SetProcessCallback(ProcessCallback, this);
	
	
	
	thread.ProcOn();
	
	
}

void testApp::update()
{
	ofBackground(255, 255, 255);
}

void testApp::draw()
{

}

void testApp::exit() {
	thread.ProcOff();
	sleep(1);
	delete infile1;
	delete infile2;
	delete input1;
	delete input2;
}

void testApp::keyPressed(int key)
{
	//int n= DEF_SR/DEF_VECSIZE; 
	
	
	
	
	
	infile1->SetPos(0.0f);
	   
	
}

void testApp::keyReleased(int key){ 

}

void testApp::mouseMoved(int x, int y ){

}

void testApp::mouseDragged(int x, int y, int button){
		
}

void testApp::mousePressed(int x, int y, int button){
	
}

void testApp::mouseReleased(){

}
