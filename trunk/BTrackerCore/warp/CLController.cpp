/*
 *  CLController.cpp
 *  BTracker
 *
 *  Created by André Cohen on 10/3/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "CLController.h"
#include "Exception.h"

static shared_ptr<CLController> instance;

CLController *CLController::getInstance() {
	if(instance.get() == NULL)
		instance = shared_ptr<CLController>(new CLController());
	return instance.get();
}

CLController::CLController() {
	cl_int error = 0;
	
	error = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	if(error != CL_SUCCESS) 
		throw Exception(error, "Error getting list of GPU devices");
	
	context = clCreateContext(NULL, 1, &device, NULL, NULL, &error);
	if(error != CL_SUCCESS) 
		throw Exception(error, "Error creating GPU context");
	
	queue = clCreateCommandQueue(context, device, NULL, &error);
	if(error != CL_SUCCESS) 
		throw Exception(error, "Error creating GPU command queue");	
}

CLController::~CLController() {
	map<string, cl_kernel>::iterator k = kernels.begin();
	for(; k!=kernels.end(); k++)
		clReleaseKernel((*k).second);

	clReleaseContext(context);
	clReleaseCommandQueue(queue);
}

void CLController::loadProgram(string name, string path) {
	cl_int error = 0;
	
	// Load CL code
	ifstream clFile(path.c_str());
	if(clFile.is_open() == false)
		throw Exception(error, "Error opening OpenCL source code");
	string clCode;
	while(clFile.peek() != EOF)
		clCode += clFile.get();
	
	// Create CL program
	const char *clCodeStart = clCode.c_str();
	
	cl_program program = clCreateProgramWithSource(context, 1, &clCodeStart, NULL, &error);
	if(error != CL_SUCCESS)
		throw Exception(error, "Error creating OpenCL program");
	
	error = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if(error != CL_SUCCESS) {
		char buffer[2048];
		size_t len;
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
		cerr<<buffer<<endl;
		throw Exception(error, "Error compiling OpenCL code");
	}
	
	cl_kernel kernel = clCreateKernel(program, name.c_str(), &error);
	if(error != CL_SUCCESS)
		throw Exception(error, "Error creating kernel");
	
	programs.insert(pair<string, cl_program>(name, program));
	kernels.insert(pair<string, cl_kernel>(name, kernel));
}
 
void CLController::createBuffer(string name, cl_mem_flags flags, int size, int length) {
	if(buffers.find(name) != buffers.end())
		return;
	cl_int error = 0;
	cl_mem buff = clCreateBuffer(context, flags, size * length, NULL, &error);
	if(error != CL_SUCCESS || buff==0)
		throw Exception(error, "Error creating OpenCL buffer");
	buffers.insert(pair<string, cl_mem>(name, buff));
}

void CLController::deleteBuffer(string name) {
	clReleaseMemObject(buffers[name]);
	buffers.erase(name);
}

void CLController::writeArrayToBuffer(string name, void *data, int size, int length) {
	cl_int error = clEnqueueWriteBuffer(queue, buffers[name], CL_TRUE, 0, size * length, data, 0, NULL, NULL);
	if(error != CL_SUCCESS)
		throw Exception(error, "Error adding buffer write to queue");
}

void CLController::setParameterWithBuffer(string kernel, int position, string buffer) {
	cl_int error = 0;
	error = clSetKernelArg(kernels[kernel], position, sizeof(cl_mem), &buffers[buffer]);
	if(error != CL_SUCCESS)
		throw Exception(error, "Error writing to buffer");
}

void CLController::setParameterWithInt(string kernel, int position, int value) {
	cl_int error = 0;
	clSetKernelArg(kernels[kernel], position, sizeof(int), &value);
	if(error != CL_SUCCESS)
		throw Exception(error, "Error writing to buffer");
}

int CLController::getNumberOfThreads(string kernel) {
	cl_int error = 0;
	size_t workers = 0;
	error = clGetKernelWorkGroupInfo(kernels[kernel], device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &workers, NULL);
	if(error != CL_SUCCESS) 
		throw Exception(error, "Error getting number of threads");
	return workers;
}

void CLController::runKernel(string kernel) {
	cl_int error = 0;
	size_t threads = getNumberOfThreads(kernel);
	size_t offset = threads;
	
	error = clEnqueueNDRangeKernel(queue, kernels[kernel], 1, NULL, &offset, &threads, 0, NULL, NULL);
	if(error != CL_SUCCESS)
		throw Exception(error, "Error adding kernel to queue");
	
	error = clFinish(queue);
	if(error != CL_SUCCESS)
		throw Exception(error, "Error running kernel");
}

void CLController::readArrayFromBuffer(string name, void *data, int size, int length) {
	cl_int error = clEnqueueReadBuffer(queue, buffers[name], CL_TRUE, 0, size * length, data, 0, NULL, NULL);
	if(error != CL_SUCCESS)
		throw Exception(error, "Error reading from buffer");
}
