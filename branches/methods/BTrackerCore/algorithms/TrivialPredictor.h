/*
 *  TrivialPredictor.h
 *  BTracker
 *
 *  Created by André Cohen on 10/20/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _TRIVIAL_PREDICTOR_H_
#define _TRIVIAL_PREDICTOR_H_

#include "Analyzer.h"

class TrivialPredictor : public Analyzer {
public:
	virtual double evaluate(shared_ptr<Sample>, shared_ptr<Sample>);
	virtual void begin(shared_ptr<Sample>);	
	virtual void end(shared_ptr<Sample>);
	
	MatrixXf predict();
	
private:
	MatrixXf lastPosition;
	MatrixXf flowEstimate;
};

#endif
