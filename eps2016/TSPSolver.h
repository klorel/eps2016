#ifndef __TSP_SOLVER_H_
#define __TSP_SOLVER_H_

#include "TSPInstance.h"
#include <xprs.h>

class TSPSolver{
public:
	TSPSolver(TSPInstance & instance);
	~TSPSolver();

	XPRSprob buildProblem();
public:
	TSPInstance * _instance;
	bool _log;
	std::set<IntSetPtr, IntSetPredicate> _knownSolution;
	int _nCuts;

	std::vector<DblVector> _heur_solutions;

public:
	int nPoints()const;
	int nVariables()const;

	void getBreakingCuts(IntListPtrList const &, DblVector & rhs, IntVector & mstart, IntVector & clind);
	bool log()const;
	bool &log();

	bool newSolution(DblVector const & sol, std::set<IntSetPtr, IntSetPredicate> & knownSolution);

	int add(IntListPtrList const & subTours, XPRSprob prob, bool isCut);
};

#endif