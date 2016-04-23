#include "TSPSolver.h"
#include "callback.h"


bool TSPSolver::log()const{
	return getTSPFormulation().log();
}
bool &TSPSolver::log(){
	return getTSPFormulation().log();
}
TSPSolver::TSPSolver(TSPFormulation & instance) :_tsp(&instance){
	_nCuts = 0;

}
TSPSolver::~TSPSolver(){

}

bool TSPSolver::newSolution(DblVector const & sol, std::set<IntSetPtr, IntSetPredicate> & knownSolution){
	IntSetPtr sol_set(new IntSet);
	for (int i(0); i < sol.size(); ++i){
		if (std::fabs(sol[i] - 1) < 1e-10)
			sol_set->insert(i);
	}

	bool result = knownSolution.insert(sol_set).second;
	return result;
}

int TSPSolver::add(IntListPtrList const & subTours, XPRSprob prob, bool isCut){
	DblVector rhs;
	IntVector mstart;
	IntVector clind;
	int nrows;
	int ncoeffs;
	CharVector sense;
	DblVector coeffs;
	IntVector _mtype;

	if (subTours.size() > 1){
		
		_tsp->getBreakingCuts(subTours, rhs, mstart, clind);
		mstart.push_back(static_cast<int>(clind.size()));

		nrows = (static_cast<int>(rhs.size()));
		ncoeffs = (static_cast<int>(clind.size()));
		sense.assign(nrows, 'L');
		coeffs.assign(ncoeffs, 1);

		if (isCut){
			_mtype.resize(rhs.size());
			for (int i(0); i < rhs.size(); ++i){
				_mtype[i] = _nCuts + i;
			}
			_nCuts += static_cast<int>(rhs.size());
			XPRSaddcuts(prob, nrows, _mtype.data(), sense.data(), rhs.data(), mstart.data(), clind.data(), coeffs.data());
			//XPRSstorecuts(prob, nrows, 2,  _mtype.data(), sense.data(), rhs.data(), mstart.data(), NULL, clind.data(), coeffs.data());
			//XPRSloadcuts(prob, -1, -1, -1, NULL);
		}
		else{
			XPRSaddrows(prob, nrows, ncoeffs, sense.data(), rhs.data(), NULL, mstart.data(), clind.data(), coeffs.data());
		}
	}
	return nrows;
}

TSPFormulation & TSPSolver::getTSPFormulation(){
	return *_tsp;
}
TSPFormulation const & TSPSolver::getTSPFormulation()const {
	return *_tsp;
}