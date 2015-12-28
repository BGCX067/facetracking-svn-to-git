/*
 *  PCADistance.cpp
 *  FT2
 *
 *  Created by Andre Cohen on 1/21/09.
 *  Copyright 2009 Rutgers University. All rights reserved.
 *
 */

#include "Global.h"
#include "PCADistance.h"

double PCADistance::evaluate(shared_ptr<Sample> best, shared_ptr<Sample> curr) {
	VectorXf diff;
	
	diff = curr->getClipVector() - best->getClipVector();
	
	if(pca.getBasis().size()==0) {
		return diff.cwise().pow(2).sum();
	}

	MatrixXf coef = pca.getBasis().transpose() * diff;
	return (pca.getBasis() * coef - diff).cwise().pow(2).sum();

}

void PCADistance::end(shared_ptr<Sample> sample) {
	pca.updateWithSample(sample);
}

double PCADistance::evaluateWithBasis(shared_ptr<Sample> best, shared_ptr<Sample> curr, MatrixXf &basis) {
	MatrixXf diff = curr->getClipVector() - best->getClipVector();
	MatrixXf coef = basis.transpose() * diff;
	
	coef = basis * coef - diff;
	return coef.cwise().pow(2).sum();
}
