#ifndef __TSP_ALGO_H_
#define __TSP_ALGO_H_

#include "TSPInstance.h"
#include <xprs.h>

class TSPSolver;

class TSPAlgo{
public:
	TSPAlgo();
	~TSPAlgo();
public:
	void run_iterative(TSPInstance &, IntVector & output, double & obj, DblVector const & lb, DblVector const & ub);
	void run_iterative(TSPInstance &, IntVector & output, double & obj);
	void run_iterative(TSPInstance & tsp, TSPSolver & solver, XPRSprob & oprob);
	void run_iterative(TSPInstance & tsp, TSPSolver & solver, XPRSprob & oprob, bool log);

	void run_callback(TSPInstance &, IntVector & output, double & obj);
	
private:
};

#endif