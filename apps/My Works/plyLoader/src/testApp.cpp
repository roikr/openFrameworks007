#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	importPlyCloud("cloud.bin", mesh);
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(255, 255, 255);
	
	
	easyCam.begin();
	
	
	glPointSize(3);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards' 
	ofScale(1, -1, -1);
	ofTranslate(0, 0, -1000); // center the points a bit
	glEnable(GL_DEPTH_TEST);
	mesh.drawVertices();
	glDisable(GL_DEPTH_TEST);
	ofPopMatrix();
	
	
	easyCam.end();
	
	
}

void testApp::exportPlyCloud(string filename, ofMesh& cloud) {
	ofFile ply;
	if (ply.open(filename, ofFile::WriteOnly, true)) {		
		// write the header
		ply << "ply" << endl;
		ply << "format binary_little_endian 1.0" << endl;
		ply << "element vertex " << cloud.getVertices().size() << endl;
		ply << "property float x" << endl;
		ply << "property float y" << endl;
		ply << "property float z" << endl;
		ply << "end_header" << endl;
		
		// write all the vertices
		vector<ofVec3f>& surface = cloud.getVertices();
		for(int i = 0; i < surface.size(); i++) {
			if (surface[i].z != 0) {
				// write the raw data as if it were a stream of bytes
				ply.write((char*) &surface[i], sizeof(ofVec3f));
			}
		}
	}
}

void testApp::importPlyCloud(string filename, ofMesh& cloud) {
	ofFile ply;
	if (ply.open(filename, ofFile::ReadOnly, true)) {		
		// write the header
		cloud.clear();
		cloud.setMode(OF_PRIMITIVE_POINTS);
	
		string line;
		int verticesNum;
		do {
			getline(ply, line);
			cout << line << endl;
		 
			vector<string> w = ofSplitString(line, " ");
			if (w[0]=="element") {
				if (w[1]=="vertex") {
					verticesNum = ofToInt(w[2]);
					cout << "ideed " << verticesNum << endl;
				}
			}
			
		} while (line!="end_header");
		
		cout << ply.tellp() << endl;
		
		ifstream::pos_type size = verticesNum*sizeof(ofVec3f);
		char * memblock = new char [size];
		ply.read (memblock, size);
		cloud.addVertices(reinterpret_cast<const ofVec3f *>(memblock), verticesNum);
		delete[] memblock;
		
		
		cout << cloud.getNumVertices() << endl;
		vector<ofVec3f>& surface = cloud.getVertices();
		for(int i = 0; i < surface.size(); i++) {
			cout << surface[i] << endl;
		}
		 
		
	}
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