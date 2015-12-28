/*
 *  Global.h
 *  BTracker
 *
 *  Created by André Cohen on 9/16/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/Eigen>
#include <Eigen/SVD>
#include <Eigen/QR>
using namespace Eigen;


#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/array.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>


using namespace boost;
namespace fs = boost::filesystem;

#include <iostream>
#include <vector>
#include <fstream>
#include <cstdio>
#include <cstdlib>
using namespace std;

#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_histogram.h>

#endif
