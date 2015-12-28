/*
 *  PCADistance.h
 *  FT2
 *
 *  Created by Andre Cohen on 1/21/09.
 *  Copyright 2009 Rutgers University. All rights reserved.
 *
 */

#ifndef _PCA_DISTANCE_H_
#define _PCA_DISTANCE_H_

#include "Analyzer.h"
#include "PCA.h"

class PCADistance : public Analyzer {
public:
	virtual double evaluate(shared_ptr<Sample>, shared_ptr<Sample>);
	virtual void end(shared_ptr<Sample>);
protected:
	
	double evaluateWithBasis(shared_ptr<Sample>, shared_ptr<Sample>, MatrixXf &);

private:
	PCA pca;
	
};

#endif
