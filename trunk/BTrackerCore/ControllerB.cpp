/*
 *  ControllerB.cpp
 *  BTracker
 *
 *  Created by André Cohen on 10/26/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include <iostream>
#include "ControllerB.h"
#include "Exception.h"
#include "CVHaarClient.h"
#include "PCA.h"
#include "TrivialPredictor.h"
#include "CVHaar.h"
#include "qlearner.h"
#include "CVVideo.h"

ControllerB::ControllerB() : Controller() {
	((TrivialPredictor *)analyzers[4].get())->begin(bestSample);
	((CVHaarClient *)analyzers[3].get())->end(bestSample);
	
	if(Config::getInstance()->getInt("Tracker")>0) {
		lastAction = -1;
	}
	
	counter = VectorXf(3);
	counter<<0, 0, 0;
}

bool ControllerB::update() {
	frame = video->getMatrix();
	if(frame.size() == 0)
		return false;
	frame = toBW(frame);
	
	doOpticalFlow(frame);
	sampler->setDynamic(getFlow());

	if(ConfigInt("Tracker")==2)
		doLearning();
	else
		pickNextAction();
		
	sampler->updateSample(bestSample);
	for(int i=0; i<analyzers.size(); i++){
		analyzers[i]->end(bestSample);
	}
	doTrace();

	clip = toRGB(bestSample->clip);
	frame = toRGB(frame);
	
	return (frame.size());
}

void ControllerB::doLearning() {
	Samples actions;
	MatrixXf times(3,2);
	times(0,0) = clock();
	actions.push_back(updateWithSampler(0));
	times(0,1) = clock();
	times(1,0) = clock();
	actions.push_back(updateWithoutSampler(3));
	times(1,1) = clock();
	times(2,0) = clock();
	actions.push_back(updateWithoutSampler(4));
	times(2,1) = clock();
	
	VectorXf groundPar = VectorXf::Zero(6);
	int found = 0;
	for(int i=0; i<actions.size(); i++)
		if(actions[i] != NULL) {
			groundPar += actions[i]->parameters;
			found++;
		}
	groundPar /= found;
	shared_ptr<Sample> groundSample = Sample::createFromParameters(groundPar);
	
	VectorXf distances(3);
	for(int i=0; i<actions.size(); i++) {
		if(actions[i]!=NULL) 
			distances(i) = groundSample->distanceTo(actions[i]);
		else
			distances(i) = 0;
	}
	
	int bid = QLearner::getInstance()->compute(getFlow(), counter, distances, times.col(1)-times.col(0));
	bestSample = actions[bid];
	
	if(lastAction==bid)
		counter(bid) += 1;
	else {
		counter<<0,0,0;
		counter(bid)++;
		lastAction = bid;
	}
	
	stringstream out;
	out<<getFlow().transpose()<<"\t"<<actionCount<<"\t"<<lastAction<<"\t"<<fixed<<(times.col(1)-times.col(0)).transpose();
	traceString = out.str();
}

int ControllerB::pickNextAction() {
	static bool start = 0;
	static int lastFlowIndex = -1;
	static int random = ConfigInt("QRandom");
	
	int bid = -1;
	
	if(random) {
		bid = rand()%3;	
	} else {
		if(start==0) {
			QLearner::getInstance()->loadLearnedData(ConfigString("QData"));
			start = 1;
		}
		
		if(lastFlowIndex != QLearner::getInstance()->flowIndex(getFlow())) {
			counter *= 0;
		}
		lastFlowIndex = QLearner::getInstance()->flowIndex(getFlow());
		
		VectorXf reward = QLearner::getInstance()->getDataFor(getFlow(), counter);
		VectorXf likelihood(reward.size());
		
		for(int i=0; i<likelihood.size(); i++)
			if(i==0)
				likelihood(i) = reward(i)/reward.sum();
			else 
				likelihood(i) = reward(i)/reward.sum() + likelihood(i-1);
		float r = ((float)rand())/RAND_MAX;
		for(int i=0; i<likelihood.size() && bid<0; i++)
			if(r<likelihood(i))
				bid = i;	
	}

	shared_ptr<Sample> best;
	
	switch(bid) {
		case 0:
			best = updateWithSampler(0);
			break;
		case 1:
			best = updateWithoutSampler(3);
			if(best == NULL)
				best = updateWithSampler(0);
			break;
		case 2:
			best = updateWithoutSampler(4);
			break;
	}
	
	bestSample = best;
	if(lastAction==bid)
		counter(bid) += 1;
	else {
		counter<<0,0,0;
		counter(bid)++;
		lastAction = bid;
	}
	
	return bid;
}

void ControllerB::doTrace() {
	if(trace.get() == NULL)
		return;
	if(traceString.size())
		trace->update(traceString);
	else {
		stringstream ss;
		ss<<lastAction<<"\t"<<counter.transpose()<<"\t"<<bestSample->parameters.transpose();
		trace->update(ss.str());
	}
}

shared_ptr<Sample> ControllerB::updateWithSampler(int analyzer) {
	Samples samples = sampler->generateSamples();
	
	try{
		warp->exec(frame, samples);
	}catch(Exception &e) {
		cout<<"Error: "<<e.what()<<endl;
		exit(0);
	}
	
	// Evaluate each sample
	for(int i=0; i<samples.size(); i++) {
		samples[i]->scores(0) = analyzers[analyzer]->evaluate(bestSample, samples[i]);
	}
	
	int id = -1;
	float max = 0;
	
	for(int i=0; i<samples.size(); i++) {
		if(id == -1 || (id!=-1 && samples[i]->scores(0)<max)) {
			max = samples[i]->scores(0);
			id = i;
		}
	}

	return samples[id];
}

shared_ptr<Sample> ControllerB::updateWithoutSampler(int analyzer) {
	shared_ptr<Sample> newSample;
	
	if(analyzer==3) {
		newSample = ((CVHaar *)analyzers[3].get())->evaluate(frame);
	} else if(analyzer == 4) {
		MatrixXf par = ((TrivialPredictor *)analyzers[analyzer].get())->predict();
		newSample = Sample::createFromParameters(par.col(0));
		warp->exec(frame, newSample);
	}

	if(newSample.get() && newSample->distanceTo(bestSample)>20 && ConfigInt("Tracker")!=1)
		return shared_ptr<Sample>();
	
	return newSample;
}
