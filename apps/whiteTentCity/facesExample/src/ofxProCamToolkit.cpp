#include "ofxProCamToolkit.h"



void drawLabeledAxes(float size) {
	ofPushMatrix();
	ofPushStyle();
	ofScale(size, size, size);
	ofSetLineWidth(3);
	ofSetColor(ofColor::red);
	ofLine(0, 0, 0, 1, 0, 0);
	ofDrawBitmapString("+x", 1, 0, 0);
	ofSetColor(ofColor::green);
	ofLine(0, 0, 0, 0, 1, 0);
	ofDrawBitmapString("+y", 0, 1, 0);
	ofSetColor(ofColor::blue);
	ofLine(0, 0, 0, 0, 0, 1);
	ofDrawBitmapString("+z", 0, 0, 1);
	ofPopStyle();
	ofPopMatrix();
}



GLdouble modelviewMatrix[16], projectionMatrix[16];
GLint viewport[4];
void updateProjectionState() {
	glGetDoublev(GL_MODELVIEW_MATRIX, modelviewMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);
}

ofVec3f ofWorldToScreen(ofVec3f world) {
	updateProjectionState();
	GLdouble x, y, z;
	gluProject(world.x, world.y, world.z, modelviewMatrix, projectionMatrix, viewport, &x, &y, &z);
	ofVec3f screen(x, y, z);
	screen.y = ofGetHeight() - screen.y;
	return screen;
}

ofVec3f ofScreenToWorld(ofVec3f screen) {
	updateProjectionState();
	GLdouble x, y, z;
	screen.y = ofGetHeight() - screen.y;
	gluUnProject(screen.x, screen.y, screen.z, modelviewMatrix, projectionMatrix, viewport, &x, &y, &z);
	ofVec3f world(x, y, z);
	return world;
}

ofMesh getProjectedMesh(const ofMesh& mesh) {
	ofMesh projected;
	for(int i = 0; i < mesh.getNumVertices(); i++) {
		ofVec3f cur = ofWorldToScreen(mesh.getVerticesPointer()[i]);
		cur.z = 0;
		projected.addVertex(cur);
	}
	return projected;
}


ofVec3f getClosestPointOnMesh(const ofMesh& mesh, float x, float y, int* choice, float* distance) {
	float bestDistance = numeric_limits<float>::infinity();
	int bestChoice = 0;
	for(int i = 0; i < mesh.getNumVertices(); i++) {
		const ofVec3f& cur = mesh.getVerticesPointer()[i];
		float dx = x - cur.x;
		float dy = y - cur.y;
		float curDistance = dx * dx + dy * dy;
		if(curDistance < bestDistance) {
			bestDistance = curDistance;
			bestChoice = i;
		}
	}
	if(choice != NULL) {
		*choice = bestChoice;
	}
	if(distance != NULL) {
		*distance = sqrtf(bestDistance);
	}
	return mesh.getVerticesPointer()[bestChoice];
}

int exportPlyVertices(ostream& ply, ofMesh& cloud) {
	int total = 0;
	int i = 0;
	ofVec3f zero(0, 0, 0);
	vector<ofFloatColor>& colors = cloud.getColors();
	vector<ofVec3f>& surface = cloud.getVertices();
	for(int i = 0; i < surface.size(); i++) {
		if (surface[i] != zero) {
			ply.write(reinterpret_cast<char*>(&(surface[i].x)), sizeof(float));
			ply.write(reinterpret_cast<char*>(&(surface[i].y)), sizeof(float));
			ply.write(reinterpret_cast<char*>(&(surface[i].z)), sizeof(float));
			if(colors.size() > 0) {
				unsigned char color[3] = {colors[i].r * 255, colors[i].g * 255, colors[i].b * 255};
				ply.write((char*) color, sizeof(char) * 3);
			}
			total++;
		}
	}
	return total;
}

void exportPlyCloud(string filename, ofMesh& cloud) {
	ofstream ply;
	ply.open(ofToDataPath(filename).c_str(), ios::out | ios::binary);
	if (ply.is_open()) {
		// create all the vertices
		stringstream vertices(ios::in | ios::out | ios::binary);
		int total = exportPlyVertices(vertices, cloud);
		
		// write the header
		ply << "ply" << endl;
		ply << "format binary_little_endian 1.0" << endl;
		ply << "element vertex " << total << endl;
		ply << "property float x" << endl;
		ply << "property float y" << endl;
		ply << "property float z" << endl;
		if (cloud.getNumColors() > 0) {
			ply << "property uchar red" << endl;
			ply << "property uchar green" << endl;
			ply << "property uchar blue" << endl;
		}
		ply << "end_header" << endl;
		
		// write all the vertices
		ply << vertices.rdbuf();
	}
}



int getClosestTripletOnMesh(const ofMesh& objectMesh,const ofMesh& imageMesh, float x, float y, float* distance) {
    
    float bestDistance = numeric_limits<float>::infinity();
    int bestChoice = 0;
    
    for(int i = 0; i < objectMesh.getNumIndices()/3; i++) {
        
		ofVec3f cur = ofVec3f(0,0,0);
        for (int j=0;j<3;j++) {
            cur+=imageMesh.getVerticesPointer()[objectMesh.getIndexPointer()[3*i+j]];
        }
        
        cur/=3.0;
        
		float dx = x - cur.x;
		float dy = y - cur.y;
        float dz = 0 - cur.z;
		float curDistance = dx * dx + dy * dy + dz * dz;
		if(curDistance < bestDistance) {
			bestDistance = curDistance;
			bestChoice = i;
		}
	}
    
    return bestChoice;
}
