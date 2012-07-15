//
//  ofxiStillCamera.mm
//  stillExample
//
//  Created by Roee Kremer on 7/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxiStillCamera.h"

#include "StillCamera.h"
#include "ofxiPhoneExtras.h"

float angleOffsetFromPortraitOrientation(AVCaptureVideoOrientation orientation);

ofEvent<ofImage> ofxiStillCameraEvent;

void ofxiStillCamera::preview() {
    
    
    //-- Create CVOpenGLESTextureCacheRef for optimal CVImageBufferRef to GLES texture conversion.
    CVReturn err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, (__bridge void *)[ofxiPhoneGetGLView() context], NULL, &videoTextureCache);
    if (err) 
    {
        NSLog(@"Error at CVOpenGLESTextureCacheCreate %d", err);
    }
    
    videoTexture = NULL;
    width = 0;
    height = 0;
    
    stillCamera = [[StillCamera alloc] init]; 
    
    
    bPlaying = false;
    bSnap = false;
    
}

void ofxiStillCamera::snap() {
    if (bPlaying && ![stillCamera isCapturingStillImage]) {
//        image.clear();
        
//        NSString *filePath = [NSString stringWithFormat:@"%@%@", NSTemporaryDirectory(), @"output.jpg"];
//        if ([[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
//            NSError *error;
//            if ([[NSFileManager defaultManager] removeItemAtPath:filePath error:&error] == NO) {
//                NSLog(@"removeFile error: %@, reason: %@",[error localizedDescription],[error localizedFailureReason]);
//                
//            }
//        }
        [stillCamera takePicture];
    }
}

void ofxiStillCamera::stop() {
    if (bPlaying) {
        
        cleanUpTextures();
        
        width = 0;
        height = 0;
        
        if (videoTextureCache) {
            CFRelease(videoTextureCache);
            videoTextureCache = 0;
        }
        
        [stillCamera stop];
        [stillCamera release];
        stillCamera = NULL;
        
        bPlaying = false;
        
    }
}




void ofxiStillCamera::update() {
	
    if (stillCamera == NULL) {
        return;
    }
	
    
	if (!bPlaying) {
		if (stillCamera.state == STILL_CAMERA_STATE_PLAYING) {
			bPlaying = true;
        }
	} else {
        CMSampleBufferRef sbuf = (CMSampleBufferRef)CMBufferQueueDequeueAndRetain([stillCamera previewBufferQueue]);
        if (sbuf) {
            CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sbuf);
            
            
            width = CVPixelBufferGetWidth(pixelBuffer);
            height = CVPixelBufferGetHeight(pixelBuffer);
            
            cleanUpTextures();
            
            CVReturn err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault, 
                                                                        videoTextureCache,
                                                                        pixelBuffer,
                                                                        NULL,
                                                                        GL_TEXTURE_2D,
                                                                        GL_RGBA,
                                                                        width,
                                                                        height,
                                                                        GL_BGRA,
                                                                        GL_UNSIGNED_BYTE,
                                                                        0,
                                                                        &videoTexture);
            
            if (!videoTexture || err) {
                NSLog(@"CVOpenGLESTextureCacheCreateTextureFromImage failed (error: %d)", err);  
            } else {
                // Set texture parameters
                bind();
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
                unbind();
            }
            
            CFRelease(sbuf);
        }
        
        
        sbuf = (CMSampleBufferRef)CMBufferQueueDequeueAndRetain([stillCamera imagesBufferQueue]);
        if (sbuf) {
            
//            NSString *filePath = [NSString stringWithFormat:@"%@%@", NSTemporaryDirectory(), @"output.jpg"];
            
//            NSData *jpegData = [AVCaptureStillImageOutput jpegStillImageNSDataRepresentation:sbuf];
//            
//            NSError *error;
//            if ([[NSFileManager defaultManager] createFileAtPath:filePath contents:jpegData attributes:nil] == NO) {
//                NSLog(@"createFileAtPath error: %@, reason: %@",[error localizedDescription],[error localizedFailureReason]);
//                
//            }
            
//            image.loadImage(ofxNSStringToString(filePath));
            
            
            CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sbuf);
                        
            CVPixelBufferLockBaseAddress( pixelBuffer, 0 );
            
            int bufferWidth = CVPixelBufferGetWidth(pixelBuffer);
            int bufferHeight = CVPixelBufferGetHeight(pixelBuffer);
            unsigned char *pixel = (unsigned char *)CVPixelBufferGetBaseAddress(pixelBuffer);
            
            ofImage image;
            image.setUseTexture(false);
            image.setFromPixels(pixel, bufferWidth, bufferHeight, OF_IMAGE_COLOR_ALPHA);
            
            CVPixelBufferUnlockBaseAddress( pixelBuffer, 0 ); 
            CFRelease(sbuf);
            
            image.setImageType(OF_IMAGE_COLOR);
            image.getPixelsRef().swapRgb();
            image.setUseTexture(true);
            image.reloadTexture();
            
            
            int nRotations = (angleOffsetFromPortraitOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT)-angleOffsetFromPortraitOrientation([stillCamera avOrientationForDeviceOrientation:[[UIDevice currentDevice] orientation]])) / -90.0;
                                                                                                                                       
            cout << "snap: " << image.getWidth() << "\t" << image.getHeight() << "\t" << nRotations << endl;
            
            
            image.rotate90(nRotations);
                
