/*
 *  OpticalFlow.cpp
 *  BTracker
 *
 *  Created by André Cohen on 10/7/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "OpticalFlow.h"

void OpticalFlow::analyze(MatrixXf previous, MatrixXf current) {
	if(previous.size() == 0 || current.size() == 0)
		return;
	
	MatrixXf k1(2,2);
	k1<<-1,1,-1,1;
	k1 = k1 * .25;
	
	MatrixXf k2(2,2);
	k2<<-1,-1,1,1;
	k2 = k2 * .25;
	
	MatrixXf k3 = MatrixXf::Ones(2,2);
	k3 = k3 * .25;
	
	MatrixXf k4(3,3);
	k4<<1/12.,1/6.,1/12.,1/6.,0,1/6.,1/12.,1/6.,1/12.;
	
	previous = blur(previous, 2);
	current = blur(current, 2);
	
	MatrixXf fx = conv(previous, k1) + conv(current, k1);
	MatrixXf fy = conv(previous, k2) + conv(current, k2);
	MatrixXf ft = conv(previous, k3) + conv(current, k3);
	
	X = MatrixXf::Zero(previous.rows(), previous.cols());
	Y = MatrixXf::Zero(previous.rows(), previous.cols());
	
	int iter = 64;
	float alpha = .5;
	for(int i=0; i<iter; i++) {
		MatrixXf xAvg = conv(X, k4);
		MatrixXf yAvg = conv(Y, k4);
		
		X = xAvg - ( fx.cwise() * ( ( fx.cwise() * xAvg ) + ( fy.cwise() * yAvg ) + ft ) ).cwise() / ((fx.cwise().pow(2) + fy.cwise().pow(2)).cwise() + pow(alpha,2));
		Y = yAvg - ( fy.cwise() * ( ( fx.cwise() * xAvg ) + ( fy.cwise() * yAvg ) + ft ) ).cwise() / ((fx.cwise().pow(2) + fy.cwise().pow(2)).cwise() + pow(alpha,2));
	}
	
}

MatrixXf OpticalFlow::conv(MatrixXf data, MatrixXf kernel) {
	MatrixXf res = MatrixXf::Zero(data.rows(), data.cols());
	
	int kRowOffset = kernel.rows()%2;
	int kColOffset = kernel.cols()%2;
	
	for(int row=0; row<data.rows(); row++) {
		for(int col=0; col<data.cols(); col++) {
			
			float acc = 0;
			
			for(int i=-kRowOffset; i+kRowOffset<kernel.rows(); i++) {
				for(int j=-kColOffset; j+kColOffset<kernel.cols(); j++) {
					if(row+i>=0 && row+i<data.rows() && col+j>=0 && col+j<data.cols()) {
						acc += data(row+i, col+j) * kernel(i+kRowOffset, j+kColOffset);
					}
				}
			}
			
			res(row, col) = acc;
		}
	}
	
	return res;	
}

MatrixXf OpticalFlow::blur(MatrixXf data, int radius) {
	MatrixXf res(data.rows(), data.cols());
	for(int y = 0; y < data.rows(); y++) {
		for(int x = 0; x < data.cols(); x++) {
			float total = 0;
			for(int ky = -radius; ky <= radius; ky++)
				for(int kx = -radius; kx <= radius; kx++)
					if(y+ky>=0 && y+ky<data.rows() && x+kx>=0 && x+kx<data.cols())
						total += data(y + ky,x + kx);
			res(y,x) = total / pow((radius * 2.0 + 1), 2);
		}
	}
	return res;
}
