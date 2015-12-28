//
//  BTrackerController.m
//  BTracker
//
//  Created by André Cohen on 9/17/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "BTrackerController.h"
#import "BTrackerPixelView.h"

#include "Controller.h"
#include "ControllerB.h"
#include "Exception.h"

@implementation BTrackerController

static Controller *controller = nil;

void applyMatrixToView(BTrackerPixelView *view, MatrixXi mat);

- (id)init {
	[super init];

	try {
		int tracker = Config::getInstance()->getInt("Tracker");
	
		if(tracker == 0) 
			controller = new Controller();
		else 
			controller = new ControllerB();
	}catch(Exception &e) {
		NSLog(@"%s", e.what());
	}

	return self;
}

- (void)dealloc {
	delete controller;
	[super dealloc];
}

- (BOOL)update {
	
	BOOL success = controller->update();
	
	if(success == YES) {	
		//[self setView:view data:controller->getFrame().data()];
		//MatrixXi frame = controller->getFrame();
		//applyMatrixToView(view, controller->getFrame());
	}
	return success;
}

- (void)setClipView:(BTrackerPixelView *)view {
	applyMatrixToView(view, controller->getClip());
}

- (void)setMovieView:(BTrackerPixelView *)view {
	applyMatrixToView(view, controller->getFrame());	
}

- (void)setFilePath:(NSPathControl *)pathControl {
	[pathControl setStringValue:[NSString stringWithCString:Config::getInstance()->getString("Video").c_str()]];
}

- (NSString *)filePath {
	return [NSString stringWithCString:Config::getInstance()->getString("Video").c_str()];
}

- (NSString *)flow {
	return [NSString stringWithFormat:@"%f %f", controller->getFlow()(0), controller->getFlow()(1)]; 
}

void applyMatrixToView(BTrackerPixelView *view, MatrixXi mat) {
	[view setImageSize:NSMakeSize(mat.cols(), mat.rows())];
	
	NSBitmapImageRep *bm = [view bitmap];
	unsigned char *data = [bm bitmapData];
	
	for(int i=0; i<mat.size(); i++) {
		unsigned int loc = i*4;
		unsigned int color = mat(i);
		data[loc + 0] = (char)((0xff0000 & color) >> 16);
		data[loc + 1] = (char)((0x00ff00 & color) >> 8);
		data[loc + 2] = (char)((0x0000ff & color) >> 0);
		data[loc + 3] = 0xff;
	}
	
	//[[view superview] setNeedsDisplay:YES];
	
}

@end
