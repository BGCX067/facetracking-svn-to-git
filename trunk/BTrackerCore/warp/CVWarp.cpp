/*
 *  CVWarp.cpp
 *  BTracker
 *
 *  Created by André Cohen on 11/19/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "CVWarp.h"

shared_ptr<Warp> CVWarp::getInstance() {
	return shared_ptr<Warp>(new CVWarp());
}

MatrixXi CVWarp::exec(MatrixXi frame, MatrixXf geom) {
	IplImage *dst = cvCreateImage(cvSize(48,48), IPL_DEPTH_8U, 1);

	CvMat *aff = cvCreateMat(2, 3, CV_32FC1);
	MatrixXf eAff = toAffine(geom.transpose());
	
	double c = cos(-eAff(0,3));
	double s = sin(-eAff(0,3));
	cvSet2D(aff, 0, 0, cvScalar(eAff(0,2)*c));
	cvSet2D(aff, 0, 1, cvScalar(eAff(0,2)*-s));
	cvSet2D(aff, 0, 2, cvScalar(eAff(0,0)));
	cvSet2D(aff, 1, 0, cvScalar(eAff(0,2)*s));
	cvSet2D(aff, 1, 1, cvScalar(eAff(0,2)*c));
	cvSet2D(aff, 1, 2, cvScalar(eAff(0,1)));			
	
	cvGetQuadrangleSubPix(currentFrame, dst, aff);
	
	MatrixXi res(48,48);
	for(int i=0; i<res.size(); i++)
		res(i) = (unsigned char)dst->imageData[i];
	
	cvReleaseMat(&aff);
	cvReleaseImage(&dst);
	
	return res;	
}

struct WarpJob {
	void run(int id) {
		samples[id]->clip = warp->exec(frame, samples[id]->parameters);
	}
	vector<int> *ids;
	Samples samples;
	Warp *warp;
	MatrixXi frame;
	pthread_mutex_t *lock;
};

void *WarpWorker(void *data) {
	WarpJob *j = (WarpJob*)data;
	while(true) {
		vector<int> queue;
		
		pthread_mutex_lock(j->lock);
		if(j->ids->size()) {
			for(int i=0; i<20 && j->ids->size(); i++) {
				queue.push_back(j->ids->at(0));
				j->ids->erase(j->ids->begin());
			}
		} else {
			pthread_mutex_unlock(j->lock);
			return NULL;
		}
		pthread_mutex_unlock(j->lock);

		for(int i=0; i<queue.size(); i++)
			j->run(queue[i]);
	}
}

Samples CVWarp::exec(MatrixXi frame, Samples samples) {
	currentFrame = cvCreateImage(cvSize(frame.cols(), frame.rows()), IPL_DEPTH_8U, 1);

	for(int i=0; i<frame.size(); i++)
		currentFrame->imageData[i] = frame(i);
	
	vector<int> ids;
	for(int i=0; i<samples.size(); i++)
		ids.push_back(i);
	
	pthread_mutex_t lock;
	pthread_mutex_init(&lock, NULL);
	
	pthread_attr_t attributes;
	pthread_attr_init(&attributes);
	pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_JOINABLE);
	
	static int numberOfThreads = 3;
	
	WarpJob job[numberOfThreads];
	for(int i=0; i<numberOfThreads; i++) {
		job[i].ids = &ids;
		job[i].samples = samples;
		job[i].warp = this;
		job[i].frame = frame;
		job[i].lock = &lock;
	}
	
	pthread_t threads[numberOfThreads];
	for(int i=0; i<numberOfThreads; i++) {
		pthread_create(&threads[i], &attributes, WarpWorker, (void*)&job[i]);
	}
	
	for(int i=0; i<numberOfThreads; i++)
		pthread_join(threads[i], NULL);
	
	//for(int i=0; i<samples.size(); i++)
	//	samples[i]->clip = exec(frame, samples[i]->parameters);

	cvReleaseImage(&currentFrame);
	
	return samples;
}

void CVWarp::exec(MatrixXi frame, shared_ptr<Sample> sample) {
	Samples s;
	s.push_back(sample);
	exec(frame, s);
}
