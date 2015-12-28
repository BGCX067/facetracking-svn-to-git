/*
 *  CVVideo.cpp
 *  BTracker
 *
 *  Created by André Cohen on 10/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "CVVideo.h"

CVVideo::CVVideo(string file) : Video(file) {
	video = cvCaptureFromAVI(videoFile.c_str());
	cvQueryFrame(video);
	currentFrame = 0;
	totalFrames = (int)cvGetCaptureProperty(video, CV_CAP_PROP_FRAME_COUNT);
}

CVVideo::~CVVideo() {
	cvReleaseCapture(&video);
}

MatrixXi CVVideo::getMatrix() {
	IplImage *image = 0;
	if(!cvGrabFrame(video) || currentFrame++==totalFrames) {
		return MatrixXi();
	}

	image = cvRetrieveFrame(video);
	MatrixXi data(image->height, image->width);
	
	for(int i=0; i<data.size(); i++) {
		int loc = i*3;
		unsigned char r = image->imageData[loc+3];
		unsigned char g = image->imageData[loc+2];
		unsigned char b = image->imageData[loc+1];
		data(i) = r<<16 | g<<8 | b;
	}
	
	return data;
	
}

int CVVideo::getWidth() {
	static int w = (int)cvGetCaptureProperty(video, CV_CAP_PROP_FRAME_WIDTH);
	return w;
}

int CVVideo::getHeight() {
	static int h = (int)cvGetCaptureProperty(video, CV_CAP_PROP_FRAME_HEIGHT);
	return h;
}
