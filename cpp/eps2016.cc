

#include "TSPInstance.h"

#include "TSPAlgo.h"
#include "TSPSolver.h"

int main(int argc, char** argv){
	TSPInstance tsp(argv[1]);
	XPRSinit("");

	TSPAlgo algo;
	std::stringstream buffer(argv[2]);
	int kind;
	buffer >> kind;
	if (kind == 0)
		algo.run_iterative(tsp);
	else
		algo.run_callback(tsp);

	XPRSfree();
}