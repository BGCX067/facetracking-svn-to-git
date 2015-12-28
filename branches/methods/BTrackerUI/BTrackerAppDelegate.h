//
//  BTrackerAppDelegate.h
//  BTracker
//
//  Created by André Cohen on 9/17/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "BTrackerPixelView.h"
#include "BTrackerController.h"

@interface BTrackerAppDelegate : NSObject <NSApplicationDelegate> {
	IBOutlet BTrackerPixelView *clipView;
	IBOutlet BTrackerPixelView *movieView;
	IBOutlet NSTextField *fps;
	IBOutlet NSTextField *flow;
	IBOutlet NSTextField *file;
	IBOutlet NSProgressIndicator *progress;
	
	IBOutlet NSButton *playButton;
	IBOutlet NSButton *nextButton;
		
    NSWindow *window;
	BTrackerController *controller;
	
	BOOL play;
	BOOL end;
}

@property (assign) IBOutlet NSWindow *window;

- (void)exec;

- (IBAction)playVideo:(id)info;
- (IBAction)stepVideo:(id)info;

@end
