#include "testApp.h"
#include "stdio.h"


void testApp::setup(){	 

	ofBackground(255,0,0);
	
	//string inname1= ofToDataPath("DRM_ELECTRO_8.wav");
//	string inname2 = ofToDataPath("VOC_BB_6.wav");
	string outname = ofToDataPath("test.wav");
	
	string inname1= ofToDataPath("NOTE_35.aif");
	string inname2 = ofToDataPath("NOTE_36.aif");
	
	//SndWave infile1((char*)inname1.c_str(), READ);
	//SndWave infile2((char*)inname2.c_str(), READ);
	SndAiff infile1((char*)inname1.c_str(), READ);
	SndAiff infile2((char*)inname2.c_str(), READ);
	SndWave outfile((char*)outname.c_str(), OVERWRITE);
	
	SndCoreAudio output;
	
	SndIn	input1(&infile1, 1); 
	SndIn	input2(&infile2, 1); 
	
	Mixer mix;
	mix.AddObj(&input1);
	mix.AddObj(&input2);
	
	//outfile.SetOutput(1, &mix);
	output.SetOutput(2, &mix);
	
	int i = 0;
	int n= DEF_SR/DEF_VECSIZE;
	bool bPlay1 = false;
	bool bPlay2 = false;
	
	
	
	while(i < n * 5 ){
		if (i % (n/3)  == 0 ) {
			bPlay1=true;
			infile1.SetPos(0.0f);
			//mix.AddObj(&input2);
		}
		
		if (i % (n/2)  == 0 ) {
			bPlay2=true;
			infile2.SetPos(0.0f);
		}
		
		
		if (bPlay1) {
			if (infile1.Eof()) {
				bPlay1 = false;
			}
			else {
				infile1.Read();
			}
		}	
		
		if (bPlay2) {
			if (infile2.Eof()) {
				bPlay2 = false;
			}
			else {
				infile2.Read();
			}
		}	
		input1.DoProcess();
		input2.DoProcess();
		mix.DoProcess();
		//outfile.Write();
		output.Write();
		i++;
	}	
	/*
	while(!infile1.Eof() && !infile2.Eof() ){
		infile1.Read(); 
		infile2.Read(); 
		input1.DoProcess();
		input2.DoProcess();
		mix.DoProcess();
		outfile.Write();
	}	
	 */
	
}

void testApp::update()
{
	ofBackground(255, 255, 255);
}

void testApp::draw()
{

}

void testApp::keyPressed(int key)
{
	
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
