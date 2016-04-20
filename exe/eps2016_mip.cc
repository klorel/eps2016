

#include "TSPInstance.h"

#include "TSPHeuristic.h"
#include "TSPAlgo.h"
#include "TSPSolver.h"

int main(int argc, char** argv){
	TSPFormulation tsp(argv[1]);

	std::cout << "TSP with " << tsp.nPoints() << " points " << std::endl;
	XPRSinit("");

	TSPAlgo algo;
	std::stringstream buffer(argv[2]);
	int kind;
	buffer >> kind;
	IntVector output;
	double obj;
	//DblVector lb(tsp.nVariables(), 0);
	//DblVector ub(tsp.nVariables(), 1);
	//lb[1] = 1;
	//if (kind == 0)
	//	algo.run_iterative(tsp, output, obj, lb, ub);
	//else
	//	algo.run_callback(tsp, output, obj);

	if (kind == 0)
		algo.run_iterative(tsp, output, obj);
	else
		algo.run_callback(tsp, output, obj);

	XPRSfree();

	std::cout << "Tour is " << output << std::endl;
	std::cout << "Obj is " << obj << std::endl;
}