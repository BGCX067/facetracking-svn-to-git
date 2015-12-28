/*
 *  PCA.h
 *  FT2
 *
 *  Created by Andre Cohen on 1/22/09.
 *  Copyright 2009 Rutgers University. All rights reserved.
 *
 */

#ifndef _PCA_H_
#define _PCA_H_

#include "Global.h"
#include "Sample.h"

class PCA {
public:
	PCA();
	
	void updateWithSample(shared_ptr<Sample>);
	
	MatrixXf &getBasis() { return basis; }
	MatrixXf &getSingular() { return singular; }
	MatrixXf &getMean() { return mean; }
	double getElements() { return elements; }
	
private:
	void diag(MatrixXf &);
	double sumSquare(MatrixXf &);
	MatrixXf toDiag(MatrixXf &);
	
	MatrixXf history;
	int bufferCount;
	int count;
	
	MatrixXf basis;
	MatrixXf singular;
	MatrixXf mean;
	double elements;
};

#endif
