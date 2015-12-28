/*
 *  ControllerB.h
 *  BTracker
 *
 *  Created by André Cohen on 10/26/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _CONTROLLER_B_H_
#define _CONTROLLER_B_H_

#include "Controller.h"

class ControllerB : public Controller {
public:
	ControllerB();
	~ControllerB(){
	}
	virtual bool update();
	
	shared_ptr<Sample> updateWithSampler(int);
	shared_ptr<Sample> updateWithoutSampler(int);
	
protected:
	virtual void doTrace();
	
private:
	int pickNextAction();
	void doLearning();
	
	int lastAction;
	int actionCount;
	VectorXf qScores;
	VectorXf counter;
	string traceString;
};

#endif
