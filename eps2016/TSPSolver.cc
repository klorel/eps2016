#include "TSPSolver.h"
#include "callback.h"


bool TSPSolver::log()const{
	return _log;
}
bool &TSPSolver::log(){
	return _log;
}
TSPSolver::TSPSolver(TSPInstance & instance) :_instance(&instance){
	_nCuts = 0;

}
TSPSolver::~TSPSolver(){

}
int TSPSolver::nPoints()const{
	return _instance->nPoints();
}
int TSPSolver::nVariables()const{
	return _instance->nVariables();
}
XPRSprob TSPSolver::buildProblem(){
	XPRSprob oprob;
	int status;
	status = XPRScreateprob(&oprob);
	if (status)errormsg("XPRScreateprob", __LINE__, status);
	XPRSsetcbmessage(oprob, cbmessage, this);
	if (status)errormsg("XPRSsetcbmessage", __LINE__, status);
	XPRSloadlp(oprob, "tsp", 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	if (status)errormsg("XPRSloadlp", __LINE__, status);
	// create variables
	int npoints(nPoints());
	int ncols(0);
	DblVector obj(_instance->nVariables(), 0);
	IntVector ind(_instance->nVariables(), 0);
	for (int i(0); i < npoints; ++i){
		for (int j(i + 1); j < npoints; ++j, ++ncols){
			obj[ncols] = _instance->_distances[i][j];
			ind[ncols] = ncols;
		}
	}
	DblVector l(ncols, 0);
	DblVector u(ncols, 1);
	XPRSaddcols(oprob, ncols, 0, obj.data(), NULL, NULL, NULL, l.data(), u.data());
	if (status)errormsg("XPRSaddcols", __LINE__, status);
	CharVector b(ncols, 'B');
	XPRSchgcoltype(oprob, ncols, ind.data(), b.data());
	if (status)errormsg("XPRSchgcoltype", __LINE__, status);
	// create degree constraint
	IntVector mstart(npoints + 1);
	IntVector clind(npoints*(npoints - 1));
	DblVector dmatval(npoints*(npoints - 1), 1);

	int ncoeffs(0);
	for (int row(0); row < npoints; ++row){
		mstart[row] = ncoeffs;
		for (int col(0); col < row; ++col, ++ncoeffs){
			clind[ncoeffs] = _instance->getVariable(row, col);
		}
		for (int col(row + 1); col < npoints; ++col, ++ncoeffs){
			clind[ncoeffs] = _instance->getVariable(row, col);
		}
	}
	mstart.back() = ncoeffs;
	CharVector rowType(npoints, 'E');
	DblVector rhs(npoints, 2);
	XPRSaddrows(oprob, npoints, ncoeffs, rowType.data(), rhs.data(), NULL, mstart.data(), clind.data(), dmatval.data());
	if (status)errormsg("XPRSaddrows", __LINE__, status);
	return oprob;
}

void TSPSolver::getBreakingCuts(IntListPtrList const & tours, DblVector & rhs, IntVector & mstart, IntVector & clind){
	rhs.reserve(tours.size());
	mstart.reserve(rhs.size() + 1);

	rhs.clear();
	mstart.clear();
	clind.clear();

	for (auto const & tourPtr : tours){
		IntList const & tour(*tourPtr);
		if (tour.size() > 1){
			rhs.push_back(static_cast<int>(tour.size() - 1));
			mstart.push_back(static_cast<int>(clind.size()));
			for (auto i : tour){
				for (auto j : tour){
					if (i < j){
						clind.push_back(_instance->getVariable(i, j));
					}
				}
			}
		}
	}
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
		getBreakingCuts(subTours, rhs, mstart, clind);
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
		}
		else{
			XPRSaddrows(prob, nrows, ncoeffs, sense.data(), rhs.data(), NULL, mstart.data(), clind.data(), coeffs.data());
		}
	}
	return nrows;
}
