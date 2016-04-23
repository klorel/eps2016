#ifndef __TSP_FORMULATION_DEGREE_H_
#define __TSP_FORMULATION_DEGREE_H_

#include "TSPFormulation.h"
#include <xprs.h>

class TSPFormulationDegree : public TSPFormulation {
public:
	TSPFormulationDegree(std::string const & fileName);
	virtual ~TSPFormulationDegree();
public:
	//
	virtual void buildVariable();
	// build the MIP formulation without sub tour elimination constraints
	virtual XPRSprob buildProblemLP(bool active_log);
	// convert a MIP solution into a tour
	virtual void getTour(DblVector const & input, IntVector &);
	// convert a tour into a MIP solution
	virtual void getSol(IntVector const & input, DblVector &);
	// build a sub tour list from a MIP solution
	virtual int getSubTours(DblVector const & sol, IntListPtrList &);
	//
	virtual void getBreakingCuts(IntListPtrList const & tours, DblVector & rhs, IntVector & mstart, IntVector & clind);
	//
	virtual double computeCost(DblVector const & input);
	//
	virtual int nVariables()const;
};

#endif
