

#include "TSPInstance.h"

#include "TSPHeuristic.h"
#include "TSPAlgo.h"
#include "TSPSolver.h"

int main(int argc, char** argv){
	TSPFormulation tsp(argv[1]);
	std::cout << "TSP with " << tsp.nPoints() << " points " << std::endl;
	XPRSinit("");
	XPRSprob prob =tsp.buildProblem(true);
	XPRSwriteprob(prob, "flow.lp", "lp");
	XPRSfree();

}
