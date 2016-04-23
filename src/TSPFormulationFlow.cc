#include "TSPFormulationFlow.h"
#include "callback.h"

TSPFlowFormulation::TSPFlowFormulation(std::string const & fileName) : TSPFormulation(fileName) {
	buildVariable();
}
TSPFlowFormulation::~TSPFlowFormulation(){

}
void TSPFlowFormulation::buildVariable() {
	int npoints(nPoints());
	int ncols(0);
	_variables.assign(npoints, IntVector(npoints, -1));
	for (int i(0); i < npoints; ++i) {
		for (int j(0); j < npoints; ++j) {
			if (i != j) {
				_variables[i][j] = ncols;
				++ncols;
			}
		}
	}
}

// build the MIP formulation without sub tour elimination constraints
XPRSprob TSPFlowFormulation::buildProblemLP(bool active_log) {
	log() = active_log;
	XPRSprob oprob;
	int status;
	status = XPRScreateprob(&oprob);
	if (status)
		errormsg("XPRScreateprob", __LINE__, status);
	XPRSsetcbmessage(oprob, cbmessage, this);
	if (status)
		errormsg("XPRSsetcbmessage", __LINE__, status);
	XPRSloadlp(oprob, "tsp", 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL);
	if (status)
		errormsg("XPRSloadlp", __LINE__, status);
	// create variables
	int npoints(nPoints());
	int nvariables(npoints * (npoints - 1));
	DblVector obj(nvariables, 0);
	IntVector ind(nvariables, 0);
	std::vector<std::string> colnames(nvariables);
	for (int i(0); i < npoints; ++i) {
		for (int j(0); j < npoints; ++j) {
			if (i != j) {
				int id(_variables[i][j]);
				obj[id] = _distances[i][j];
				ind[id] = id;
				std::stringstream buffer;
				buffer << "x_" << i << "_" << j;
				colnames[id] = buffer.str();
			}
		}
	}
	DblVector lower(nvariables, 0);
	DblVector upper(nvariables, XPRS_PLUSINFINITY);
	XPRSaddcols(oprob, nvariables, 0, obj.data(), NULL, NULL, NULL, lower.data(), upper.data());
	for (int i(0); i < nvariables; ++i){
		XPRSaddnames(oprob, 2, colnames[i].c_str(), i, i);
	}
	if (status)
		errormsg("XPRSaddcols", __LINE__, status);
	// create flow constraint
	IntVector mstart;
	IntVector clind;

	std::vector<std::string> rownames;
	for (int row(0); row < npoints; ++row) {
		mstart.push_back((int)clind.size());
		for (int col(0); col < npoints; ++col) {
			if (row != col){
				clind.push_back(getVariable(col, row));
			}
		}
		std::stringstream buffer;
		buffer << "in_flow_" << row;
		rownames.push_back(buffer.str());
	}
	for (int row(0); row < npoints; ++row) {
		mstart.push_back((int)clind.size());
		for (int col(0); col < npoints; ++col) {
			if (row != col){
				clind.push_back(getVariable(row, col));
			}
		}
		std::stringstream buffer;
		buffer << "out_flow_" << row;
		rownames.push_back(buffer.str());
	}
	CharVector rowType(mstart.size(), 'E');
	DblVector rhs(mstart.size(), 1);
	DblVector dmatval(clind.size(), 1);

	mstart.push_back((int)clind.size());
	XPRSaddrows(oprob, (int)rowType.size(), (int)clind.size(), rowType.data(), rhs.data(), NULL,
		mstart.data(), clind.data(), dmatval.data());
	for (int row(0); row < npoints; ++row){
		XPRSaddnames(oprob, 1, rownames[row].c_str(), row, row);
	}
	for (int row(0); row < npoints; ++row){
		XPRSaddnames(oprob, 1, rownames[row + npoints].c_str(), row + npoints, row + npoints);
	}
	if (status)
		errormsg("XPRSaddrows", __LINE__, status);

	return oprob;
}



// convert a MIP solution into a tour
void TSPFlowFormulation::getTour(DblVector const & input, IntVector &){

}
// convert a tour into a MIP solution
void TSPFlowFormulation::getSol(IntVector const & input, DblVector &){

}
// build a sub tour list from a MIP solution
int TSPFlowFormulation::getSubTours(DblVector const & sol, IntListPtrList &){
	return 0;

}
//
void TSPFlowFormulation::getBreakingCuts(IntListPtrList const & tours, DblVector & rhs, IntVector & mstart, IntVector & clind){

}
//
double TSPFlowFormulation::computeCost(DblVector const & input){
	return 0;

}
//
int TSPFlowFormulation::nVariables()const{
	return 0;

}