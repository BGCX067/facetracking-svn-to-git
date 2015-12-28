//
//  BTrackerAppDelegate.m
//  BTracker
//
//  Created by André Cohen on 9/17/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "BTrackerAppDelegate.h"

@implementation BTrackerAppDelegate

@synthesize window;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	
	controller = [[BTrackerController alloc] init];
	[window setAlphaValue:1];
	[progress setUsesThreadedAnimation:YES];
	[progress setAlphaValue:0];
	[file setStringValue:[controller filePath]];
	
	play = NO;
	
	//[self exec];
	
}

- (void)applicationWillFinishLaunching:(NSNotification *)aNotification {
	[window setAlphaValue:.9];
}

- (void)exec {
	static clock_t avg = 0;
	clock_t start = clock();
	
	[progress startAnimation:nil];
	
	BOOL success = [controller update];
	
	if(avg == 0) avg = clock() - start;
	else avg = (avg + (clock() - start))/2.0;
	
	[fps setStringValue:[NSString stringWithFormat:@"Sec per frame: %f", (double)avg/CLOCKS_PER_SEC]];
	
	if(success) {
		[controller setClipView:clipView];
		[controller setMovieView:movieView];
		[flow setStringValue:[NSString stringWithFormat:@"Optical flow: %@", [controller flow]]];
		[[movieView superview] setNeedsDisplay:YES];
		[[clipView superview] setNeedsDisplay:YES];
	} else {
		play = NO;
		end = YES;
	}
	
	//[progress setAlphaValue:0];
	[progress stopAnimation:nil];
	
	if(play) {
		[self performSelector:@selector(exec) 
				   withObject:nil
				   afterDelay:0];
	}
	
}

-(void)applicationWillTerminate:(NSNotification *)notification {
	[controller release];
}

- (IBAction)playVideo:(id)info {
	if(end || [playButton isEnabled] == NO){
	}else if(play == NO){
		play = YES;
		[playButton setTitle:@"Stop"];
		[nextButton setEnabled:NO];
		[self exec];
	}else if(play == YES) {
		play = NO;
		[playButton setTitle:@"Play"];
		[nextButton setEnabled:YES];
	}
}

- (IBAction)stepVideo:(id)info {
	if([nextButton isEnabled])
		[self exec];
}

@end
