/*
 *  Exception.h
 *  BTracker
 *
 *  Created by André Cohen on 9/21/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include "Global.h"

class Exception : public std::exception {
public:
	Exception(string message) {
		this->message = message;
		code = 0;
	}
	
	Exception(int code, string message) {
		this->message = message;
		this->code = code;
	}
	
	~Exception() throw(){
	}
	
	const char *what() { 
		stringstream ss;
		if(code != 0)
			ss<<"("<<code<<") ";
		ss<<message;
		return ss.str().c_str();
	}
	
private:
	string message;
	int code;
};

#endif
