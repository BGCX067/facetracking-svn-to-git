/*
 *  CVHaar.h
 *  BTracker
 *
 *  Created by André Cohen on 11/1/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _CV_HAAR_H_
#define _CV_HAAR_H_

#ifdef __LINUX__
#include <opencv/cv.h>
#else
#include <OpenCV/OpenCV.h>
#include "CLWarp.h"
#endif

#include "Analyzer.h"
#include "Sample.h"

class CVHaar : public Analyzer {
public:
	CVHaar();
	~CVHaar();
	virtual double evaluate(shared_ptr<Sample>, shared_ptr<Sample>);

	shared_ptr<Sample> evaluate(MatrixXi);
	
	virtual void begin(shared_ptr<Sample>);
	virtual void end(shared_ptr<Sample>);
	
private:
	shared_ptr<Sample> last;
	vector<CvHaarClassifierCascade *> cascades;
};

#endif
