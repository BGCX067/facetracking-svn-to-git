//
//  BTrackerController.h
//  BTracker
//
//  Created by André Cohen on 9/17/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "BTrackerPixelView.h"

@interface BTrackerController : NSObject {
}

- (id)init;

- (void)setFilePath:(NSPathControl *)pathControl;
- (void)setClipView:(BTrackerPixelView *)view;
- (void)setMovieView:(BTrackerPixelView *)view;
- (NSString *)flow;

- (NSString *)filePath;

- (BOOL)update;

@end
