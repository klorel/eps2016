#include "TSPInstance.h"

int TSPInstance::nPoints()const{
	return static_cast<int>(_coordinates.size());
}

int TSPInstance::nVariables()const{
	return nPoints()*(nPoints() - 1) / 2;
}

TSPInstance::TSPInstance(std::string const & fileName){
	read(fileName);
	buildDistance();
	buildVariable();
}

TSPInstance::~TSPInstance(){

}

void TSPInstance::read(std::string const & fileName){
	std::ifstream file(fileName.c_str());
	if (file.is_open()){
		std::string line;
		while (std::getline(file, line)){
			std::stringstream buffer(line);
			int id;
			double x;
			double y;
			buffer >> id;
			buffer >> x;
			buffer >> y;
			_coordinates.push_back(std::make_pair(x, y));
		}
		file.close();
	}
	else{
		std::cout << "unable to open : " << fileName << std::endl;
	}
}
void TSPInstance::buildVariable(){
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
void TSPInstance::buildDistance(){
	_distances.assign(nPoints(), DblVector(nPoints(), -1));
	for (size_t i(0); i < nPoints(); ++i){
		for (size_t j(0); j < nPoints(); ++j){
			_distances[i][j] = std::sqrt(std::pow(_coordinates[i].first - _coordinates[j].first, 2.0) + std::pow(_coordinates[i].second - _coordinates[j].second, 2.0));
		}
	}
}


double TSPInstance::computeCost(IntVector const & input){
	double result(0);
	for (size_t i(0); i < input.size() - 1; ++i){
		size_t const vi(input[i]);
		size_t const vj(input[i + 1]);
		result += _distances[vi][vj];
	}
	result += _distances[input.back()][input.front()];
	return result;
}


int TSPInstance::getVariable(int i, int j){
	return _variables[i][j];

}
bool TSPInstance::isInteger(DblVector const & sol)const{
	for (auto v : sol){
		if (std::fabs(v) > 1e-10 && std::fabs(1 - v) > 1e-10)
			return false;
	}
	return true;
}
int TSPInstance::getSubTours(DblVector const & sol, IntListPtrList& result){
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

int TSPInstance::getMipSolution(XPRSprob prob, DblVector & sol){
	sol.resize(nVariables());
	XPRSgetmipsol(prob, sol.data(), NULL);
	return 0;
}

int TSPInstance::getSolution(XPRSprob prob, DblVector & sol){
	sol.resize(nVariables());
	int MIPINFEAS;
	XPRSgetintattrib(prob, XPRS_MIPINFEAS, &MIPINFEAS);
	if (MIPINFEAS == 0){
		XPRSgetlpsol(prob, sol.data(), NULL, NULL, NULL);
	}
	return MIPINFEAS;
}



// input  : a tour
// output : a mip solution
void TSPInstance::getSol(IntVector const   & input, DblVector & output) {
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
void TSPInstance::getTour(DblVector const & input, IntVector & output) {
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