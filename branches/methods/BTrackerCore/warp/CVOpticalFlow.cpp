/*
 *  CVOpticalFlow.cpp
 *  BTracker
 *
 *  Created by André Cohen on 12/6/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "CVOpticalFlow.h"

void CVOpticalFlow::analyze(MatrixXf previous, MatrixXf current) {
	IplImage *image1 = cvCreateImage(cvSize(previous.cols(), previous.rows()), IPL_DEPTH_8U, 1);
	IplImage *image2 = cvCreateImage(cvSize(previous.cols(), previous.rows()), IPL_DEPTH_8U, 1);
	IplImage *pyramid1 = cvCreateImage(cvSize(previous.cols(), previous.rows()), IPL_DEPTH_8U, 1);
	IplImage *pyramid2 = cvCreateImage(cvSize(previous.cols(), previous.rows()), IPL_DEPTH_8U, 1);
	IplImage *temp = cvCreateImage(cvSize(previous.cols(), previous.rows()), IPL_DEPTH_32F, 1);
	IplImage *eigen = cvCreateImage(cvSize(previous.cols(), previous.rows()), IPL_DEPTH_32F, 1);
	
	for(int i=0; i<previous.size(); i++){
		image1->imageData[i] = previous(i);
		image2->imageData[i] = current(i);
	}
	
	CvPoint2D32f features1[400];
	CvPoint2D32f features2[400];
	char found[400];
	float error[400];
	CvSize window = cvSize(3,3);
	int featureCount = 400;
	CvTermCriteria criteria = cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, .3 );
	
	cvGoodFeaturesToTrack(image1, eigen, temp, features1, &featureCount, .01, .01, NULL);

	cvCalcOpticalFlowPyrLK(image1, image2, pyramid1, pyramid2, features1, features2, featureCount, window, 5, found, error, criteria, 0);
	
	int totalFound = 0;
	for(int i=0; i<400; i++)
		if(found[i])
			totalFound++;
	
	X = MatrixXf::Zero(totalFound, 1);
	Y = MatrixXf::Zero(totalFound, 1);

	int count=0;
	for(int i=0; i<400; i++) {
		if(found[i]) {
			X(count) = (int)(features2[i].x - features1[i].x);
			if(X(count)==NAN || abs(X(count))>20)
				X(count)=0;
			Y(count) = (int)(features2[i].y - features1[i].y);
			if(Y(count)==NAN || abs(Y(count))>20)
				Y(count)=0;
			count++;
		}
	}

	//cout<<X.sum()/X.size()<<"\t"<<Y.sum()/Y.size()<<endl;
	
	cvReleaseImage(&image1);
	cvReleaseImage(&image2);
	cvReleaseImage(&pyramid1);
	cvReleaseImage(&pyramid2);
	cvReleaseImage(&eigen);
	cvReleaseImage(&temp);
}