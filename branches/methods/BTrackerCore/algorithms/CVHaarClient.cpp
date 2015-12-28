/*
 *  CVHaarClient.cpp
 *  BTracker
 *
 *  Created by André Cohen on 10/16/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "CVHaarClient.h"

static MatrixXi image;
static bool processNewImage;

CVHaarClient::CVHaarClient() {

}

CVHaarClient::~CVHaarClient() {

}

MatrixXf CVHaarClient::getPoint() {
	return point;
}

void CVHaarClient::setImage(MatrixXi newImage) {
	image = newImage;
	processNewImage = true;
}

double CVHaarClient::evaluate(shared_ptr<Sample> best, shared_ptr<Sample> current) {

	
	if(processNewImage) {
		process();
		processNewImage = false;
	}
	
	if(point(0)>0 && point(1)>0) {
		MatrixXf distance = current->parameters.block(0,0,2,1) + current->parameters.block(2,0,4,1)/2  - point;
		distance = distance.cwise().pow(2);
		return 1-1/sqrt(distance(0)+distance(1));
	}
	
	
	return 2;
}

void CVHaarClient::process() {
	static bool first = true;
	static ba::io_service io_service;
	static bip::tcp::socket socket(io_service);
	
	try{
		if(first) {
			bip::tcp::resolver resolver(io_service);
			
			bip::tcp::resolver::query query("localhost", "1111");
			
			bip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
			bip::tcp::resolver::iterator end;
			
			boost::system::error_code error = boost::asio::error::host_not_found;
			while (error && endpoint_iterator != end)
			{
				socket.close();
				socket.connect(*endpoint_iterator++, error);
			}
			first = false;
		}
		int rows = image.rows(), cols = image.cols();
		socket.write_some(boost::asio::buffer(&rows, sizeof(int)));
		socket.write_some(boost::asio::buffer(&cols, sizeof(int)));
		
		vector<unsigned char> img;
		for(int i=0; i<rows*cols; i++)
			img.push_back(image.transpose()[i]);
		socket.write_some(boost::asio::buffer(img));
		
		point = MatrixXf::Zero(4, 1);
		
		int num = 0;
		boost::asio::read(socket, boost::asio::buffer(&num, sizeof(int)));
		
		if(num == 0) {
			return;
		}
		
		MatrixXf points = MatrixXf::Zero(num, 4);
		for(int i=0; i<num; i++) {
			VectorXf p(4);
			boost::asio::read(socket, boost::asio::buffer(p.data(), sizeof(float) * 4));
			points.row(i) = p;
			
		}
		
		int id = -1;
		double d = 0;
		cout<<"** "<<lastPoint.transpose()<<endl;
		for(int i=0; i<points.rows(); i++) {
			cout<<points.row(i)<<endl;
			double t = sqrt(pow(lastPoint(0)-points(i,0)+points(i,2)/2,2) + pow(lastPoint(1)-points(i,1)+points(i,3)/2,2)); 
			cout<<t<<endl;
			if(id==-1 || t<d) {
				id = i;
				d = t;
			}
		}
		
		cout<<"-- "<<points.row(id)<<endl;
		point = points.row(id);
	}catch(std::exception &e) {
		cerr<<e.what()<<endl;
		exit(0);
	}	
}

void CVHaarClient::end(shared_ptr<Sample> best) {
	lastPoint = best->parameters;
}
