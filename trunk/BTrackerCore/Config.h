/*
 *  Config.h
 *  BTracker
 *
 *  Created by André Cohen on 9/21/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "Global.h"

#define ConfigInt(X)	(Config::getInstance()->getInt(X))
#define ConfigFloat(X)	(Config::getInstance()->getFloat(X))
#define ConfigString(X)	(Config::getInstance()->getString(X))
#define ConfigVector(X)	(Config::getInstance()->getVector(X))

class Config {
public:
	static Config *getInstance();
	
	int getInt(string);
	float getFloat(string);
	string getString(string);
	VectorXf getVector(string);
	
	void setString(string, string);
	void setInt(string, int);
	
	void loadConfig(string);
	
private:
	Config();
	
	map<string, string> properties;
};

#endif
