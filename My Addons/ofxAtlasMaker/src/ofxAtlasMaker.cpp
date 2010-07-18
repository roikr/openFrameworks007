/*
 *  ofxAtlasMaker.cpp
 *  lipsAtlasCreator
 *
 *  Created by Roee Kremer on 3/17/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxAtlasMaker.h"
#include "ofMain.h"
#include "ofxXmlSettings.h"

void ofxAtlasMaker::update(int width,int height) {
	this->width = width;
	this->height = height;
	
	dim.clear();
	int i = 10;
	
	int n = numFrames ;
	printf("%u frames\n",n);
	
	int minw = ceil(log(width)/log(2));
	int minh = ceil(log(height)/log(2));
	
	while (n>0 && i>max(minw,minh)) {
		int p1 = pow(2,i);
		int fPerTex1 = ((p1/width) * (p1/height));
		int j=0;
		while (n>fPerTex1) {
			j++;
			n-=fPerTex1;
		}
		if (j) {
			printf("%u textures of %u (%u frames)\n",j,p1,fPerTex1*j);
			dim.push_back(make_pair(p1, j));
		}
		
		if (n>0) {
			int p0 = pow(2,i-1);
			int fPerTex0 = ((p0/width) * (p0/height));
			
			if (n/fPerTex0 >= 4) {
				/*
				 printf("1 texture of %u (%u frames)\n",p1,n);
				 if (dim.back().first == p1)
				 dim.back().second++;
				 else 
				 dim.push_back(make_pair(p1,1));
				 
				 n = 0;
				 */
				break;
			} else 
				i--;
			
		}
	}
	
	if (n) {
		int p1 = pow(2,i);
		int fPerTex1 = ((p1/width) * (p1/height));
		int last = n/fPerTex1;
		if (n % fPerTex1)
			last++;
		
		printf("last %u textures of %u (%u frames)\n",last,p1,n);
		
		if (!dim.empty() && dim.back().first == p1)
			dim.back().second+=last;
		else 
			dim.push_back(make_pair(p1,last));
	}
	printf("\n");
	
}

void ofxAtlasMaker::setup(string name,float &width,float &height,int numFrames) {
	this->name = name;
	
	ofxXmlSettings xml;
	
	bool newParams = true;
	
	if (xml.loadFile("textures.xml")) {
		
		xml.pushTag("actor");
		
		for (int i=0; i<xml.getNumTags("sequence"); i++) {
			if (xml.getAttribute("sequence", "prefix","",  i) == name) {
				width = this->width = xml.getAttribute("sequence", "width", 0, i);
				height = this->height = xml.getAttribute("sequence", "height", 0 , i);
				this->numFrames = xml.getAttribute("sequence", "frames",0, i);
				newParams = false;
				break;
			}
		}
		
		xml.popTag();
	}
	
	if (newParams) {
		this->width = width;
		this->height = height;
		this->numFrames = numFrames;
	}
	
	
	update(this->width, this->height);
}


void ofxAtlasMaker::save() {
	

	ofxXmlSettings xml;
	
	if (!xml.loadFile("textures.xml")) {
		xml.addTag("actor");
	}
	
	xml.pushTag("actor");
	
	
	for (int i=0; i<xml.getNumTags("sequence"); i++) {
		if (xml.getAttribute("sequence", "prefix","",  i) == name) {
			xml.removeTag("sequence", i);
			break;
		}
	}
	
	int which = xml.addTag("sequence");
	xml.addAttribute("sequence", "prefix", name, which);
	xml.addAttribute("sequence", "width", width, which);
	xml.addAttribute("sequence", "height", height, which);
	xml.addAttribute("sequence", "frames",numFrames, which);
	xml.pushTag("sequence", which);
	for (iter = dim.begin();iter!=dim.end();iter++) {
		which = xml.addTag("textures");
		xml.addAttribute("textures", "size", iter->first, which);
		xml.addAttribute("textures", "number", iter->second, which);
	}
	xml.popTag();
	
	xml.saveFile("textures.xml");	
	
	
	
}



void ofxAtlasMaker::process(string path,ofxSequencePlayer &movie,vector<pair<int,int> > *vec ) {

	
	
	
	iter = dim.begin();
	
	
	int currentTexture = 0;
	int relativeTexture = 0;
	int startFrame = 0;
	int divs = movie.getTotalNumFrames() / numFrames;
	
	unsigned char currentImage[1024*1024*3];
	memset(currentImage, 0, 1024*1024*3);
	
	movie.setPaused(true);
	
	for (int currentFrame=0; currentFrame<numFrames; currentFrame++) {
		movie.setFrame(currentFrame * divs);
		
		
		printf("frame: %u, start %u, relative: %u, ",currentFrame,startFrame,currentFrame-startFrame);
		printf("texture: %u/%u, size: %u\n",relativeTexture,iter->second,iter->first);
		
		int columns = iter->first/width;
		int rows = iter->first/height;
		
		int relativeFrame = (currentFrame - startFrame) % (rows*columns);
		
		
		int size = iter->first;
		//int y = (int) ((float)size*(float)(relativeFrame % (rows*columns) / columns) / (float) rows);
		//int x =(int) ((float)size*(float)(relativeFrame % (rows*columns)  % columns)/(float)columns);
		
		int y = (relativeFrame % (rows*columns) / columns) * height ;
		int x =(relativeFrame % (rows*columns)  % columns) * width;
		
		printf("copy frame %u(%u), size:%u, columns: %u, rows: %u, x: %u, y:%u\n",relativeFrame,currentFrame,size,columns,rows,x,y);
		
		
		
		unsigned char * pixels = movie.getPixels();
		
		if (!vec) {
			for (int j=0; j<height;j++ ) 
				for (int i=0;i<width;i++)
					for (int k=0;k<3;k++)
						currentImage[((y+j)*size + x + i)*3+k] = pixels[(j*width +i)*3+k];
		} else {
			int divFrame = currentFrame / (movie.getTotalNumFrames() / vec->size());
			int tx = (*vec)[divFrame].first;
			int ty = (*vec)[divFrame].second;
			for (int j=0; j<height;j++ ) 
				for (int i=0;i<width;i++)
					for (int k=0;k<3;k++)
						currentImage[((y+j)*size + x + i)*3+k] = pixels[((ty+j)*movie.getWidth() + tx+i)*3+k];
		}

		
		if (currentFrame == numFrames-1 || (currentFrame - startFrame+1) == rows*columns ) {
			ofImage image;
			image.setFromPixels(currentImage,iter->first,iter->first,OF_IMAGE_COLOR,true);
			image.saveImage(path+'_'+ofToString(currentTexture)+".png");
			
			printf("save image of size: %u\n",iter->first);
			
			memset(currentImage, 0, 1024*1024*3);
			currentTexture++;
			
			if ((currentFrame - startFrame+1) == rows*columns ) {
				
				relativeTexture++;
				startFrame = currentFrame+1;
				
				if (relativeTexture >= iter->second  ) {
					iter++;
					relativeTexture = 0;				
				}
				
			}
		}
	}
	
	movie.setPaused(false);
}



	



string ofxAtlasMaker::getDebugStr() {
	string res;
	char str[250];
	for (int i=0; i<dim.size(); i++) {
		int col = dim.at(i).first/width;
		int row = dim.at(i).first/height;
		sprintf(str, "%u textures of %u^2 (%u frames)",dim.at(i).second,dim.at(i).first,col*row);
		res+= string(str) + "\n";
	}
	return res;
}