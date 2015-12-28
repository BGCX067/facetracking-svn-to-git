/*
 *  SVM.cpp
 *  FT2
 *
 *  Created by Andre Cohen on 1/29/09.
 *  Copyright 2009 Rutgers University. All rights reserved.
 *
 */

#include "SVM.h"
#include "Config.h"

SVM::SVM(string path) : PCAFromFile(path) {
	image = MatrixXf(Config::getInstance()->getInt("SampleSize"), 1);
	loadFile("SVM/m", image);
	
	basis = MatrixXf(Config::getInstance()->getInt("SampleSize"), 100);
	loadFile("SVM/B", basis);
	tBasis = MatrixXf(basis.transpose());
	
	singular = MatrixXf(1, 100);
	loadFile("SVM/lambda", singular);
	
	weight = MatrixXf(1, 100);
	loadFile("SVM/weight", weight);
}

double SVM::evaluate(shared_ptr<Sample> best, shared_ptr<Sample> curr) {
	static double SVMB0 = .5692089;
	
	MatrixXf res = weight * (tBasis * (curr->getClipVector() - image));
	
	return res(0,0) + SVMB0;
}
