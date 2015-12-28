/*
 *  NullPredictor.h
 *  BTracker
 *
 *  Created by André Cohen on 10/21/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _NULL_PREDICTOR_H_
#define _NULL_PREDICTOR_H_

#include "Analyzer.h"

class NullPredictor : public Analyzer {
public:
	virtual double evaluate(shared_ptr<Sample> best, shared_ptr<Sample> current) {
		return 0;
	}
	
};

#endif
