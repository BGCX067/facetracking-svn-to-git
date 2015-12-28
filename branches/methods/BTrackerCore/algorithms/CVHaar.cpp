/*
 *  CVHaar.cpp
 *  BTracker
 *
 *  Created by André Cohen on 11/1/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "CVWarp.h"
#include "CVHaar.h"
#include "Exception.h"




string CascadeFiles[] = {	
	"haarcascade_profileface_umist1.xml",
	"haarcascade_profileface_umist2.xml",
	"haarcascade_frontalface_default.xml", 
	"haarcascade_frontalface_alt_tree.xml", 
	"opencv-24x24.xml",
	"haar.xml", 
	"parojos-22x15.xml", 
	"parojosG-45x11.xml"
};

int CascadeSize[][2] = { {18,22}, {18,22}, {24,24}, {24,24}, {24,24}, {24,24}, {22,15}, {45,11} };
int CascadeCount = 4;


CVHaar::CVHaar() {
	string path = Config::getInstance()->getString("CVHaarPath");
	for(int i=0; i<CascadeCount; i++) {
		string fullpath = path+"/"+CascadeFiles[i];
		CvHaarClassifierCascade *cascade = (CvHaarClassifierCascade *)cvLoadHaarClassifierCascade(fullpath.c_str(), cvSize(CascadeSize[i][0], CascadeSize[i][1]));
		if(cascade == NULL)
			cerr<<"Error opening Haar cascade file"<<endl;
		else
			cascades.push_back(cascade);
	}
	
	if(cascades.size() == 0)
		throw Exception("No Haar cascades found");
}

CVHaar::~CVHaar() {
	for(int i=0; i<cascades.size(); i++)
		cvReleaseHaarClassifierCascade(&cascades[i]);
}

double CVHaar::evaluate(shared_ptr<Sample>, shared_ptr<Sample>) {
	throw Exception("CVHaar does not implement this function");
	return 0;
}

shared_ptr<Sample> CVHaar::evaluate(MatrixXi frame) {
	static float scale = Config::getInstance()->getFloat("HaarScale");
	static float neighbors = Config::getInstance()->getFloat("HaarNeighbors");
	
	IplImage *gray = cvCreateImage(cvSize(frame.cols(), frame.rows()), 8, 1);
	
	for(int i=0; i<frame.size(); i++)
		gray->imageData[i] = frame.transpose()(i);
	
	vector<CvSeq *> faces;
	vector<CvMemStorage *> storages;
	
	for(int i=0; i<cascades.size(); i++) {
		
		CvMemStorage* storage = NULL;
		while((storage = cvCreateMemStorage(0))==NULL){
			cerr<<"Error creating storage"<<endl;
		}
		
		CvSeq *face = NULL;
		
		if(cascades[i] != NULL)
			face = cvHaarDetectObjects(gray, cascades[i], storage, scale, neighbors, 0);
		
		faces.push_back(face);
		storages.push_back(storage);
	}
	
	int total = 0;
	for(int i=0; i<faces.size(); i++)
		if(faces[i] != NULL)
			total += faces[i]->total;
	//cout<<"Total: "<<total<<endl;
	
	VectorXf lastPoint = last->parameters;
	double distance = 0;
	CvRect *best = NULL;
	
	for(int f=0; f<faces.size(); f++) {
		if(faces[f]->total != 0) {
			for(int i=0; i<faces[f]->total; i++) {
				CvRect *r = (CvRect *)cvGetSeqElem(faces[f], i);
				
				double t = sqrt(pow(lastPoint(0)-r->x+r->width/2,2) + pow(lastPoint(1)-r->y+r->height/2,2)); 
				if(best==NULL || t<distance) {
					distance = t;
					best = r;
				}
			}
		}
	}
	
	if(best == NULL) {
		return shared_ptr<Sample>();
		//VectorXf bestPoint(6); 
		//bestPoint<<0,0,0,0,0,0;
		//shared_ptr<Sample> best = Sample::createFromParameters(bestPoint);
		//best->clip = MatrixXi::Zero(48,48);
		//return best;
	}
	
	VectorXf bestPoint(6);
	bestPoint<<best->x+best->width/2, best->y+best->height/2, (best->width * best->height)/(48*48.), 0, 1, 1;
	
	shared_ptr<Sample> bestSample = Sample::createFromParameters(bestPoint);
	
	#ifdef __LINUX__
		CVWarp::getInstance()->exec(frame, bestSample);
	#else
		CLWarp::getInstance()->exec(frame, bestSample);
	#endif
	
	for(int i=0; i<storages.size(); i++) {
		cvReleaseMemStorage(&storages[i]);
	}
	
	cvReleaseImage(&gray);
	
	return bestSample;
}

void CVHaar::begin(shared_ptr<Sample> sample) {
	last = sample;
}

void CVHaar::end(shared_ptr<Sample> sample) {
	last = sample;
}	
