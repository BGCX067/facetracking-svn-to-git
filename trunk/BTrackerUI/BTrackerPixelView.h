//
//  BTrackerPixelView.h
//  BTracker
//
//  Created by André Cohen on 9/17/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface BTrackerPixelView : NSView {
	NSBitmapImageRep *rep;
	NSImage *image;
	NSSize size;
}


- (void)setImageSize:(NSSize)size;
- (NSBitmapImageRep *)bitmap;
- (void)setPixel:(int)color atPoint:(NSPoint)point;

@end
