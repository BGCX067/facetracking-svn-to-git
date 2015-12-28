/*
 *  CVOpticalFlow.h
 *  BTracker
 *
 *  Created by André Cohen on 12/6/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _CV_OTPICAL_FLOW_H_
#define _CV_OPTICAL_FLOW_H_

#ifdef __LINUX__
#include <opencv/cv.h>
#else
#include <OpenCV/OpenCV.h>
#endif

#include "OpticalFlow.h"

class CVOpticalFlow : public OpticalFlow {
public:
	CVOpticalFlow() {}
	~CVOpticalFlow() {}
	
	virtual void analyze(MatrixXf, MatrixXf);
};

#endif