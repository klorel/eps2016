#include "callback.h"
#include <iomanip>
#include "TSPHeuristic.h"

void XPRS_CC errormsg(const char *sSubName, int nLineNo, int nErrCode) {
	int nErrNo(0); /* Optimizer error number */

	/* Print error message */
	printf("The subroutine %s has failed on line %d\n", sSubName, nLineNo);

	/* Append the error code, if it exists */
	if (nErrCode != -1)
		printf("with error code %d\n", nErrCode);

	/* Free memory, close files and exit */
	XPRSfree();
	exit(nErrCode);
}

void XPRS_CC cbmessage(XPRSprob prob, void* data, const char *sMsg, int nLen, int nMsgLvl) {
	std::string message;
	message.assign(sMsg, nLen);
	TSPSolver*solver = (TSPSolver*)data;
	switch (nMsgLvl) {
		/* Print Optimizer error messages and warnings */
	case 4: /* error */
	case 3: /* warning */
	case 2: /* dialogue */
	case 1: /* information */
		// printf("%*s\n",nLen,sMsg);
		if (solver->log())
			std::cout << message << std::endl;
		break;
		/* Exit and flush buffers */
	default:
		fflush(NULL);
		break;
	}

}
void XPRS_CC cboptnode(XPRSprob prob, void* vContext, int* feas){
	TSPSolver*solver = (TSPSolver*)vContext;
	TSPInstance * tsp(solver->_instance);
	DblVector sol;
	IntListPtrList subtours;


	if (!solver->_heur_solutions.empty()){
		for (auto & sol : solver->_heur_solutions)
			XPRSaddmipsol(prob, sol.size(), sol.data(), NULL, NULL);
		solver->_heur_solutions.clear();
	}

	int MIPINFEAS = tsp->getSolution(prob, sol);
	double obj;
	XPRSgetdblattrib(prob, XPRS_LPOBJVAL, &obj);
	bool isNew(solver->newSolution(sol, solver->_knownSolution));
	if (MIPINFEAS == 0 && isNew){
		int nSubTour = tsp->getSubTours(sol, subtours);
		//std::cout << std::setw(10) << obj;
		//std::cout << std::setw(10) << isNew;
		//std::cout << std::setw(10) << nSubTour;
		//std::cout << std::endl;
		if (nSubTour > 1){
			solver->add(subtours, prob, true);
		}
	}
}


void XPRS_CC cbpreintsol(XPRSprob prob, void* vContext, int isheuristic, int* ifreject, double* cutoff){
	TSPSolver*solver = (TSPSolver*)vContext;
	TSPInstance * tsp(solver->_instance);
	DblVector sol;
	IntListPtrList subtours;
	if (isheuristic == 1){
		tsp->getSolution(prob, sol);
		*ifreject = 1;
		if (solver->newSolution(sol, solver->_knownSolution)){
			int nSubTour = tsp->getSubTours(sol, subtours);
			if (nSubTour == 1){
				*ifreject = 0;
			}
		}
	}
}
void XPRS_CC cbintsol(XPRSprob prob, void* vContext){
	TSPSolver*solver = (TSPSolver*)vContext;
	TSPInstance * tsp(solver->_instance);
	DblVector sol;
	IntListPtrList subtours;
	tsp->getSolution(prob, sol);

	TSPHeuristic heuristic(*solver);
	IntVector tour;
	tsp->getTour(sol, tour);
	SolutionHeur output;
	if (heuristic.localSearch(tour, output, true)){
		tsp->getSol(output.second, sol);
		std::cout << "cost is " << tsp->computeCost(sol) << std::endl;;
		//XPRSaddmipsol(prob, sol.size(), sol.data(), NULL, NULL);
		solver->_heur_solutions.push_back(sol);
	}

}