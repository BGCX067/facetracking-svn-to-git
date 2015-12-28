/*
 *  CLWarp.cpp
 *  BTracker
 *
 *  Created by André Cohen on 9/21/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "CLWarp.h"
#include "Config.h"
#include "Exception.h"
#include "CLController.h"

static shared_ptr<CLWarp> instance;

CLWarp::CLWarp() {
	CLController::getInstance()->loadProgram("warp", Config::getInstance()->getString("WarpCL"));
}

shared_ptr<Warp> CLWarp::getInstance() {
	if(instance.get() == NULL)
		instance = shared_ptr<CLWarp>(new CLWarp());
	return instance;
}

void CLWarp::exec(MatrixXi frame, shared_ptr<Sample> sample) {
	Samples s;
	s.push_back(sample);
	exec(frame, s);
}

Samples CLWarp::exec(MatrixXi frame, Samples samples) {
	MatrixXf geom = MatrixXf::Zero(samples.size(), 6);
	
	for(int i=0; i<samples.size(); i++) {
		geom.row(i) = samples[i]->parameters;
	}
	
	MatrixXi res = exec(frame, toAffine(geom));
	
	for(int i=0; i<samples.size(); i++) {
		samples[i]->clip = res.block(i*48*48, 0, 48*48,1);
		samples[i]->clip.resize(48,48);
	}
	
	return samples;
}

MatrixXi CLWarp::exec(MatrixXi frame, MatrixXf affine) {	
	CLController *cl = CLController::getInstance();
	
	static int bH = Config::getInstance()->getInt("SampleHeight");
	static int bW = Config::getInstance()->getInt("SampleWidth");
	static int iW = frame.cols(); 
	static int iH = frame.rows();
	int samples = affine.rows();
	
	MatrixXi output = MatrixXi::Zero(bH * bW * affine.rows(), 1);
	int workPerThread = ceil((double)samples / cl->getNumberOfThreads("warp"));
	
	cl->createBuffer("warpCoord", CL_MEM_READ_ONLY, sizeof(float), affine.size());
	cl->createBuffer("warpImage", CL_MEM_READ_ONLY, sizeof(int), frame.size());
	cl->createBuffer("warpOutput", CL_MEM_WRITE_ONLY, sizeof(int), output.size());
	
	affine.transposeInPlace();
	cl->writeArrayToBuffer("warpCoord", affine.data(), sizeof(float), affine.size());
	cl->writeArrayToBuffer("warpImage", frame.data(), sizeof(int), frame.size());
	
	cl->setParameterWithBuffer("warp", 0, "warpCoord");
	cl->setParameterWithInt("warp", 1, bW);
	cl->setParameterWithInt("warp", 2, bH);
	cl->setParameterWithBuffer("warp", 3, "warpImage");
	cl->setParameterWithInt("warp", 4, iW);
	cl->setParameterWithInt("warp", 5, iH);
	cl->setParameterWithBuffer("warp", 6, "warpOutput");
	cl->setParameterWithInt("warp", 7, samples);
	cl->setParameterWithInt("warp", 8, workPerThread);
	
	cl->runKernel("warp");
	
	cl->readArrayFromBuffer("warpOutput", output.data(), sizeof(int), output.size());
	
	cl->deleteBuffer("warpCoord");
	cl->deleteBuffer("warpImage");
	cl->deleteBuffer("warpOutput");
	
	return output;
}

