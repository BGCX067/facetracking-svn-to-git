/*
 *  CVVideo.h
 *  BTracker
 *
 *  Created by André Cohen on 10/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _CV_VIDEO_H_
#define _CV_VIDEO_H_

#include "Global.h"
#include "Video.h"

#ifdef __LINUX__
#include <opencv/cv.h>
#include <opencv/highgui.h>
#else
#include <OpenCV/OpenCV.h>
#endif

class CVVideo : public Video {
public:
	CVVideo(string file);
	~CVVideo();
	
	virtual MatrixXi getMatrix();
	
	virtual int getWidth();
	virtual int getHeight();
	
	int getCurrentFrame() { return currentFrame; }
private:
	CvCapture *video;
	int currentFrame;
	int totalFrames;
};

#endif
