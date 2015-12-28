/*
 *  qlearner.h
 *  BTracker
 *
 *  Created by André Cohen on 12/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _Q_LEARNER_H_
#define _Q_LEARNER_H_

#include "Global.h"

class QLearner {
public:
	~QLearner();
	static shared_ptr<QLearner> getInstance();
	
	void loadLearnedData(string);
	int compute(VectorXf, VectorXf, VectorXf, VectorXf);
	VectorXf getDataFor(VectorXf, VectorXf);
	
	int flowIndex(VectorXf);
private:	
	QLearner();
	
	float efficiency;
	float learningRate;
	
	void createRepeatQ();
	int pickBestAction(int, VectorXf);
	
	vector<MatrixXf> q;
	
};

#endif
