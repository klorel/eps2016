#ifndef __TSP_FORMULATION_H_
#define __TSP_FORMULATION_H_

#include "TSPInstance.h"
#include <xprs.h>

class TSPFormulation : public TSPInstance {
public:
	using TSPInstance::computeCost;
	TSPFormulation(std::string const & fileName);
	virtual ~TSPFormulation();
public:
	//
	virtual void buildVariable() = 0;
	// build the LP relaxation of the MIP formulation without sub tour elimination constraints
	virtual XPRSprob buildProblemLP(bool active_log) = 0;
	// convert a MIP solution into a tour
	virtual void getTour(DblVector const & input, IntVector &) = 0;
	// convert a tour into a MIP solution
	virtual void getSol(IntVector const & input, DblVector &) = 0;
	// build a sub tour list from a MIP solution
	virtual int getSubTours(DblVector const & sol, IntListPtrList &) = 0;
	//
	virtual void getBreakingCuts(IntListPtrList const & tours, DblVector & rhs, IntVector & mstart, IntVector & clind) = 0;
	//
	virtual double computeCost(DblVector const & input) = 0;
	//
	virtual int nVariables()const = 0;
public:
	// id of variable associated with (i,j) or (j,i) if symmetric formulation
	int getVariable(int i, int j);
	// 
	int setMip(XPRSprob oprob);
	// build the MIP formulation without sub tour elimination constraints
	XPRSprob buildProblemMIP(bool active_log);
protected:
	// (i,j) return the id of the variable in the LP
	IntMatrix _variables;
};

#endif
