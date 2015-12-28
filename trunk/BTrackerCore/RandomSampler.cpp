/*
 *  RandomSampler.cpp
 *  BTracker
 *
 *  Created by André Cohen on 9/19/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "RandomSampler.h"
#include "Exception.h"
#include "Config.h"

RandomSampler::RandomSampler(shared_ptr<Sample> sample) {
	updateSample(sample);
	dynamic = VectorXf::Zero(2);
}

RandomSampler::~RandomSampler() {
}

void RandomSampler::updateSample(shared_ptr<Sample> sample) {
	lastSample = sample;
}

Samples RandomSampler::generateSamples() {
	Samples samples;
	
	static float sigma = Config::getInstance()->getFloat("WeightsSigma");
	static int sampleCount = Config::getInstance()->getInt("Samples");
	static int maxX = Config::getInstance()->getInt("VideoWidth");
	static int maxY = Config::getInstance()->getInt("VideoHeight");
	static VectorXf weight = Config::getInstance()->getVector("Weights");
	static int distribution = Config::getInstance()->getInt("SamplerDistribution");

	VectorXf dWeights = weight;
	VectorXf lastLocation = lastSample->parameters;
	int dSampleCount = sampleCount;
	
	
	if(ConfigInt("Tracker")>0) {
		int experiment = ConfigInt("SamplerExperiment");
		if(experiment==1) {
			dWeights(0) += abs(dynamic(0)*2);
			dWeights(1) += abs(dynamic(1)*2);
			dSampleCount = (int)(dWeights(0)*dWeights(1))/(weight(0) * weight(1)) * sampleCount;
			dSampleCount = (dSampleCount>1500)?1500:dSampleCount;
		}else if(experiment==2) {
			lastLocation(0) += dynamic(0);
			lastLocation(1) += dynamic(1);
		}else if(experiment==3) {
			lastLocation(0) += dynamic(0);
			lastLocation(1) += dynamic(1);
			dSampleCount = (int)(dWeights(0)*dWeights(1))/(weight(0) * weight(1)) * sampleCount;
			dSampleCount = (dSampleCount>1500)?1500:dSampleCount;
		}else if(experiment==4) {
			lastLocation(0) += dynamic(0);
			lastLocation(1) += dynamic(1);
			dWeights(0) += abs(dynamic(0)*2);
			dWeights(1) += abs(dynamic(1)*2);
			dSampleCount = (int)(dWeights(0)*dWeights(1))/(weight(0) * weight(1)) * sampleCount;
			dSampleCount = (dSampleCount>1500)?1500:dSampleCount;
		}else if(experiment==5 || experiment==6) {
			if(suggested.size()>0 && suggested.sum()>10) {
				dSampleCount = dSampleCount * suggested.sum()/dWeights.sum();
				dSampleCount = (dSampleCount>1500)?1500:dSampleCount;
				dSampleCount = (dSampleCount<200)?200:dSampleCount;
				dWeights = suggested;
			}
			if(location.size() && experiment==6)
				lastLocation = location;
		}
	}
	
	mt19937 generator; 
	generator.seed(time(NULL));
	normal_distribution<double> normal(0, sigma);
	variate_generator<mt19937, normal_distribution<double> > randSample(generator, normal);
	
	int maxTries = dSampleCount*10;
	while(samples.size() < dSampleCount && maxTries-->0) {
		VectorXf par = lastLocation;
		VectorXf rnd;
		if(distribution == 0) {
			rnd = VectorXf::Constant(par.size(), 0);
			for(int i=0; i<par.size(); i++)
				rnd(i) = ((double)rand())/RAND_MAX;
		} else if(distribution == 1) {
			rnd = VectorXf::Constant(par.size(), 0);
			for(int i=0; i<par.size(); i++)
				rnd(i) = randSample();
		}

		par = par + rnd.cwise() * dWeights;
		
		if(par(0)>0 && par(0)<maxX && par(1)>0 && par(1)<maxY && par(2)>=0)
			samples.push_back(Sample::createFromParameters(par));
	}

	if(samples.size()==0)
		throw Exception("Unable to create any good samples!");
	
	return samples;
}
