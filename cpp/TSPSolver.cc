#include "TSPSolver.h"
#include "callback.h"


bool TSPSolver::isInteger(DblVector const & sol)const{
	for (auto v : sol){
		if (std::fabs(v) > 1e-10 && std::fabs(1 - v) > 1e-10)
			return false;
	}
	return true;
}

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
	_variables.assign(npoints, IntVector(npoints, -1));
	for (int i(0); i < npoints; ++i){
		for (int j(i + 1); j < npoints; ++j, ++ncols){
			_variables[i][j] = ncols;
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
			clind[ncoeffs] = _variables[col][row];
		}
		for (int col(row + 1); col < npoints; ++col, ++ncoeffs){
			clind[ncoeffs] = _variables[row][col];
		}
	}
	mstart.back() = ncoeffs;
	CharVector rowType(npoints, 'E');
	DblVector rhs(npoints, 2);
	XPRSaddrows(oprob, npoints, ncoeffs, rowType.data(), rhs.data(), NULL, mstart.data(), clind.data(), dmatval.data());
	if (status)errormsg("XPRSaddrows", __LINE__, status);
	return oprob;
}



//
//for (Map.Entry<Pair<Integer, Integer>, XPRBvar> kvp : variables.entrySet()) {
//	final int i = kvp.getKey().getKey();
//	final int j = kvp.getKey().getValue();
//	final int sub_i = in_sub_tour[i];
//	final int sub_j = in_sub_tour[j];
//	final double solValue = kvp.getValue().getSol();
//	if (solValue > 1 - 1e-10 && in_sub_tour[i] != in_sub_tour[j]) {
//		LinkedList<Integer> clone_tour = new LinkedList<>(sub_tour[sub_j]);
//		sub_tour[sub_j].clear();
//		sub_tour[sub_i].addAll(clone_tour);
//		for (int v : clone_tour) {
//			in_sub_tour[v] = sub_i;
//		}
//	}
//}
void TSPSolver::getSubTours(DblVector const & sol, IntListPtrList& result){
	result.clear();

	IntListPtrVector subtour(nPoints(), IntListPtr());
	IntVector inSubTour(nPoints(), -1);
	for (int i(0); i < nPoints(); ++i){
		inSubTour[i] = i;
		subtour[i].reset(new IntList);
		subtour[i]->push_back(i);
	}

	for (int i(0); i < nPoints(); ++i){
		for (int j(i + 1); j < nPoints(); ++j){
			int const id(_variables[i][j]);
			if (inSubTour[i] != inSubTour[j] && sol[id] > 1 - 1e-10){
				//std::cout << "-------------------------" << std::endl;
				//std::cout << "inSubTour[" << i << "]=" << inSubTour[i] << std::endl;
				//std::cout << "inSubTour[" << j << "]=" << inSubTour[j] << std::endl;
				//std::cout << "sub_i : " << subtour[inSubTour[i]] << std::endl;
				//std::cout << "sub_j : " << subtour[inSubTour[j]] << std::endl;
				IntList clone(*subtour[inSubTour[j]]);
				subtour[inSubTour[i]]->splice(subtour[inSubTour[i]]->begin(), *subtour[inSubTour[j]]);

				for (auto const k : clone){
					//std::cout << "inSubTour[" << k << "]=" << inSubTour[i] << std::endl;
					inSubTour[k] = inSubTour[i];
				}
				//std::cout << "sub_i : " << subtour[inSubTour[i]] << std::endl;
				//std::cout << "sub_j : " << subtour[inSubTour[j]] << std::endl;
				//std::cout << "-------------------------" << std::endl;
				//if (subtour[inSubTour[i]]->empty()){
				//	std::cout << "ERROR sub of " << i << ", " << inSubTour[i] << " is empty" << std::endl;
				//	std::exit(0);
				//}
				//if (subtour[inSubTour[j]]->empty()){
				//	std::cout << "ERROR sub of " << j << ", " << inSubTour[j] << " is empty" << std::endl;
				//	std::exit(0);
				//}
			}
		}
	}
	for (auto & tour : subtour){
		if (!tour->empty()){
			result.push_back(tour);
		}
	}
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
						clind.push_back(_variables[i][j]);
					}
				}
			}
		}
	}
}

int TSPSolver::getMipSolution(XPRSprob prob){
	sol.resize(nVariables());
	XPRSgetmipsol(prob, sol.data(), NULL);
	return 0;
}

int TSPSolver::getSolution(XPRSprob prob){
	sol.resize(nVariables());
	int MIPINFEAS;
	XPRSgetintattrib(prob, XPRS_MIPINFEAS, &MIPINFEAS);
	if (MIPINFEAS == 0){
		XPRSgetlpsol(prob, sol.data(), NULL, NULL, NULL);
	}
	return MIPINFEAS;
}

int TSPSolver::getSubTours(){
	getSubTours(sol, subTours);
	return static_cast<int>(subTours.size());
}

bool TSPSolver::newSolution(){
	IntSetPtr sol_set(new IntSet);

	for (int i(0); i < sol.size(); ++i){
		if (std::fabs(sol[i] - 1) < 1e-10)
			sol_set->insert(i);
	}

	bool result = _knownSolution.insert(sol_set).second;
	return result;
}

void TSPSolver::add(XPRSprob prob, bool isCut){
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
}