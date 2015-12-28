/*
 *  PCA.cpp
 *  FT2
 *
 *  Created by Andre Cohen on 1/22/09.
 *  Copyright 2009 Rutgers University. All rights reserved.
 *
 */

#include "PCA.h"
#include "Config.h"

PCA::PCA() {
	history = MatrixXf(Config::getInstance()->getInt("SampleSize"), 
					  Config::getInstance()->getInt("PCABatchSize"));
	bufferCount = 0;
	count = 0;
}

void PCA::updateWithSample(shared_ptr<Sample> sample) {
	
	MatrixXf imageV = sample->getClipVector();
	
	history.col(count++%history.cols()) = imageV;

	MatrixXf buffer;
	if(count<history.cols())
		buffer = history.block(0, 0, history.rows(), count);
	else
		buffer = history;
	
	
	int n = buffer.cols();
	
	if(basis.size()==0) {
		mean = MatrixXf(buffer.rows(), 1);
		mean = buffer.rowwise().sum() / buffer.cols();
	
		for(int i=0; i<buffer.cols(); i++)
			buffer.col(i) -= mean.col(0);
		
		Eigen::SVD<MatrixXf> svd(buffer);
		basis = svd.matrixU();
		singular = svd.singularValues();
		elements = n;
	} else {
		static double ff = .9;
		
		// Update old mean
		MatrixXf m1(buffer.rows(), 1);
		m1 = buffer.rowwise().sum()/buffer.cols();
		
		for(int i=0; i<buffer.cols(); i++)
			buffer.col(i) -= m1.col(0);
		
		MatrixXf data(buffer.rows(), buffer.cols() + 1);
		data.block(0, 0, buffer.rows(), buffer.cols()) = buffer;
		data.col(data.cols()-1) = sqrt((n * elements)/(n + elements)) * (mean - m1);
		mean = (ff * elements * mean + n * m1)/(n + ff * elements);
		n = n + ff*elements;

		// Update basis
		MatrixXf s1 = VectorXf(singular).asDiagonal();

		MatrixXf tmp = MatrixXf::Zero(data.rows(), data.cols()+s1.cols());
		tmp.block(0, 0, basis.rows(), s1.cols()) = (ff* basis * s1).eval();
		tmp.block(0, s1.cols(), data.rows(), data.cols()) = data;
		
		Eigen::QR<MatrixXf> qr = tmp.qr();
		Eigen::SVD<MatrixXf> svd = qr.matrixR().svd();
		
		double cutoff = singular.cwise().pow(2).sum() * 1e-6;
		basis = svd.matrixU();
		
		vector<int> good;
		for(int i=0; i<svd.singularValues().rows(); i++) {
			if(pow(svd.singularValues()(i),2)>=cutoff)
				good.push_back(i);
		}
		singular = MatrixXf(good.size(), 1);
		for(int i=0; i<good.size(); i++) 
			singular(i) = svd.singularValues()(good[i]);
		singular = singular * sqrt(elements/n);
		
		basis = qr.matrixQ() * svd.matrixU().block(0,0,svd.matrixU().rows(), singular.rows());
		
		elements = n;
	}

	

	static int max = Config::getInstance()->getInt("PCAMaxSize");
	if(basis.cols() > max) {
		MatrixXf tmp = basis.block(0, 0, basis.rows(), max);
		basis = tmp;
		tmp = singular.block(0, 0, max, 1);
		singular = tmp;
	}
}

MatrixXf PCA::toDiag(MatrixXf &m) {
	MatrixXf res(m.cols(), 1);
	for(int i=0; i<m.cols(); i++)
		res(i, 0) = m(i, i);
	return res;
}

void PCA::diag(MatrixXf &m) {
	for(int y=0; y<m.rows(); y++)
		for(int x=0; x<m.cols(); x++) 
			if(x != y)
				m(y, x) = 0;
}

double PCA::sumSquare(MatrixXf &m) {
	double res = 0;
	for(int y=0; y<m.rows(); y++)
		for(int x=0; x<m.cols(); x++) 
			res = res + pow(m(y, x), 2);
	return res;
}
