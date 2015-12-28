/*
 *  PCAPose.h
 *  FT2
 *
 *  Created by Andre Cohen on 1/28/09.
 *  Copyright 2009 Rutgers University. All rights reserved.
 *
 */

#ifndef _PCA_POSE_H_
#define _PCA_POSE_H_

#include "PCAFromFile.h" 

class PCAPose : public PCAFromFile {
public:
	PCAPose(string);
	
	virtual double evaluate(shared_ptr<Sample>, shared_ptr<Sample>);
private:
	vector<MatrixXf > image;
	vector<MatrixXf > singular;
	vector<MatrixXf > basis;
	vector<MatrixXf > tBasis;
};

#endif
