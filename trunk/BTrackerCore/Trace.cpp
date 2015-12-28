/*
 *  Trace.cpp
 *  BTracker
 *
 *  Created by André Cohen on 11/4/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Trace.h"
#include "Config.h"
#include "Exception.h"

Trace::Trace() {
	fs::path p(Config::getInstance()->getString("Video"));
	stringstream ss;
	ss<<ConfigString("TraceOutput")<<p.leaf()<<"."<<ConfigInt("Tracker")<<".txt";
	file.open(ss.str().c_str());
	if(file.is_open()==false) 
		throw Exception("Unable to open trace file");
}

Trace::~Trace() {
	file.close();
}

void Trace::update(string text) {
	file<<clock()<<"\t"<<text<<endl;
	file.flush();
}
