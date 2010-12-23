//
//  VideoImporter.h
//  VideoToTextures
//
//  Created by Roee Kremer on 12/22/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#import <CoreMedia/CoreMedia.h>

class ofxiPhoneVideo;


@interface VideoImporter : NSObject {
	CMVideoDimensions videoDimensions;
	CMVideoCodecType videoType;
}

@property (readwrite) CMVideoDimensions videoDimensions;
@property (readwrite) CMVideoCodecType videoType;


+(id)videoImporter;

- (void)import:(NSURL*)videoURL withVideo:(ofxiPhoneVideo *)video; //withContext:(EAGLContext *)contextA withCompletionHandler:(void (^)(void))completionHandler;
-(void)renderCameraToSprite:(uint)text;

@end
