#include "TSPFormulation.h"
#include "callback.h"

TSPFormulation::~TSPFormulation() {

}

TSPFormulation::TSPFormulation(std::string const & fileName) :
		TSPInstance(fileName) {
}

int TSPFormulation::getVariable(int i, int j) {
	return _variables[i][j];
}

int TSPFormulation::setMip(XPRSprob oprob){
	CharVector b(nVariables(), 'B');
	IntVector ind(b.size());
	for (int i(0); i < b.size(); ++i){
		ind[i] = i;
	}
	int status = XPRSchgcoltype(oprob, (int)b.size(), ind.data(), b.data());
	if (status)
		errormsg("XPRSchgcoltype", __LINE__, status);
	return status;
}	

XPRSprob TSPFormulation::buildProblemMIP(bool active_log){
	XPRSprob result = buildProblemLP(active_log);
	setMip(result);
	return result;
}