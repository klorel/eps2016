#ifndef __TSP_SOLVER_H_
#define __TSP_SOLVER_H_

#include "TSPFormulation.h"
#include <xprs.h>

class TSPSolver{
public:
	TSPSolver(TSPFormulation & instance);
	~TSPSolver();
	TSPFormulation const& getTSPFormulation()const;
	TSPFormulation & getTSPFormulation();
public:
	TSPFormulation * _tsp;
	
	std::set<IntSetPtr, IntSetPredicate> _knownSolution;
	int _nCuts;

	std::vector<DblVector> _heur_solutions;

public:
	bool log()const;
	bool &log();

	bool newSolution(DblVector const & sol, std::set<IntSetPtr, IntSetPredicate> & knownSolution);

	int add(IntListPtrList const & subTours, XPRSprob prob, bool isCut);
};

#endif