/*
 *  TrivialPredictor.cpp 
 *  BTracker
 *
 *  Created by André Cohen on 10/20/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "TrivialPredictor.h"

double TrivialPredictor::evaluate(shared_ptr<Sample> best, shared_ptr<Sample> current) {
	
	if(flowEstimate.size() == 0) {
		flowEstimate = MatrixXf::Zero(lastPosition.rows(), 1);
	}
	
	MatrixXf eval = lastPosition + flowEstimate;
	eval = lastPosition - current->parameters;
	eval = eval.cwise().pow(2);
	
	double distance = eval.transpose().sum()/eval.rows();
	
	return distance;
}

void TrivialPredictor::begin(shared_ptr<Sample> best) {
	lastPosition = best->parameters;
}

void TrivialPredictor::end(shared_ptr<Sample> best) {
	flowEstimate  = best->parameters - lastPosition;
	lastPosition = best->parameters;
}

MatrixXf TrivialPredictor::predict() {
	static float PredictorDrag = ConfigFloat("PredictorDrag");
	if(flowEstimate.size() == 0) {
		flowEstimate = MatrixXf::Zero(lastPosition.rows(), 1);
	}
	
	VectorXf newPos = lastPosition + flowEstimate;
	newPos(2) = abs(newPos(2));
	newPos(3) = abs(newPos(3));
	
	return lastPosition + flowEstimate/PredictorDrag;
}
