

#include "TSPInstance.h"
#include "TSPHeuristic.h"

int main(int argc, char** argv){

	//std::srand(std::time(0)); // use current time as seed for random generator
	std::srand(10); // use current time as seed for random generator
	XPRSinit("");
	TSPInstance tsp(argv[1]);
	TSPSolver solver(tsp);
	TSPHeuristic heuristic(solver);

	IntVector input;

	std::stringstream buffer(argv[2]);
	int kind;
	buffer >> kind;
	SolutionHeur output;
	heuristic.randomInit(input);
	std::cout << input << std::endl;
	if (kind == 0){
		heuristic.localSearch(input, output);
	}
	else{
		heuristic.vns(kind, input, output);
	}
	XPRSfree();
}