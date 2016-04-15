#include "callback.h"
#include <iomanip>

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
	int MIPINFEAS = solver->getSolution(prob);
	double obj;
	XPRSgetdblattrib(prob, XPRS_LPOBJVAL, &obj);
	bool isNew(solver->newSolution());
	if (MIPINFEAS == 0 && isNew){
		int nSubTour = solver->getSubTours();
		//std::cout << std::setw(10) << obj;
		//std::cout << std::setw(10) << isNew;
		//std::cout << std::setw(10) << nSubTour;
		//std::cout << std::endl;
		if (nSubTour > 1){
			solver->add(prob, true);
		}
	}
}


void XPRS_CC cbpreintsol(XPRSprob prob, void* vContext, int isheuristic, int* ifreject, double* cutoff){
	TSPSolver*solver = (TSPSolver*)vContext;
	
	if (isheuristic == 1){
		solver->getSolution(prob);
		*ifreject = 1;
		if (solver->newSolution()){
			int nSubTour = solver->getSubTours();
			if (nSubTour == 1){
				*ifreject = 0;
			}
		}
	}
}