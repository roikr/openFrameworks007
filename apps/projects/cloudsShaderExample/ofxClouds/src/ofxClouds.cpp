//
//  ofxClouds.cpp
//  shaderExample
//
//  Created by Roee Kremer on 3/28/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxClouds.h"

#define TEXTURE_SIZE 512
#define TEXTURE_SIZE_MINUS_1 511
#define TEXTURE_MEMORY_SIZE 0x40000
#define TEXTURE_MEMORY_SIZE_MINUS_1 0x3ffff
#define TEXTURE_LINE_SHIFT 9

void interpolate( unsigned char *buffer, int step)
{
    int             i, j;
    float           a, b, c, d, xf, yf;
    
    for( int y=0; y<TEXTURE_SIZE; y+= step )
        for( int x=0; x<TEXTURE_SIZE; x+= step )
        {
            a = buffer[(y<<TEXTURE_LINE_SHIFT)+x];
            b = buffer[(y<<TEXTURE_LINE_SHIFT)+((x+step)&TEXTURE_SIZE_MINUS_1)];
            c = buffer[(((y+step)&TEXTURE_SIZE_MINUS_1)<<TEXTURE_LINE_SHIFT)+x];
            d = buffer[(((y+step)&TEXTURE_SIZE_MINUS_1)<<TEXTURE_LINE_SHIFT)+((x+step)&TEXTURE_SIZE_MINUS_1)];
            
            for( j=0; j< step; j++ )
                for( i=0; i< step; i++ )
                {
                    xf = (float)i/(float)step;
                    yf = (float)j/(float)step;
                    
                    xf = xf*xf*(3.0f-2.0f*xf);
                    yf = yf*yf*(3.0f-2.0f*yf);
                    
                    buffer[((y+j)<<TEXTURE_LINE_SHIFT)+(x+i)] =  a + xf*(b-a) +
                    yf*(c-a) + xf*yf*(a-b-c+d);
                }
        }
}

void ofxClouds::setup() {
    
    // we need GL_TEXTURE_2D for our models coords.
    ofDisableArbTex();
    
    ofPixels pixels,comp;
        
    comp.allocate(TEXTURE_SIZE, TEXTURE_SIZE, OF_IMAGE_GRAYSCALE);
    pixels.allocate(TEXTURE_SIZE, TEXTURE_SIZE, OF_IMAGE_COLOR_ALPHA);
    
    for(int i=0; i<TEXTURE_MEMORY_SIZE*4; i++ ) {
        pixels[i] = 0;
    }
    
    for (int j=0; j<4; j++) {
        for(int i=0; i<TEXTURE_MEMORY_SIZE; i++ ) {
           comp[i] = rand()&255;
        }
        
        interpolate(comp.getPixels(), 1 << (j*2+1));
        
        for(int i=0; i<TEXTURE_MEMORY_SIZE; i++ ) {
            pixels[i*4+j] = comp[i];
        }
        
    }
    
    
    
    fbm.allocate(pixels.getWidth(), pixels.getHeight(), ofGetGlInternalFormat(pixels));
    fbm.loadData(pixels);
    
    glBindTexture(GL_TEXTURE_2D, fbm.getTextureData().textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    ofEnableAlphaBlending();
    
	shader.load("shaders/clouds.vert", "shaders/clouds.frag");
	

    shader.begin();
    
    shader.setUniform2f("resolution", fbm.getWidth(),fbm.getHeight());
    shader.setUniformTexture("tex0", fbm, 0); 
    
    shader.setUniform2f("d0", 0.0f,2.5f);
    shader.setUniform2f("d1", 3.0f,0.0f);
    shader.setUniform2f("d2", -3.5f,0.0f);
    shader.setUniform2f("d3", 0.0f,4.0f);
    
    float a=0.2;
    float b=0.8;
    
    shader.setUniform1f("c", -a/(b-a));
    shader.setUniform1f("d", (1-a)/(b-a));
    
    shader.setUniform3f("color0", 0.7f,0.7f,0.7f);
    shader.setUniform3f("color1", 1.0f,1.0f,1.0f);
    
    shader.end();
    
    
	
    
}

void ofxClouds::update() {
/*
    float t = ofGetElapsedTimef()*velocity;
    
    layers[0].d = (unsigned int)(  t*2.5f ); layers[0].d <<= TEXTURE_LINE_SHIFT;
    layers[1].d = (unsigned int)(  t*3.0f );
    layers[2].d = (unsigned int)( -t*3.5f );
    layers[3].d = (unsigned int)(  t*4.0f ); layers[3].d <<= TEXTURE_LINE_SHIFT;
    layers[4].d = (unsigned int)(  t*4.5f );
    layers[5].d = (unsigned int)( -t*5.0f );
    layers[6].d = (unsigned int)(  t*5.5f ); layers[6].d <<= TEXTURE_LINE_SHIFT;
    layers[7].d = (unsigned int)( -t*6.0f );
    
    // composition (synthesis)
    for( int i=0; i<TEXTURE_MEMORY_SIZE; i++ )
    {
        unsigned int c = 0;
        for(int  k=0; k<8; k++ )
        {
            unsigned int o = i + layers[k].d;
            o &=  TEXTURE_MEMORY_SIZE_MINUS_1;
            c += (layers[k].texture.getPixels()[o] << k);
        }
        c+=128;
        fbm.getPixels()[i*4] = c>>8;
        fbm.getPixels()[i*4+1] = c>>8;
        fbm.getPixels()[i*4+2] = c>>8;
        fbm.getPixels()[i*4+3] = ofMap(c>>8,0,255,0,maxAlpha*255);
    }
    fbm.update();
*/
}

void ofxClouds::draw() {
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbm.getTextureData().textureID);
   
    
    shader.begin();

    shader.setUniform1f("time", ofGetElapsedTimef());
    
    //fbm.draw(0, 0);
    
    ofRect(0, 0, fbm.getWidth(), fbm.getHeight());
	
	
    shader.end();
	
    
}

