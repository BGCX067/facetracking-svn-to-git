/*
 *  Analyzer.h
 *  FT2
 *
 *  Created by Andre Cohen on 1/21/09.
 *  Copyright 2009 Rutgers University. All rights reserved.
 *
 */

#ifndef _ANALYZER_H_
#define _ANALYZER_H_

#include "Sample.h"

class Analyzer {
public:
	virtual double evaluate(shared_ptr<Sample>, shared_ptr<Sample>) = 0;
	virtual void begin(shared_ptr<Sample>) {}
	virtual void end(shared_ptr<Sample>) {}	
};

typedef vector< shared_ptr<Analyzer> > Analyzers;

#endif
