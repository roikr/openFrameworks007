//
//  ofxTexturesVector.h
//  MixMeXmas
//
//  Created by Roee Kremer on 10/31/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#pragma once


#include <string>
#include <vector>

using namespace std; 

class ofxRKTexture;

class ofxTexturesVector {
	
	public :
	
	void setup(string filename,int numTextures);
	void init();
    void release();
    
    void setTexture(int i);
	void draw();
    int getNumTextures();
		
protected:
	
	
	string filename;
    vector<ofxRKTexture *> textures;
    vector<ofxRKTexture *>::iterator titer;
    bool bInitialized;
	
};

