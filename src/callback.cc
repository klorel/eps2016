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
	TSPFormulation *tsp = (TSPFormulation*)data;
	switch (nMsgLvl) {
		/* Print Optimizer error messages and warnings */
	case 4: /* error */
	case 3: /* warning */
	case 2: /* dialogue */
	case 1: /* information */
		// printf("%*s\n",nLen,sMsg);
		if (tsp->log())
			std::cout << message << std::endl;
		break;
		/* Exit and flush buffers */
	default:
		fflush(NULL);
		break;
	}

}

void XPRS_CC cbintsol(XPRSprob prob, void* vContext){
	TSPSolver*solver = (TSPSolver*)vContext;
	TSPFormulation * tsp(solver->_tsp);
	DblVector sol;
	int mip_infeas=	tsp->getSolution(prob, sol);
	// if it is a integer solution, mip_infeas <0 means the it is a heuristic solution
	if (mip_infeas <= 0){
		IntListPtrList subtours;
		int nSubTours = tsp->getSubTours(sol, subtours);
		// is it is a real tour
		if (nSubTours == 1){
			TSPHeuristic heuristic(*solver);
			IntVector tour;
			tsp->getTour(sol, tour);
			SolutionHeur output;
			// if we improve the solution
			if (heuristic.localSearch(tour, output, true)){
				tsp->getSol(output.second, sol);
				std::cout << "cost is " << tsp->computeCost(sol) << std::endl;
				// if it is a real new solution
				if (solver->newSolution(sol, solver->_knownHeurSolution)){
					std::stringstream buffer;
					buffer << "user_mip_sol_" << solver->_knownHeurSolution.size();
					std::string const sol_name(buffer.str());
					std::cout << "adding improved solution " << sol_name << std::endl;
					int status = XPRSaddmipsol(prob, (int)sol.size(), sol.data(), NULL, sol_name.c_str());
					if (status){
						errormsg("XPRSaddmipsol", __LINE__, status);
					}
				}
			}
		}
	}

}