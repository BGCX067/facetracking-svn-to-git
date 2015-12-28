/*
 *  PCAFromFile.cpp
 *  FT2
 *
 *  Created by Andre Cohen on 1/28/09.
 *  Copyright 2009 Rutgers University. All rights reserved.
 *
 */

#include "PCAFromFile.h"
#include "Exception.h"
#include <fstream>

void PCAFromFile::loadFile(string filename, MatrixXf &m) {
	if(path.length()!=0) 
		filename = path+"/"+filename;
	
	//cout<<"Loading: "<<filename<<endl;
	
	// Open file
	ifstream in(filename.c_str());
	if(!in.is_open()) 
		throw Exception("Error opening PCA data file");
	
	// Read data into matrix
	for(int y=0; y<m.rows(); y++)
		for(int x=0; x<m.cols(); x++) 
			in>>m(y,x);
	
	// Close file
	in.close();
}
