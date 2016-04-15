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
	int nPoints()const;
	int nVariables()const;
	void getSubTours(DblVector const & sol, IntListPtrList &);

	void getBreakingCuts(IntListPtrList const &, DblVector & rhs, IntVector & mstart, IntVector & clind);
	bool log()const;
	bool &log();

	bool isInteger(DblVector const &)const;
private:
	TSPInstance * _instance;
	IntMatrix _variables;
	bool _log;
	// cut generation part
public:
	std::set<IntSetPtr, IntSetPredicate> _knownSolution;
public:
	DblVector sol;
	IntListPtrList subTours;
	DblVector rhs;
	IntVector mstart;
	IntVector clind;
	int nrows;
	int ncoeffs;
	CharVector sense;
	DblVector coeffs;
	int _nCuts;
	IntVector _mtype;
public:
	int getMipSolution(XPRSprob prob);
	int getSolution(XPRSprob prob);
	int getSubTours();

	bool newSolution();

	void add(XPRSprob prob, bool isCut);
};

#endif