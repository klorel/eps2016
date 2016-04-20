#include "TSPFormulation.h"
#include "callback.h"

TSPFormulation::~TSPFormulation(){

}

TSPFormulation::TSPFormulation(std::string const & fileName):TSPInstance(fileName){
	buildVariable();
}


int TSPFormulation::nVariables()const{
	// (i, j) telle que i<j
	return nPoints()*(nPoints() - 1) / 2;
}
void TSPFormulation::buildVariable(){
	int npoints(nPoints());
	int ncols(0);
	_variables.assign(npoints, IntVector(npoints, -1));
	for (int i(0); i < npoints; ++i){
		for (int j(i + 1); j < npoints; ++j, ++ncols){
			_variables[i][j] = ncols;
			_variables[j][i] = ncols;
		}
	}
}
double TSPFormulation::computeCost(DblVector const & input){
	double result(0);
	for (int i(0); i < nPoints(); ++i){
		for (int j(i + 1); j < nPoints(); ++j){
			int const id(getVariable(i, j));
			if (input[id] > 1 - 1e-6)
				result += _distances[i][j];
		}
	}
	return result;
}


int TSPFormulation::getVariable(int i, int j){
	return _variables[i][j];
}

int TSPFormulation::getSubTours(DblVector const & sol, IntListPtrList& result){
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
			int const id(getVariable(i, j));
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
	int nSubtour(0);
	for (auto & tour : subtour){
		if (!tour->empty()){
			result.push_back(tour);
			++nSubtour;
		}
	}
	return nSubtour;
}


// input  : a tour
// output : a mip solution
void TSPFormulation::getSol(IntVector const   & input, DblVector & output) {
	output.assign(nVariables(), 0);
	for (int pos(1); pos < nPoints(); ++pos){
		int const current(input[pos]);
		int const prev(input[pos - 1]);

		output[getVariable(prev, current)] = 1;
	}
	output[getVariable(input.front(), input.back())] = 1;
}


// input  : a mip solution
// output : a tour
void TSPFormulation::getTour(DblVector const & input, IntVector & output) {
	std::vector<IntPair> intPairs(nPoints(), IntPair(nPoints(), nPoints()));
	for (int i(0); i < nPoints(); ++i){
		for (int j(i + 1); j < nPoints(); ++j){
			int const id(getVariable(i, j));
			if (input[id] > 1 - 1e-6){
				if (intPairs[i].first == nPoints())
					intPairs[i].first = j;
				else
					intPairs[i].second = j;

				if (intPairs[j].first == nPoints())
					intPairs[j].first = i;
				else
					intPairs[j].second = i;
			}
		}
	}
	output.assign(nPoints(), nPoints());
	output[0] = 0;
	for (int pos(0); pos + 1 < nPoints(); ++pos){
		int const curr(output[pos]);
		IntPair const & pair(intPairs[curr]);
		if (pos > 0){
			int const prev(output[pos - 1]);
			output[pos + 1] = pair.first == prev ? pair.second : pair.first;
		}
		else{
			output[pos + 1] = pair.first;
		}
	}
}
XPRSprob TSPFormulation::buildProblem(bool active_log){
	log() = active_log;
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
	DblVector obj(nVariables(), 0);
	IntVector ind(nVariables(), 0);
	for (int i(0); i < npoints; ++i){
		for (int j(i + 1); j < npoints; ++j, ++ncols){
			obj[ncols] = _distances[i][j];
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
			clind[ncoeffs] = getVariable(row, col);
		}
		for (int col(row + 1); col < npoints; ++col, ++ncoeffs){
			clind[ncoeffs] = getVariable(row, col);
		}
	}
	mstart.back() = ncoeffs;
	CharVector rowType(npoints, 'E');
	DblVector rhs(npoints, 2);
	XPRSaddrows(oprob, npoints, ncoeffs, rowType.data(), rhs.data(), NULL, mstart.data(), clind.data(), dmatval.data());
	if (status)errormsg("XPRSaddrows", __LINE__, status);
	return oprob;
}



void TSPFormulation::getBreakingCuts(IntListPtrList const & tours, DblVector & rhs, IntVector & mstart, IntVector & clind){
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
						clind.push_back(getVariable(i, j));
					}
				}
			}
		}
	}
}
