/*
 *  PCAFromFile.h
 *  FT2
 *
 *  Created by Andre Cohen on 1/28/09.
 *  Copyright 2009 Rutgers University. All rights reserved.
 *
 */

#ifndef _PCA_FROM_FILE_H_
#define _PCA_FROM_FILE_H_

#include "PCADistance.h"

class PCAFromFile : public PCADistance {
public:
	PCAFromFile(string path) {
		this->path = path;
	}	
	
protected: 
	void loadFile(string, MatrixXf &);
	string path;
};

#endif
