/*
 *  CLController.h
 *  BTracker
 *
 *  Created by André Cohen on 10/3/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _CL_CONTROLLER_H_
#define _CL_CONTROLLER_H_

#include <OpenCL/opencl.h>
#include "Global.h"

class CLController {
public:
	
	static CLController *getInstance();
	~CLController();
	
	void loadProgram(string, string);
	
	void createBuffer(string, cl_mem_flags, int, int);
	void deleteBuffer(string);
	void writeArrayToBuffer(string, void *, int, int);
	
	void readArrayFromBuffer(string, void *, int, int);
	
	void setParameterWithBuffer(string, int, string);
	void setParameterWithInt(string, int, int);
	
	void runKernel(string);
	
	int getNumberOfThreads(string);
	
private:
	
	CLController();
	
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;
	
	map<string, cl_program> programs;
	map<string, cl_kernel> kernels;
	map<string, cl_mem> buffers;
};

#endif