/*
 *  RandomSampler.h
 *  BTracker
 *
 *  Created by André Cohen on 9/19/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _RANDOM_SAMPLER_H_
#define _RANDOM_SAMPLER_H_

#include "Global.h"
#include "Sample.h"

class RandomSampler {
public:
	RandomSampler(shared_ptr<Sample>);
	~RandomSampler();
	
	Samples generateSamples();
	
	void updateSample(shared_ptr<Sample>);
	
	void setDynamic(VectorXf dyna) {
		dynamic = dyna;
	}
	
	void setSuggestedWeights(VectorXf sugg) {
		suggested = sugg;
	}
	void setSuggestedLocation(VectorXf loc) {
		location = loc;
	}
	
private:
	shared_ptr<Sample> lastSample;
	VectorXf dynamic;
	VectorXf suggested;
	VectorXf location;
};

#endif
