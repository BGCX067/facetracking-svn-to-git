/*
 *  Trace.h
 *  BTracker
 *
 *  Created by André Cohen on 11/4/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _TRACE_H_
#define _TRACE_H_

#include "Global.h"


class Trace {
public:
	Trace();
	~Trace();
	
	void update(string);

private:
	ofstream file;
	
};

#endif
