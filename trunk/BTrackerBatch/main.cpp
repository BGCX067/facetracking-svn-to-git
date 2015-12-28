/*
 *  main.cpp
 *  BTracker
 *
 *  Created by André Cohen on 11/3/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Global.h"
#include "Config.h"
#include "Controller.h"
#include "ControllerB.h"
#include "Exception.h"

void parseArgs(int, char **);
void analyzeVideo();
void analyzeFolder(string, string, int, int);
string getArg(int, char **, string);

int main(int argc, char **argv) {
	
	if(argc>1) {
		parseArgs(argc, argv);

	}
	
	string videoFolder = getArg(argc, argv, "-videoFolder");
	string stateFolder = getArg(argc, argv, "-stateFolder");

	
	if(videoFolder != "" && stateFolder != "") {
		int increment = lexical_cast<int>(getArg(argc, argv, "-folderInc"));
		int max = lexical_cast<int>(getArg(argc, argv, "-folderMax"));
		analyzeFolder(videoFolder, stateFolder, increment, max);
	}
	else
		analyzeVideo();
	
	return 0;
	
}

void analyzeFolder(string videos, string states, int increment, int max) {
	fs::path vFolder(videos);
	fs::path sFolder(states);
	fs::directory_iterator vIter(vFolder);
	fs::directory_iterator sIter(sFolder);
	fs::directory_iterator end;
	
	int counter = 0;
	while(counter++<max && vIter != end && sIter != end) {
		if(is_regular_file(vIter->status())) {
			fs::ifstream stateFile(sFolder/sIter->leaf());
			stringstream data;
			for(int i=0; i<6; i++) {
				float val;
				stateFile>>val;
				data<<val<<" ";
			}
			
			Config::getInstance()->setString("Video", (vFolder/vIter->leaf()).string());
			Config::getInstance()->setString("InitialSample", data.str());
			cout<<"Video: "<<vIter->leaf()<<endl;
			
			try {
				analyzeVideo();
			}catch(std::exception &e) {
				cerr<<"Error with video "<<vIter->leaf()<<endl;
				cerr<<"("<<e.what()<<")\n";
			}
			
		}
		
		for(int i=0; sIter!=end && vIter!=end && i<increment; i++) {
			vIter++;
			sIter++;
		}
	}
	
}

void analyzeVideo() {
	shared_ptr<Controller> controller;
	
	try {
		if(ConfigInt("Tracker")==0) {
			controller = shared_ptr<Controller>(new Controller());
		} else {
			controller = shared_ptr<Controller>(new ControllerB());	
		}
	}catch(Exception &e) {
		cerr<<"Exception: "<<e.what()<<endl;
	}

	bool play = true;
	int counter = 0;
	static int playUpTo = ConfigInt("PlayUpToFrame");
	while(play && (playUpTo<1 || counter++<playUpTo)) {
		play = controller->update();
	}	
}

void parseArgs(int argc, char **argv) {
	string argValue;
	
	if((argValue = getArg(argc, argv, "-tracker")) != "") 
		Config::getInstance()->setInt("Tracker", lexical_cast<int>(argValue));
	
	if((argValue = getArg(argc, argv, "-experiment")) != "") 
		Config::getInstance()->setInt("SampleExperiment", lexical_cast<int>(argValue));
	
	if((argValue = getArg(argc, argv, "-random")) != "") 
		Config::getInstance()->setInt("QRandom", lexical_cast<int>(argValue));
	
	if((argValue = getArg(argc, argv, "-video")) != "") 
		Config::getInstance()->setString("Video", argValue);
	
	if((argValue = getArg(argc, argv, "-state")) != "") 
		Config::getInstance()->setString("InitialSample", argValue);
	
	if((argValue = getArg(argc, argv, "-output")) != "") 
		Config::getInstance()->setString("TraceOutput", argValue);
		
	if((argValue = getArg(argc, argv, "-learningrate")) != "") 
		Config::getInstance()->setString("QLearningRate", argValue);
		
	if((argValue = getArg(argc, argv, "-efficiency")) != "") 
		Config::getInstance()->setString("QEfficiency", argValue);
}

string getArg(int argc, char **argv, string arg) {
	for(int i=1; i<argc; i+=2) {
			if(strcmp(arg.c_str(), argv[i])==0)
				if(i+1<argc)
					return argv[i+1];
				else
					return "";
	}
	return "";
}
