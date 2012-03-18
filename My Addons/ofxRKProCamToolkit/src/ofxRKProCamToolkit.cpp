#include "ofxRKProCamToolkit.h"


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


void findHomography(ofPoint src[4], ofPoint dst[4], float homography[16]){
	// arturo castro - 08/01/2010
	//
	// create the equation system to be solved
	//
	// from: Multiple View Geometry in Computer Vision 2ed
	//       Hartley R. and Zisserman A.
	//
	// x' = xH
	// where H is the homography: a 3 by 3 matrix
	// that transformed to inhomogeneous coordinates for each point
	// gives the following equations for each point:
	//
	// x' * (h31*x + h32*y + h33) = h11*x + h12*y + h13
	// y' * (h31*x + h32*y + h33) = h21*x + h22*y + h23
	//
	// as the homography is scale independent we can let h33 be 1 (indeed any of the terms)
	// so for 4 points we have 8 equations for 8 terms to solve: h11 - h32
	// after ordering the terms it gives the following matrix
	// that can be solved with gaussian elimination:
	
	float P[8][9]={
		{-src[0].x, -src[0].y, -1,   0,   0,  0, src[0].x*dst[0].x, src[0].y*dst[0].x, -dst[0].x }, // h11
		{  0,   0,  0, -src[0].x, -src[0].y, -1, src[0].x*dst[0].y, src[0].y*dst[0].y, -dst[0].y }, // h12
		
		{-src[1].x, -src[1].y, -1,   0,   0,  0, src[1].x*dst[1].x, src[1].y*dst[1].x, -dst[1].x }, // h13
		{  0,   0,  0, -src[1].x, -src[1].y, -1, src[1].x*dst[1].y, src[1].y*dst[1].y, -dst[1].y }, // h21
		
		{-src[2].x, -src[2].y, -1,   0,   0,  0, src[2].x*dst[2].x, src[2].y*dst[2].x, -dst[2].x }, // h22
		{  0,   0,  0, -src[2].x, -src[2].y, -1, src[2].x*dst[2].y, src[2].y*dst[2].y, -dst[2].y }, // h23
		
		{-src[3].x, -src[3].y, -1,   0,   0,  0, src[3].x*dst[3].x, src[3].y*dst[3].x, -dst[3].x }, // h31
		{  0,   0,  0, -src[3].x, -src[3].y, -1, src[3].x*dst[3].y, src[3].y*dst[3].y, -dst[3].y }, // h32
	};
	
	gaussian_elimination(&P[0][0],9);
	
	// gaussian elimination gives the results of the equation system
	// in the last column of the original matrix.
	// opengl needs the transposed 4x4 matrix:
	float aux_H[]={ P[0][8],P[3][8],0,P[6][8], // h11  h21 0 h31
		P[1][8],P[4][8],0,P[7][8], // h12  h22 0 h32
		0 ,      0,0,0,       // 0    0   0 0
		P[2][8],P[5][8],0,1};      // h13  h23 0 h33
	
	for(int i=0;i<16;i++) homography[i] = aux_H[i];
}

void gaussian_elimination(float *input, int n){
	// arturo castro - 08/01/2010
	//
	// ported to c from pseudocode in
	// http://en.wikipedia.org/wiki/Gaussian_elimination
	
	float * A = input;
	int i = 0;
	int j = 0;
	int m = n-1;
	while (i < m && j < n){
		// Find pivot in column j, starting in row i:
		int maxi = i;
		for(int k = i+1; k<m; k++){
			if(fabs(A[k*n+j]) > fabs(A[maxi*n+j])){
				maxi = k;
			}
		}
		if (A[maxi*n+j] != 0){
			//swap rows i and maxi, but do not change the value of i
			if(i!=maxi)
				for(int k=0;k<n;k++){
					float aux = A[i*n+k];
					A[i*n+k]=A[maxi*n+k];
					A[maxi*n+k]=aux;
				}
			//Now A[i,j] will contain the old value of A[maxi,j].
			//divide each entry in row i by A[i,j]
			float A_ij=A[i*n+j];
			for(int k=0;k<n;k++){
				A[i*n+k]/=A_ij;
			}
			//Now A[i,j] will have the value 1.
			for(int u = i+1; u< m; u++){
				//subtract A[u,j] * row i from row u
				float A_uj = A[u*n+j];
				for(int k=0;k<n;k++){
					A[u*n+k]-=A_uj*A[i*n+k];
				}
				//Now A[u,j] will be 0, since A[u,j] - A[i,j] * A[u,j] = A[u,j] - 1 * A[u,j] = 0.
			}
			
			i++;
		}
		j++;
	}
	
	//back substitution
	for(int i=m-2;i>=0;i--){
		for(int j=i+1;j<n-1;j++){
			A[i*n+m]-=A[i*n+j]*A[j*n+m];
			//A[i*n+j]=0;
		}
	}
}