//            image.saveImage(ofxNSStringToString(filePath));
            
            
            ofNotifyEvent(ofxiStillCameraEvent , image);
            
            
        }
        

    }    
}



void ofxiStillCamera::draw(ofRectangle rect,ofRectangle tex) {
	if (!videoTexture) {
        return;
	}
    
    
	glPushMatrix();
    
	
	GLfloat spriteTexcoords[] = {
		tex.x+tex.width,tex.y+tex.height,   
		tex.x+tex.width,tex.y,
		tex.x,tex.y+tex.height,   
		tex.x,tex.y,};
	
    //	float w =stillCamera.width*u;//ofGetWidth()/2;
    //	float h =stillCamera.height*v;//(float)ofGetWidth()/(float)width*(float)height/2;
	
	GLfloat spriteVertices[] =  {
		rect.x+rect.width,rect.y+rect.height,0,   
		rect.x+rect.width,rect.y,0,   
		rect.x,rect.y+rect.height,0, 
		rect.x,rect.y,0};
	
	
//    glTranslatef(rect.x+rect.width/2, rect.y+rect.height/2, 0);
//    glRotatef(angleOffsetFromPortraitOrientation(ofxiPhoneGetOrientation())-angleOffsetFromPortraitOrientation(stillCamera.videoOrientation), 0.0, 0.0, 1.0);
//    glTranslatef(-rect.x-rect.width/2, -rect.y-rect.height/2, 0);
    
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, spriteVertices);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, spriteTexcoords);	
    
    glEnable(GL_TEXTURE_2D);
    
    bind();
    
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    
    unbind();
    	
	glDisable(GL_TEXTURE_2D);
	
	glPopMatrix();
    
    glFlush();
}

void ofxiStillCamera::bind() {
    glBindTexture(CVOpenGLESTextureGetTarget(videoTexture), CVOpenGLESTextureGetName(videoTexture));
    
	
}

void ofxiStillCamera::unbind() {
    glBindTexture(CVOpenGLESTextureGetTarget(videoTexture), 0);
}

void ofxiStillCamera::cleanUpTextures()
{    
    if (videoTexture)
    {
        CFRelease(videoTexture);
        videoTexture = NULL;        
    }
    
    // Periodic texture cache flush every frame
    CVOpenGLESTextureCacheFlush(videoTextureCache, 0);
}



float ofxiStillCamera::getFrameRate() {
	return stillCamera ? stillCamera.videoFrameRate : 0;
}


float ofxiStillCamera::getWidth() {
	return width;
}

float ofxiStillCamera::getHeight() {
	return height;
}

bool ofxiStillCamera::getIsPlaying() {
	return bPlaying;
}

bool ofxiStillCamera::getIsFrameVisible() {
    return videoTexture;
}


float angleOffsetFromPortraitOrientation(AVCaptureVideoOrientation orientation)
{
	CGFloat angle = 0.0;
	
	switch (orientation) {
		case AVCaptureVideoOrientationPortrait:
			angle = 0.0;
			break;
		case AVCaptureVideoOrientationPortraitUpsideDown:
			angle = 180;
			break;
		case AVCaptureVideoOrientationLandscapeRight:
			angle = -90;
			break;
		case AVCaptureVideoOrientationLandscapeLeft:
			angle = 90;
			break;
		default:
			break;
	}
    
	return angle;
}
