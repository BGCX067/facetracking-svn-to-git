/*
 *  Controller.h
 *  BTracker
 *
 *  Created by André Cohen on 9/16/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "Video.h"
#include "RandomSampler.h"
#include "Analyzer.h"
#include "OpticalFlow.h"
#include "Trace.h"
#include "Warp.h"

class Controller {
public:
	Controller();
	~Controller();
	
	virtual bool update();
	
	shared_ptr<Sample> getBestSample() { return bestSample; }
	MatrixXi getClip() { return clip; }
	MatrixXi getFrame() { return frame; }
	VectorXf getFlow() { 
		if(currentFlow.size())
			return currentFlow; 
		return VectorXf::Zero(2);
	}
protected:
	virtual	void doTrace();
	VectorXf doOpticalFlow(MatrixXi);
	
	void doExperimental(Samples);
	int bestSampleForMethod(Samples, int);
	
	void evaluate(shared_ptr<Sample>);
	void evaluate(Samples);
	shared_ptr<Sample> getBestSample(Samples);
	double evaluateScore(shared_ptr<Sample>);
	
	MatrixXi toRGB(MatrixXi);
	MatrixXi toBW(MatrixXi);
	
	shared_ptr<Video> video;
	shared_ptr<RandomSampler> sampler;
	shared_ptr<OpticalFlow> flow;
	shared_ptr<Trace> trace;
	shared_ptr<Warp> warp;
	
	Analyzers analyzers;

	shared_ptr<Sample> bestSample;
	
	MatrixXi clip, frame;
	VectorXf currentFlow;
	
};

#endif