vector<ofVec3f> findBoundingBox(vector<ofVec3f> &input) {
    
    ofVec3f base = input[1]-input[0];
    ofVec3f perp = ofVec3f(0.0,0.0,0.0);
        
    float distance=0;
    for (int i=2; i<input.size(); i++) {
        float proj = base.dot(input[i]-input[0])/base.length();
        ofVec3f vec = input[i]-(input[0]+base.getNormalized()*proj);
       
        if ( vec.length() > perp.length()) {
            perp = vec;
        }
    }
    
   // roikr: to find if clockwise or anti clockwise and determine if base is up or down
    
    vector<ofVec3f> output;
    output.push_back(input[0]);
    output.push_back(input[1]);
    output.push_back(input[1]+perp);
    output.push_back(input[0]+perp);
   
    
   
    
    return output;
    
}

vector<ofVec3f> findScreenCorners(vector<ofVec3f> &input,int width,int height,float scale) {
    ofVec3f &origin = input[3];
    
    ofVec3f xVec = (input[2]-input[3]).getNormalized()*width/scale;
    ofVec3f yVec = (input[0]-input[3]).getNormalized()*height/scale;
    
    vector<ofVec3f> output;
    output.push_back(origin);
    output.push_back(origin+xVec);
    output.push_back(origin+xVec+yVec);
    output.push_back(origin+yVec);
    
    return output;
}

void screenCoordinateSystem(vector<ofVec3f> &input,ofVec3f &origin,ofVec3f &xVec,ofVec3f &yVec) {
    origin = input[3];
    
    xVec = (input[2]-input[3]).getNormalized();
    yVec = (input[0]-input[3]).getNormalized();
}

void findCoordinateSystem(vector<ofVec3f> &input,ofVec3f &origin,ofVec3f &xVec,ofVec3f &yVec) {
    origin = input[0];
    xVec = (input[1]-input[0]).getNormalized();
    float proj = xVec.dot(input[3]-input[0]);
    yVec = (input[3]-(input[0]+xVec*proj)).getNormalized();
}

void ofSaveMatrices(string filename) {
    ofMatrix4x4 projMat,mvMat;
    glGetFloatv(GL_PROJECTION_MATRIX,projMat.getPtr());
	glGetFloatv(GL_MODELVIEW_MATRIX,mvMat.getPtr());
    
    
    ofstream myfile(filename.c_str());
    myfile <<projMat;
    myfile <<endl;
    myfile <<mvMat;
    myfile.close();
    
    cout <<projMat;
    cout << endl;
    cout <<mvMat;
}

void ofLoadMatrices(string filename,ofMatrix4x4 &projMat,ofMatrix4x4 &mvMat) {
    
    ifstream myfile(filename.c_str());
    if (myfile.is_open() && myfile.good())
    {
        myfile >> projMat;
        myfile >> mvMat;
        
        cout << projMat;
        cout << endl;
        cout << mvMat;
        
        
        
    } else {
        cout << "Unable to open file"; 
    }
    
    myfile.close();
}

void ofApplyMatrices(ofMatrix4x4 &projMat,ofMatrix4x4 &mvMat) {
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projMat.getPtr());
    
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(mvMat.getPtr());
}