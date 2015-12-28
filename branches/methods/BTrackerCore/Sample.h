/*
 *  Sample.h
 *  BTracker
 *
 *  Created by André Cohen on 9/19/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SAMPLE_H_
#define _SAMPLE_H_

#include "Global.h"
#include "Config.h"

class Sample {
public:
	static shared_ptr<Sample> createFromParameters(VectorXf par) {
		return shared_ptr<Sample>(new Sample(par));
	}
	
	MatrixXf getClipVector() { 
		if(vClip.size()==0) {
			vClip = clip.cast<float>();
			vClip /= 255;
			vClip.resize(vClip.size(), 1);
		}
		return vClip;
	}
	
	double distanceTo(shared_ptr<Sample> sample) {
		return sqrt(pow(parameters(0)-sample->parameters(0),2) + pow(parameters(1)-sample->parameters(1),2));
	}
	
	void setClip(MatrixXi c) {
		clip = c;
		getClipVector();
	}
	
	VectorXf parameters;
	MatrixXi clip;
	
	VectorXf scores;
	
	VectorXf poseScores;
	string poseName;

private:
	Sample(VectorXf par) {
		static int methods = Config::getInstance()->getVector("Analyzers").rows();
		vClip = MatrixXf();
		parameters = par;
		scores = VectorXf::Zero(methods);
	}
	
	MatrixXf vClip;
};

typedef vector< shared_ptr<Sample> > Samples;

#endif
