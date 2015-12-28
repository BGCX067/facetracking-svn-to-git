/*
 *  Config.cpp
 *  BTracker
 *
 *  Created by André Cohen on 9/21/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Config.h"
#include "Exception.h"

static shared_ptr<Config> instance;

Config::Config() {
	try{
		loadConfig("settings.conf");
		loadConfig("video.conf");
		loadConfig("dynamic.conf");
	} catch(Exception &e) {
		cerr<<e.what()<<endl;
		exit(0);
	}
}

Config *Config::getInstance() {
	if(instance == NULL)
		instance = shared_ptr<Config>(new Config());
	return instance.get();
}

void Config::loadConfig(string path) {
	fs::ifstream file(path);
	
	if(file.is_open() == false) 
		throw Exception("Failed to open settings.conf");
	
	string var, val;
	char eq;
	int line = 0;
	
	while(file.eof() == false) {
		line++;
		if(file.peek() == -1 || file.peek() == '#' || file.peek()==10) {
			while(file.eof() == false && file.get()!='\n'){}
		} else { 
			file>>var>>eq;
			getline(file, val);
			
			if(val[0] == '"' && val[val.length() -1] != '"') {
				while(file.eof()==false && val[val.length() - 1] != '"') {
					val += file.get();
				}
				
				if(file.eof() && val[val.length() - 1] != '"') {
					throw Exception("Settings file has an error");
				}
			}
			
			if(val[1] == '"')
				val = val.substr(2, val.length()-3);

			if(var[0] != '#') {
				if(properties.find(var) != properties.end())
					cout<<"Warning("<<line<<"): Overwriting property ("<<var<<")"<<endl;
				properties.insert(pair<string, string>(var, val));
			}
		}
	}	
}

int Config::getInt(string prop) {
	return atoi(getString(prop).c_str());
}

float Config::getFloat(string prop) {
	return atof(getString(prop).c_str());
}

string Config::getString(string prop) {
	if(properties.find(prop) == properties.end()){
		cerr<<"Missing: "<<prop<<endl;
		throw Exception("Property not defined");
	}
	return properties[prop];
}

VectorXf Config::getVector(string prop) {
	VectorXf vec;
	vector<string> tmp;
	stringstream str(getString(prop));
	string num;

	while(getline(str, num, ' ')) {
		tmp.push_back(num);
	}
	
	vec = VectorXf(tmp.size());
	for(int i=0; i<tmp.size(); i++)
		vec(i) = atof(tmp[i].c_str());
	
	return vec;
}

void Config::setString(string name, string value) {
	if(properties.find(name) != properties.end())
		properties[name] = value;
	properties.insert(pair<string, string>(name, value));
}

void Config::setInt(string name, int value) {
	setString(name, lexical_cast<string>(value));
}
