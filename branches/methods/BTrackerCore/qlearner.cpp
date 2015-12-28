/*
 *  qlearner.cpp
 *  BTracker
 *
 *  Created by André Cohen on 12/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "qlearner.h"
#include "Config.h"

static shared_ptr<QLearner> instance;
static bool wasLoaded;

QLearner::QLearner() {
	wasLoaded = false;
	createRepeatQ();
	learningRate = ConfigFloat("QLearningRate");
	efficiency = ConfigFloat("QEfficiency");
}


QLearner::~QLearner() {
	if(wasLoaded == false) {
		stringstream filename;
		filename<<"q-"<<clock()<<".txt";

		fs::ofstream outFile(filename.str());

		outFile<<"# Learning rate\t"<<learningRate<<endl;
		outFile<<"# Efficiency\t"<<efficiency<<endl;
		
		for(int i=0; i<q.size(); i++){
			stringstream out;
			out<<q[i]<<endl<<endl;
			outFile<<out.str();
		}
	}	
}

shared_ptr<QLearner> QLearner::getInstance() {
	if(instance == NULL) 
		instance = shared_ptr<QLearner>(new QLearner());
	return instance;
}

void QLearner::loadLearnedData(string file) {
	wasLoaded = true;
	cout<<"RL - Loading Rewards"<<endl;
	fs::ifstream data(file);
	q.clear();
	
	string line;
	while(data.eof()==false) {
		if(data.peek() == '#')
			getline(data, line);
		else {
			MatrixXf partial(ConfigVector("FlowSegments").size(), 3);
			for(int m=0; m<partial.rows(); m++)
				for(int n=0; n<partial.cols(); n++)
					data>>partial(m, n);
			q.push_back(partial);
		}
	}
}

VectorXf QLearner::getDataFor(VectorXf flow, VectorXf repeat) {
	int fid = flowIndex(flow);
	VectorXf vals(repeat.size());	
	for(int i=0; i<vals.size(); i++) {
		if(repeat(i) > q.size())
			vals(i) = q[q.size() - 1](fid, i);
		else
			vals(i) = q[(int)repeat(i)](fid, i);
	}
	
	return vals;
}

int QLearner::compute(VectorXf flow, VectorXf repeat, VectorXf distance, VectorXf cost) {
	int fIndex = flowIndex(flow);
	
	// Update
	float MaxDistance = distance.sum();
	static float effic = ConfigFloat("QEfficiency");
	static float history = ConfigFloat("QLearningRate");

	for(int i=0; i<3; i++) {
		int r = (int)repeat(i);
		
		if(r>=q.size())
			createRepeatQ();
		
		float aErr = distance(i)/MaxDistance;
		float aCost = cost(i)/1000000;
		
		cout<<aErr<<"\t"<<aCost<<endl;
		
		float reward = ((1-effic)*aErr - (effic)*aCost)/2;
		
		if(i==1 && distance(i)==0)
			reward = 0;

		reward *= 100;
		if(q[r](fIndex, i) == 0)
			q[r](fIndex, i) = reward;
		else
			q[r](fIndex, i) = (1-history)*q[r](fIndex, i) + history*reward;
		
		
	}
	// Pick action
	int bestAction = pickBestAction(fIndex, repeat);
	
	
	if(distance(1)==0)
		return 0;
	return bestAction;
}

int QLearner::pickBestAction(int flow, VectorXf repeat) {	
	VectorXf rewards = VectorXf::Zero(3);
	for(int i=0; i<3; i++) {
		rewards(i) = q[(int)repeat(i)](flow, i);
	}
	
	VectorXf prob = VectorXf(3);
	for(int i=0; i<3; i++) {
		if(i==0)
			prob(i) = rewards(i);
		else
			prob(i) = rewards(i) + prob(i-1); 
	}
	prob /= rewards.sum();
	
	//cout<<prob.transpose()<<"\t";

	float pick = ((float)rand()/RAND_MAX);
	//cout<<pick<<endl;
	
	for(int i=0; i<3; i++)
		if(pick<prob(i))
			return i;
	return -1;
}

int QLearner::flowIndex(VectorXf flow) {
	float f = sqrt(flow.cwise().pow(2).sum());

	int index = 0;
	VectorXf segments = ConfigVector("FlowSegments");
	for(int i=0; i<segments.size(); i++)
		if(f<segments(i))
			return index;
		else 
			index++;
	return index;

}

void QLearner::createRepeatQ() {
	MatrixXf r = MatrixXf::Constant(ConfigVector("FlowSegments").size(), 3, ConfigFloat("QInitialValue"));
	
	q.push_back(r);
}
