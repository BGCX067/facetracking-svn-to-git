/*
 *  Controller.cpp
 *  BTracker
 *
 *  Created by André Cohen on 9/16/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Controller.h"
#include "Config.h"
#include "OpticalFlow.h"
#include "CVOpticalFlow.h"
#include "Exception.h"
#include "PCADistance.h"
#include "PCAPose.h"
#include "PCA.h"
#include "SVM.h"
#include "CVHaarClient.h"
#include "CVVideo.h"
#include "CVHaar.h"

#ifndef __LINUX__
#include "CLWarp.h"
#endif
#include "CVWarp.h"

#include "TrivialPredictor.h"

#define CheckIfEnabled(I, X) if(Config::getInstance()->getVector("Analyzers")(I) == 1) 

Controller::Controller() {
	// Start core compoenets
	try {
		video = shared_ptr<Video>(new CVVideo(Config::getInstance()->getString("Video")));
		
		if(ConfigInt("FlowMethod")==0)
			flow = shared_ptr<OpticalFlow>(new OpticalFlow());
		if(ConfigInt("FlowMethod")==1)
			flow = shared_ptr<OpticalFlow>(new CVOpticalFlow());

		#ifndef __LINUX__
			if(ConfigInt("WarpMethod")==0)
				warp = CLWarp::getInstance();
			else
				warp = CVWarp::getInstance();
		#else
			warp = CVWarp::getInstance();
		#endif
		
		if(Config::getInstance()->getInt("Trace")==1)
			trace = shared_ptr<Trace>(new Trace());
	}catch(Exception &e) {
		cerr<<"Error: "<<e.what()<<endl;	
	}
	
	// Update config variables
	Config::getInstance()->setInt("VideoWidth",video->getWidth());
	Config::getInstance()->setInt("VideoHeight",video->getHeight());
	
	// Get first sample
	VectorXf sampleVec = Config::getInstance()->getVector("InitialSample");
	bestSample = Sample::createFromParameters(sampleVec);
	try{
		frame = video->getMatrix();
		warp->exec(toBW(frame), bestSample);
	}catch(Exception &e) {
		cerr<<e.what()<<endl;
		exit(0);
	}
	sampler = shared_ptr<RandomSampler>(new RandomSampler(Sample::createFromParameters(sampleVec)));
	
	try{
		analyzers.push_back(shared_ptr<Analyzer>(new PCADistance()));
		analyzers.push_back(shared_ptr<Analyzer>(new PCAPose(Config::getInstance()->getString("PosePath"))));
		analyzers.push_back(shared_ptr<Analyzer>(new SVM(Config::getInstance()->getString("SVMPath"))));
		analyzers.push_back(shared_ptr<Analyzer>(new CVHaar()));
		analyzers.push_back(shared_ptr<Analyzer>(new TrivialPredictor()));
	}catch(Exception &e) {
		cerr<<e.what()<<endl;
		exit(0);
	}
	
	for(int i=0; i<analyzers.size(); i++) 
		analyzers[i]->begin(bestSample);
}

Controller::~Controller() {
}

/*
 
 Best = Best sample from each method
 Scores = Distance between 
 
 */

bool Controller::update() {
	// Get next frame 
	MatrixXi current = video->getMatrix();
	if(current.size()==0) { return false; }
	current = toBW(current);
	
	if(ConfigInt("Tracker")>0)
		doOpticalFlow(current);
	
	// Generate samples
	Samples samples = sampler->generateSamples();
	
	try{
		warp->exec(current, samples);
	}catch(Exception &e) {
		cout<<"Error: "<<e.what()<<endl;
		exit(0);
	}
	
	evaluate(samples);
	
	bestSample = getBestSample(samples);
	sampler->updateSample(bestSample);
	for(int i=0; i<analyzers.size(); i++)
		analyzers[i]->end(bestSample);
	if(trace.get())
		doTrace();
	
	// Set next frame to be the best sample
	clip = toRGB(bestSample->clip);
	frame = toRGB(current);
	
	return frame.size()>0;
}

void Controller::doTrace() {
	stringstream ss; 
	ss<<bestSample->parameters.transpose();
	trace->update(ss.str());
}

void Controller::doExperimental(Samples samples) {
}

int Controller::bestSampleForMethod(Samples samples, int m) {
	int id = -1;
	for(int i=0; i<samples.size(); i++) {
		if(id==-1 || (id>=0 && samples[i]->scores(m) < samples[id]->scores(m)))
			id = i;
	}
	return id;
}

struct WorkerJob {
	const void run(){
		for(int i=0; i<samples.size(); i++)
			samples[i]->scores(index) = ana->evaluate(best, samples[i]);
	}
	shared_ptr<Analyzer> ana;
	shared_ptr<Sample> best;
	Samples samples;
	int index;
};

void *Worker(void *data) {
	((WorkerJob *)data)->run();
	return NULL;
}

