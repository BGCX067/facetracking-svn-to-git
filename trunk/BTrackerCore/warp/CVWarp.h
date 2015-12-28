/*
 *  CVWarp.h
 *  BTracker
 *
 *  Created by André Cohen on 11/19/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _CV_WARP_H_
#define _CV_WARP_H_

#ifdef __LINUX__
#include <opencv/cv.h>
#else
#include <OpenCV/OpenCV.h>
#endif

#include "Warp.h"
#include "Global.h"

class CVWarp : public Warp {
public:
	static shared_ptr<Warp> getInstance();
	
	virtual MatrixXi exec(MatrixXi, MatrixXf);
	virtual Samples exec(MatrixXi, Samples);
	virtual void exec(MatrixXi, shared_ptr<Sample>);
private:
	IplImage *currentFrame;
};

#endif
