/*
 *  FFVideo.cpp
 *  BTracker
 *
 *  Created by André Cohen on 10/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "FFVideo.h"

void error(string msg, int code=0) {
	cerr<<code<<": \t"<<msg<<endl;
	exit(0);
}

void avError(void *ptr, int code, const char *str, va_list list) {
	cerr<<code<<": "<<str<<endl;
}

// Opens clip to be played with specified filename
FFView::FFView(string filename) {
	av_register_all();
	av_log_set_callback(&avError);
	int errno;
	
	// Attempts to open input file
	errno = av_open_input_file(&formatContext, filename.c_str(), NULL, 0, NULL);
	if(errno!=0) 
		error("Error opening video file", errno);
	
	// Can be used to print information about movie
	//dump_format(formatContext, 0, filename.c_str(), 0);
	
	// Searches for stream information
	errno = av_find_stream_info(formatContext);
	if(errno!=0)
		assert("Error reading video information");
	
	// Loop through the channels to find the video channel
	videoStream = -1;
	cout<<"Found "<<formatContext->nb_streams<<endl;
	for(unsigned int i=0; videoStream==-1 && i<formatContext->nb_streams; i++) {
		if(formatContext->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO) {
			videoStream = (int)i;
			break;
		}
	}
	
	// If none were found, there is no image to be played
	if(videoStream<0)
		error("Unable to find video stream");
	
	// Set the codec to match that of the video stream
	codecContext = formatContext->streams[videoStream]->codec;
	
	// Attempt to find information
	codec = avcodec_find_decoder(codecContext->codec_id);
	if(codec == NULL) 
		error("Unable to find codec");
	
	// Attempt to open codec
	if(avcodec_open(codecContext, codec)<0)
		error("Unable to open codec");
	
	frameRGB = NULL;
	buffer = NULL;
	swsContext = NULL;
}

FFView::~FFView() {
	av_close_input_file(formatContext);
	avcodec_close(codecContext);
	if(swsContext)
		sws_freeContext(swsContext);
	
}

// Accessor function to get frame. Not entirely needed
MatrixXi FFView::getMatrix() {
	if(decodeFrame() == 0) {
		return MatrixXi();
	}
	return surface;
}

// Helper function used to get frame from the clip
bool FFView::decodeFrame() {
	int finished=0;
	AVPacket packet;
	
	AVFrame *frame;
	frame = avcodec_alloc_frame();
	
	// Create frame buffer
	if(frameRGB==NULL) {
		frameRGB = avcodec_alloc_frame();
		int size = avpicture_get_size(PIX_FMT_RGB32, codecContext->width, codecContext->height);
		buffer = (uint8_t *)av_malloc(size*sizeof(uint8_t));
		avpicture_fill((AVPicture *)frameRGB, buffer, PIX_FMT_RGB32, codecContext->width, codecContext->height);
	}
	
	// Create decoder context
	if(swsContext==NULL) {
		swsContext = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt, codecContext->width, codecContext->height, PIX_FMT_RGB32, SWS_FAST_BILINEAR, NULL, NULL, NULL);
	}
	
	// Get one frame
	while(finished==0 && av_read_frame(formatContext, &packet)>=0) {	
		if(packet.stream_index == videoStream) {
			avcodec_decode_video(codecContext, frame, &finished, packet.data, packet.size);
		}
	}
	// No frames left
	if(finished==0)
		return 0;
	
	// Convert to RGB
	sws_scale(swsContext, frame->data, frame->linesize, 0, codecContext->height, frameRGB->data, frameRGB->linesize);
	
	// surface.data() contains an RGB array of pixels from the current frame.
	// Bellow is one manner to make use of the data, copying it to a matrix.
	// One could also use it as it is, or copy to some STL structure as well.
	
	// Create matrix
	surface = MatrixXi(codecContext->height, codecContext->width);
	// Copy to matrix
	memcpy(surface.data(), frameRGB->data[0], (codecContext->width*codecContext->height*4));
	
	return finished;
}