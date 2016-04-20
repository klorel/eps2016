#ifndef __TSP_ALGO_H_
#define __TSP_ALGO_H_

#include "TSPFormulation.h"
#include <xprs.h>

class TSPSolver;

class TSPAlgo{
public:
	TSPAlgo();
	~TSPAlgo();
public:
	void run_iterative(TSPFormulation &, IntVector & output, double & obj, DblVector const & lb, DblVector const & ub);
	void run_iterative(TSPFormulation &, IntVector & output, double & obj);
	void run_iterative(TSPFormulation & tsp, TSPSolver & solver, XPRSprob & oprob);
	void run_iterative(TSPFormulation & tsp, TSPSolver & solver, XPRSprob & oprob, bool log);

	void run_callback(TSPFormulation &, IntVector & output, double & obj);
	
private:
};

#endif