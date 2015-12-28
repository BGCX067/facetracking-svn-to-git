/*
 *  CVHaarClient.h
 *  BTracker
 *
 *  Created by André Cohen on 10/16/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _CV_HAAR_CASCADE_H_
#define _CV_HAAR_CASCADE_H_

#include "Analyzer.h"

#include <boost/asio.hpp>

namespace ba = ::boost::asio;
namespace bip = ::boost::asio::ip;

class CVHaarClient : public Analyzer {
public:
	CVHaarClient();
	~CVHaarClient();
	
	static void setImage(MatrixXi);
	void process();
	virtual double evaluate(shared_ptr<Sample>, shared_ptr<Sample>);
	void end(shared_ptr<Sample>);
	
	MatrixXf getPoint();
	
private:
	MatrixXf lastPoint;
	MatrixXf point;
};

#endif
