/*
 *  Warp.h
 *  BTracker
 *
 *  Created by André Cohen on 9/21/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _WARP_H_
#define _WARP_H_

#include "Global.h"
#include "Sample.h"

class Warp {
public:
	virtual MatrixXi exec(MatrixXi, MatrixXf) = 0;
	virtual Samples exec(MatrixXi, Samples) = 0;
	virtual void exec(MatrixXi, shared_ptr<Sample>) = 0;
	
protected:
	MatrixXf toAffine(MatrixXf geom) {
		MatrixXf aff(geom.rows(), geom.cols());
		
		MatrixXf s = geom.col(2);
		MatrixXf th = geom.col(3);
		MatrixXf r = geom.col(4);
		MatrixXf phi = geom.col(5);
		
		MatrixXf cth = th.cwise().cos();
		MatrixXf sth = th.cwise().sin();
		MatrixXf cph = phi.cwise().cos();
		MatrixXf sph = phi.cwise().sin();
		
		MatrixXf ccc = (cth.cwise() * cph).cwise() * cph;
		MatrixXf ccs = (cth.cwise() * cph).cwise() * sph;
		MatrixXf css = (cth.cwise() * sph).cwise() * sph;
		MatrixXf scc = (sth.cwise() * cph).cwise() * cph;
		MatrixXf scs = (sth.cwise() * cph).cwise() * sph;
		MatrixXf sss = (sth.cwise() * sph).cwise() * sph;
		
		aff.col(0) = geom.col(0);
		aff.col(1) = geom.col(1);
		aff.col(2) = s.cwise() * ( ccc + scs + r.cwise() * (css - scs));
		aff.col(3) = s.cwise() * ( r.cwise() * (ccs - scc) - ccs - sss);
		aff.col(4) = s.cwise() * ( scc - ccs + r.cwise() * (ccs + sss));
		aff.col(5) = s.cwise() * ( r.cwise() * (ccc + scs) - scs + css);
		
		return aff;
	}
	
private:
	
};

#endif
