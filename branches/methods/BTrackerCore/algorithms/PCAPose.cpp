/*
 *  PCAPose.cpp
 *  FT2
 *
 *  Created by Andre Cohen on 1/28/09.
 *  Copyright 2009 Rutgers University. All rights reserved.
 *
 */

#include "PCAPose.h"

string PoseFiles[] = {"Lprof", "Rprof", "L45", "R45", "Front"};

PCAPose::PCAPose(string path) : PCAFromFile(path) {
	for(int i=0; i<5; i++) {
		image.push_back(MatrixXf(48*48, 1));
		loadFile("Pose/"+PoseFiles[i]+"/m", image[i]);
		
		singular.push_back(MatrixXf(16, 1));
		loadFile("Pose/"+PoseFiles[i]+"/lambda", singular[i]);
		
		MatrixXf tmp(48*48, 16);
		loadFile("Pose/"+PoseFiles[i]+"/B", tmp);
		basis.push_back(tmp);
		tBasis.push_back(tmp.transpose());
	}
}

double PCAPose::evaluate(shared_ptr<Sample> best, shared_ptr<Sample> curr) {
	VectorXf scores(5);
	int bestPose = -1;

	for(int i=0; i<5; i++) {
	
		MatrixXf diff = (curr->getClipVector() - image[i]);
		MatrixXf coef = (tBasis[i] * diff);
		double d = (basis[i] * coef - diff).cwise().pow(2).sum();

		if(bestPose<0 || (bestPose>=0 && d<scores(bestPose))) {
			bestPose = i;
		}
		
		scores(i) = d;
	}

	curr->poseName = PoseFiles[bestPose];
	curr->poseScores = scores;

	return scores(bestPose);
}
