//
//  EventRouteWindow.m
//  routeEvents
//
//  Created by Roee Kremer on 6/21/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "EventRouteWindow.h"

@interface EventRouteWindow() {
}
- (void) handleTouches:(UIEvent*)event;
@end

@implementation EventRouteWindow
//@synthesize overlayListener;
@synthesize view;

-(id)initWithFrame:(CGRect)frame
{
	if((self = [super initWithFrame:frame]))
	{
	}
	return self;
}

-(void)dealloc
{
	[super dealloc];
}

- (void)sendEvent:(UIEvent*)event {
	[super sendEvent:event];	
//	if(overlayListener != nil && [overlayListener respondsToSelector:@selector(handleTouches:)]) {
//		[overlayListener performSelector:@selector(handleTouches:) withObject:event];
//    }
    
    [self handleTouches:event];
    
}


- (void) handleTouches:(UIEvent*)event {   
    NSSet *touches = [event allTouches];
    NSMutableSet *began = nil;
    NSMutableSet *moved = nil;
    NSMutableSet *ended = nil;
    NSMutableSet *cancelled = nil;
    
    // sort the touches by phase so we can handle them similarly to normal event dispatch
    for(UITouch *touch in touches) {
        switch ([touch phase]) {
            case UITouchPhaseBegan:
                if (!began) began = [NSMutableSet set];
                [began addObject:touch];
                break;
            case UITouchPhaseMoved:
                if (!moved) moved = [NSMutableSet set];
                [moved addObject:touch];
                break;
            case UITouchPhaseEnded:
                if (!ended) ended = [NSMutableSet set];
                [ended addObject:touch];
                break;
            case UITouchPhaseCancelled:
                if (!cancelled) cancelled = [NSMutableSet set];
                [cancelled addObject:touch];
                break;
            default:
                break;
        }
    }
    // call our methods to handle the touches
    if (began)     [self.view touchesBegan:began withEvent:event];
    if (moved)     [self.view touchesMoved:moved withEvent:event];
    if (ended)     [self.view touchesEnded:ended withEvent:event];
    if (cancelled) [self.view touchesCancelled:cancelled withEvent:event];
    
}
@end