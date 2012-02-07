//
//  VideoStream.h
//  videoStream
//
//  Created by Roee Kremer on 2/3/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>

@interface VideoStream : NSObject<AVCaptureVideoDataOutputSampleBufferDelegate>

- (void)setupAVCapture;
- (void)tearDownAVCapture;

@end
