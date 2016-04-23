

#include "TSPFormulationDegree.h"

#include "TSPHeuristic.h"
#include "TSPAlgo.h"
#include "TSPSolver.h"

int main(int argc, char** argv){
	TSPFormulationDegree tsp(argv[1]);
	XPRSinit("");
	TSPAlgo algo;
	if (argc > 2){
		std::stringstream buffer(argv[2]);
		int kind;
		buffer >> kind;
		algo.user_heuristic() = false;
		if (kind > 0)
			algo.user_heuristic() = true;
	}
	IntVector output;
	double obj;

	algo.run_iterative(tsp, output, obj);

	std::cout << "Tour is " << output << std::endl;
	std::cout << "Obj is " << obj << std::endl;

	XPRSfree();
}