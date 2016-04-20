#ifndef __TSP_HEURISTIC_H_
#define __TSP_HEURISTIC_H_

#include "TSPInstance.h"
#include "TSPSolver.h"

typedef std::pair<double, IntVector> SolutionHeur;
class TSPHeuristic{
public:
	TSPHeuristic(TSPSolver &);
	~TSPHeuristic();

	bool localSearch(IntVector const & input, SolutionHeur & output);

	void shake(size_t k, IntVector const & input, DblVector & lb, DblVector & ub);
	void vns(size_t kMax, IntVector const & input, SolutionHeur & output);

	void randomInit(IntVector & output);
private:
	TSPInstance * _instance;
	TSPSolver * _solver;
};

#endif