#ifndef __TSP_FORMULATION_H_
#define __TSP_FORMULATION_H_

#include "TSPInstance.h"
#include <xprs.h>

class TSPFormulation : public TSPInstance {
public:
	using TSPInstance::computeCost;
	TSPFormulation(std::string const & fileName);
	~TSPFormulation();
public:
	//
	virtual void buildVariable();
	// build the MIP formulation without sub tour elimination constraints
	virtual XPRSprob buildProblem(bool active_log);
	// convert a MIP solution into a tour
	virtual void getTour(DblVector const & input, IntVector &);
	// convert a tour into a MIP solution
	virtual void getSol(IntVector const & input, DblVector &);
	// build a sub tour list from a MIP solution
	virtual int getSubTours(DblVector const & sol, IntListPtrList &);
	//
	virtual void getBreakingCuts(IntListPtrList const & tours, DblVector & rhs, IntVector & mstart, IntVector & clind);
public:
	// id of variable associated with (i,j) or (j,i) if symmetric formulation
	int getVariable(int i, int j);
	//
	int nVariables()const;
	//
	double computeCost(DblVector const & input);

protected:
	// (i,j) return the id of the variable in the LP
	IntMatrix _variables;
};

#endif