void Controller::evaluate(Samples samples) {
	WorkerJob jobs[3];
	for(int i=0; i<3; i++) {
		jobs[i].ana = analyzers[i];
		jobs[i].best = bestSample;
		jobs[i].samples = samples;
		jobs[i].index = i;
	}
	
	pthread_attr_t attributes;
	pthread_attr_init(&attributes);
	pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_JOINABLE);
	
	bestSample->getClipVector();
	for(int i=0; i<samples.size(); i++)
		samples[i]->getClipVector();
	
	pthread_t threads[3];
	for(int i=0; i<3; i++) 
		pthread_create(&threads[i], &attributes, Worker, (void*)&jobs[i]);
	
	for(int i=0; i<3; i++)
		pthread_join(threads[i], NULL);
	/*
	for(int i=0; i<samples.size(); i++) {
		evaluate(samples[i]);
	}
	*/
}

void Controller::evaluate(shared_ptr<Sample> sample) {
	sample->scores(0) = analyzers[0]->evaluate(bestSample, sample);
	sample->scores(1) = analyzers[1]->evaluate(bestSample, sample);
	sample->scores(2) = analyzers[2]->evaluate(bestSample, sample);
}

shared_ptr<Sample> Controller::getBestSample(Samples samples) {
	static double minSVM = -1;
	
	double bestScore;
	int id = -1;
	bool useSVM = false;
	
	for(int i=0; i<samples.size(); i++)
		if(samples[i]->scores(2)>minSVM)
			useSVM = true;
	
	VectorXf scores = VectorXf::Zero(samples.size());
	
	for(int i=0; i<samples.size(); i++) {
		if(useSVM!=true || samples[i]->scores(2)>minSVM) {
			scores(i) = evaluateScore(samples[i]);
		}
	}
	
	double total = scores.sum();
	scores /= total;
	for(int i=0; i<samples.size(); i++) {
		if(useSVM!=true || samples[i]->scores(2)>minSVM) {
			if(id==-1 || (id>=0 && scores(i)>bestScore)) {
				id = i;
				bestScore = scores(i);
			}
		}
	}
	
	return samples[id];
}

double Controller::evaluateScore(shared_ptr<Sample> sample) {
	static MatrixXf lambda = Config::getInstance()->getVector("ScoreWeights").transpose();
	static double condensig = .3;

	double s =  (lambda(0)*sample->scores(0)
				 -lambda(1)*sample->scores(1)
				 +lambda(2)*sample->scores(2))/condensig;
	return exp(s);
}

VectorXf Controller::doOpticalFlow(MatrixXi current) {
	static MatrixXi previous;

	VectorXf newFlow = VectorXf::Zero(2);
	
	if(previous.size()>0 && current.size()>0) {
		int x = (int)bestSample->parameters(0);
		int y = (int)bestSample->parameters(1);
		int c = (int)bestSample->parameters(2) * 24;
		
		c = (c<0)?abs(c):c;
		c = (c==0)?(24):(c);
		
		
		int minX = x-c;
		int minY = y-c;
		int maxX = 2*c;
		int maxY = 2*c;
		
		if(minX<0) minX = 0;
		if(minY<0) minY = 0;
		if(minX + maxX >= current.cols()) maxX = current.cols() - minX;
		if(minY + maxY >= current.rows()) maxY = current.rows() - minY;
			
		if(ConfigInt("FlowSize")==0)
			flow->analyze(previous.block(minY, minX, maxY, maxX).cast<float>(), 
							current.block(minY, minX, maxY, maxX).cast<float>());
		else if(ConfigInt("FlowSize")==1)
			flow->analyze(previous.cast<float>(), current.cast<float>());
		else 
			throw Exception("Optical flow method specified not known");

		
		for(int i=0; i<flow->X.size(); i++) {
			newFlow(0) += flow->X(i);
			newFlow(1) += flow->Y(i);
		}
		
		newFlow *= 1.0/flow->X.size();
		
		if(currentFlow.size() == 0)
			currentFlow = newFlow;
		else
			currentFlow = (currentFlow + newFlow)/2;

	}
	
	previous = current;
	
	return currentFlow;
}

MatrixXi Controller::toRGB(MatrixXi bw) {
	MatrixXi res(bw.rows(), bw.cols());
	for(int i=0; i<bw.size(); i++) {
		if(bw(i)>256)
			res(i) = 0xffffff;
		else
			res(i) = (bw(i)<<16) | (bw(i)<<8) | bw(i);
	}
	return res;
}

MatrixXi Controller::toBW(MatrixXi color) {
	if(color.size() == 0)
		return MatrixXi();
	MatrixXi res(color.rows(), color.cols());
	for(int i=0; i<color.size(); i++) {
		res(i) = (int)(((color(i) & 0xff0000)>>16) * .3) + (((color(i) & 0x00ff00)>>8) * .6) + (((color(i) & 0x0000ff)>>0) * .1); 
		
	}
	return res;
}
