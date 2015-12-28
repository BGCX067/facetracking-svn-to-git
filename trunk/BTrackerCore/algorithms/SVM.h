/*
 *  SVM.h
 *  FT2
 *
 *  Created by Andre Cohen on 1/29/09.
 *  Copyright 2009 Rutgers University. All rights reserved.
 *
 */

#ifndef _SVM_H_
#define _SVM_H_

#include "PCAFromFile.h"

class SVM : public PCAFromFile {
public:
	SVM(string);
	
	virtual double evaluate(shared_ptr<Sample>, shared_ptr<Sample>);
	
private:
	MatrixXf image, basis, tBasis, singular, weight;
};


#endif
