#ifndef __TSP_INSTANCE_H_
#define __TSP_INSTANCE_H_

#include "common.h"

class TSPInstance{
public:
	Coordinates _coordinates;
	DblMatrix _distances;
	bool _log;
	//IntMatrix _variables;
public:
	TSPInstance();
	TSPInstance(std::string const & fileName);
	virtual  ~TSPInstance();

	int nPoints()const;
	bool & log();
	bool log()const;
public:
	double computeCost(IntVector const & input);
	//virtual double computeCost(DblVector const & input);
	virtual bool isInteger(DblVector const &)const;
	virtual void randomInit(IntVector & output);
	
	// get the MIP solution 
	int getMipSolution(XPRSprob prob, DblVector & sol);
	// get the LP solution 
	int getSolution(XPRSprob prob, DblVector & sol);
private:
	void read(std::string const & fileName);
	void buildDistance();
	
};

#endif