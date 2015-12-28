//
//  BTrackerPixelView.m
//  BTracker
//
//  Created by André Cohen on 9/17/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "BTrackerPixelView.h"


@implementation BTrackerPixelView

- (id)init {
	[super init];
	rep = nil;
	return self;
}

- (void)setImageSize:(NSSize)aSize {
	if(rep)
		[rep release];

	self->size = aSize;
	rep = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:nil 
												  pixelsWide:size.width
												  pixelsHigh:size.height
											   bitsPerSample:8 
											 samplesPerPixel:4
													hasAlpha:YES 
													isPlanar:NO 
											  colorSpaceName:NSDeviceRGBColorSpace 
												 bytesPerRow:0
												bitsPerPixel:32];
	
}

- (NSBitmapImageRep *)bitmap {
	return rep;
}

- (void)drawRect:(NSRect)dirtyRect {
	[NSGraphicsContext saveGraphicsState];

	image = [[[NSImage alloc] initWithCGImage:[rep CGImage] size:size] autorelease];

	[image drawAtPoint:NSMakePoint(0, 0)
			  fromRect:NSMakeRect(0, 0, size.width, size.height)
			 operation:NSCompositeSourceOver 
			  fraction:1];
	
	[NSGraphicsContext restoreGraphicsState];

}

- (void)setPixel:(int)color atPoint:(NSPoint)point {
	int loc = point.y * (rep.bytesPerRow) + point.x * rep.bitsPerPixel/8;
	[rep bitmapData][loc + 0] = (0xff0000 & color) >> 16;
	[rep bitmapData][loc + 1] = (0x00ff00 & color) >> 8;
	[rep bitmapData][loc + 2] = (0x0000ff & color) >> 0;

}

@end
