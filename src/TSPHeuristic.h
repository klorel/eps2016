#ifndef __TSP_HEURISTIC_H_
#define __TSP_HEURISTIC_H_

#include "TSPFormulation.h"
#include "TSPSolver.h"


class TSPHeuristic{
public:
	TSPHeuristic(TSPSolver &);
	~TSPHeuristic();
public:
	bool localSearch(IntVector const & input, SolutionHeur & output, bool log);

	size_t shake(size_t k, IntVector const & input, DblVector & lb, DblVector & ub);
	void vns(size_t kMax, IntVector const & input, SolutionHeur & output);

private:
	TSPFormulation * _tsp;
	TSPSolver * _solver;

};

#endif