/*
 *  ofxiTexture.mm
 *  iPhonePincherExample
 *
 *  Created by Roee Kremer on 1/2/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#include "ofxiTexture.h"

#define PVR_TEXTURE_FLAG_TYPE_MASK	0xff

enum
{
	kPVRTextureFlagTypePVRTC_2 = 24,
	kPVRTextureFlagTypePVRTC_4
};


typedef struct _PVRTexHeader
{
	uint32_t headerLength;
	uint32_t height;
	uint32_t width;
	uint32_t numMipmaps;
	uint32_t flags;
	uint32_t dataLength;
	uint32_t bpp;
	uint32_t bitmaskRed;
	uint32_t bitmaskGreen;
	uint32_t bitmaskBlue;
	uint32_t bitmaskAlpha;
	uint32_t pvrTag;
	uint32_t numSurfs;
} PVRTexHeader;

	
bool ofxiTexture::load(string filename,int type) {
	
	
	if (type==OFX_TEXTURE_TYPE_PNG) {
		
	
			
		NSString *texturePath = [NSString stringWithCString:filename.c_str() encoding:[NSString defaultCStringEncoding]];
		
		
		
		
		CGImageRef textureImage = [UIImage imageNamed:texturePath].CGImage;
		if (textureImage == nil) {
			NSLog(@"Failed to load texture image");
			return false;
		}
		
		NSInteger texWidth = CGImageGetWidth(textureImage);
		NSInteger texHeight = CGImageGetHeight(textureImage);
		
		GLubyte *textureData = (GLubyte *)malloc(texWidth * texHeight * 4);
		
		CGContextRef textureContext = CGBitmapContextCreate(textureData,
															texWidth, texHeight,
															8, texWidth * 4,
															CGImageGetColorSpace(textureImage),
															kCGImageAlphaPremultipliedLast);
		
		// Rotate the image
		CGContextTranslateCTM(textureContext, 0, texHeight);
		CGContextScaleCTM(textureContext, 1.0, -1.0);
		
		CGContextDrawImage(textureContext, CGRectMake(0.0, 0.0, (float)texWidth, (float)texHeight), textureImage);
		CGContextRelease(textureContext);
		
		if (_name != 0)
			glDeleteTextures(1, &_name);
		
		glGenTextures(1, &_name);
		
		glBindTexture(GL_TEXTURE_2D, _name);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
		
		free(textureData);
		
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		_width = texWidth;
		_height = texHeight;
		_hasAlpha = CGImageGetAlphaInfo(textureImage) != kCGImageAlphaNone;
		_internalFormat = GL_RGBA;
				
		return true;
	}	
			
		
		
			
	char gPVRTexIdentifier[5] = "PVR!";
	//NSString * path = [[NSString alloc] initWithCString:filename.c_str()];
	NSString *path = [NSString stringWithFormat:@"%s",filename.c_str()];
	NSData *data = [NSData dataWithContentsOfFile:path];
	
	NSMutableArray *_imageData = [NSMutableArray array];
	
	_name = 0;
	_width = _height = 0;
	_internalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
	_hasAlpha = false;
	
	PVRTexHeader *header = NULL;
	uint32_t flags, pvrTag;
	uint32_t dataLength = 0, dataOffset = 0, dataSize = 0;
	uint32_t blockSize = 0, widthBlocks = 0, heightBlocks = 0;
	uint32_t width = 0, height = 0, bpp = 4;
	uint8_t *bytes = NULL;
	uint32_t formatFlags;
	
	header = (PVRTexHeader *)[data bytes];
	
	pvrTag = CFSwapInt32LittleToHost(header->pvrTag);
	
	if (gPVRTexIdentifier[0] != ((pvrTag >>  0) & 0xff) ||
		gPVRTexIdentifier[1] != ((pvrTag >>  8) & 0xff) ||
		gPVRTexIdentifier[2] != ((pvrTag >> 16) & 0xff) ||
		gPVRTexIdentifier[3] != ((pvrTag >> 24) & 0xff))
	{
		return false;
	}
	
	flags = CFSwapInt32LittleToHost(header->flags);
	formatFlags = flags & PVR_TEXTURE_FLAG_TYPE_MASK;
	
	if (formatFlags == kPVRTextureFlagTypePVRTC_4 || formatFlags == kPVRTextureFlagTypePVRTC_2)
	{
		[_imageData removeAllObjects];
		
		if (formatFlags == kPVRTextureFlagTypePVRTC_4)
			_internalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
		else if (formatFlags == kPVRTextureFlagTypePVRTC_2)
			_internalFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
		
		_width = width = CFSwapInt32LittleToHost(header->width);
		_height = height = CFSwapInt32LittleToHost(header->height);
		
		if (CFSwapInt32LittleToHost(header->bitmaskAlpha))
			_hasAlpha = true;
		else
			_hasAlpha = false;
		
		dataLength = CFSwapInt32LittleToHost(header->dataLength);
		
		bytes = ((uint8_t *)[data bytes]) + sizeof(PVRTexHeader);
		
		// Calculate the data size for each texture level and respect the minimum number of blocks
		while (dataOffset < dataLength)
		{
			if (formatFlags == kPVRTextureFlagTypePVRTC_4)
			{
				blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
				widthBlocks = width / 4;
				heightBlocks = height / 4;
				bpp = 4;
			}
			else
			{
				blockSize = 8 * 4; // Pixel by pixel block size for 2bpp
				widthBlocks = width / 8;
				heightBlocks = height / 4;
				bpp = 2;
			}
			
			// Clamp to minimum number of blocks
			if (widthBlocks < 2)
				widthBlocks = 2;
			if (heightBlocks < 2)
				heightBlocks = 2;
			
			dataSize = widthBlocks * heightBlocks * ((blockSize  * bpp) / 8);
			
			[_imageData addObject:[NSData dataWithBytes:bytes+dataOffset length:dataSize]];
			
			dataOffset += dataSize;
			
			width = MAX(width >> 1, 1);
			height = MAX(height >> 1, 1);
		}
		
		
	} else {
		return false;
	}

	
	
	width = _width;
	height = _height;
	GLenum err;
	
	if ([_imageData count] > 0)
	{
		if (_name != 0)
			glDeleteTextures(1, &_name);
		
		glGenTextures(1, &_name);
		//NSLog(@"allocating texture: %i",_name);
		glBindTexture(GL_TEXTURE_2D, _name);
	}
	
	if ([_imageData count] > 1)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	for (int i=0; i < [_imageData count]; i++)
	{
		data = [_imageData objectAtIndex:i];
		glCompressedTexImage2D(GL_TEXTURE_2D, i, _internalFormat, width, height, 0, [data length], [data bytes]);
		
		err = glGetError();
		if (err != GL_NO_ERROR)
		{
			NSLog(@"Error uploading compressed texture level: %d. glError: 0x%04X", i, err);
			return false;
		}
		
		width = MAX(width >> 1, 1);
		height = MAX(height >> 1, 1);
	}
	
	[_imageData removeAllObjects];
	
			
		
	
	return true;
	
}

void ofxiTexture::draw() {
	
	
	
	glPushMatrix();
	
	
	
	float u = 1;
	float v = 1;
	
	GLfloat spriteTexcoords[] = {
		u,v,   
		u,0.0f,
		0,v,   
		0.0f,0,};
	
	float w = _width*u;//ofGetWidth()/2;
	float h = _height*v;//(float)ofGetWidth()/(float)width*(float)height/2;
	
	GLfloat spriteVertices[] =  {
		w,h,0,   
		w,0,0,   
		0,h,0, 
		0,0,0};
	
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, spriteVertices);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, spriteTexcoords);	
	glBindTexture(GL_TEXTURE_2D, _name);
	glEnable(GL_TEXTURE_2D);
	
	
	if (_hasAlpha) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		GLfloat fColor[4]={1.0f,1.0f,1.0f,0.0f};
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
		glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB,         GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB,         GL_CONSTANT);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA,    GL_REPLACE);
		glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_ALPHA,       GL_TEXTURE);
		glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, fColor);
	
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glDisable(GL_BLEND);
	} else {
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	
	glDisable(GL_TEXTURE_2D);
	
	
	glPopMatrix();
	
	
	
}
	
	
void ofxiTexture::release() {
	if (_name) {
		glDeleteTextures(1, &_name);
		_name = 0;
		_width = 0;
		_height = 0;
#ifdef LOG_OFXI_TEXTURE
		cout << "ofxiTexture::release: " <<  _name  << endl; 
#endif
	} else {
#ifdef LOG_OFXI_TEXTURE
		cout << "ofxiTexture::release: already empty" << endl; 
#endif
	}
	
	
}
