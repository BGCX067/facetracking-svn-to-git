/*
 *  FFVideo.h
 *  BTracker
 *
 *  Created by André Cohen on 10/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _FF_VIDEO_H_
#define _FF_VIDEO_H_

#include "Global.h"
extern "C" { 
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

class FFView {
public: 
	FFView(string);
	~FFView();
	
	virtual MatrixXi getMatrix();
	
	virtual int getWidth()	{ return codecContext->width; }
	virtual int getHeight()	{ return codecContext->height; }
	
private:
	bool decodeFrame();
	
	AVFormatContext *formatContext;
	AVCodecContext *codecContext;
	SwsContext *swsContext;
	
	AVCodec *codec;
	int videoStream;
	
	AVFrame *frameRGB;
	uint8_t *buffer;
	
	MatrixXi surface;
	
};

#endif