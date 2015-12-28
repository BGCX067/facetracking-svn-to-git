/*
 *  OpticalFlow.h
 *  BTracker
 *
 *  Created by André Cohen on 10/7/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _OPTICAL_FLOW_H_
#define _OPTICAL_FLOW_H_

#include "Global.h"

class OpticalFlow {
public:
	OpticalFlow(){}
	~OpticalFlow(){}
	virtual void analyze(MatrixXf, MatrixXf);
	
	MatrixXf X, Y;
	
private:
	
	MatrixXf conv(MatrixXf, MatrixXf);
	MatrixXf blur(MatrixXf, int);
};

#endif
