

#include "TSPInstance.h"
#include "TSPFormulationFlow.h"
#include "TSPHeuristic.h"
#include "TSPAlgo.h"
#include "TSPSolver.h"

int main(int argc, char** argv){
	TSPFlowFormulation tsp(argv[1]);
	tsp.buildVariable();
	XPRSinit("");
	XPRSprob prob = tsp.buildProblemLP(true);
	XPRSsetintcontrol(prob, XPRS_THREADS, 1);
	//XPRSwriteprob(prob, "flow_relaxed.lp", ".lp");
	XPRSminim(prob, "");
	tsp.setMip(prob);
	XPRSmipoptimize(prob, "");
	//XPRSwriteprob(prob, "flow_mip.lp", ".lp");
	XPRSfree();

}
