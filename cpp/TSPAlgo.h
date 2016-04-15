#ifndef __TSP_ALGO_H_
#define __TSP_ALGO_H_

#include "TSPInstance.h"

class TSPAlgo{
public:
	TSPAlgo();
	~TSPAlgo();
public:
	void run_iterative(TSPInstance &);
	void run_callback(TSPInstance &);
private:
};

#endif