/*
 *  CLWarp.h
 *  BTracker
 *
 *  Created by André Cohen on 9/21/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _CL_WARP_H_
#define _CL_WARP_H_

#include <OpenCL/opencl.h>
#include "Warp.h"

class CLWarp : public Warp{
public:
	static shared_ptr<Warp> getInstance();
	
	virtual MatrixXi exec(MatrixXi, MatrixXf); 
	virtual Samples exec(MatrixXi, Samples); 
	virtual void exec(MatrixXi, shared_ptr<Sample>);
	
private:
	CLWarp();
	
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;
	cl_program program;
	cl_kernel kernel;
};

#endif
