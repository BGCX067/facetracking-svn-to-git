/*
 *  Video.h
 *  FaceTracker
 *
 *  Created by Andre Cohen on 11/9/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef VIDEO_H
#define VIDEO_H

#include "Global.h"

class Video {
public: 
	Video(string file) {
		videoFile = file;
	}
	~Video() {
	}
	
	virtual MatrixXi getMatrix() = 0;
	
	virtual int getWidth() = 0;
	virtual int getHeight()	= 0;
		
protected:
	string videoFile;
};

#endif
