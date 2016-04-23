#include "TSPInstance.h"

int TSPInstance::nPoints()const{
	return static_cast<int>(_coordinates.size());
}

TSPInstance::TSPInstance(){

}
TSPInstance::TSPInstance(std::string const & fileName){
	read(fileName);
	buildDistance();
}

TSPInstance::~TSPInstance(){

}

bool & TSPInstance::log(){
	return _log;
}
bool TSPInstance::log()const{
	return _log;
}
void TSPInstance::read(std::string const & fileName){
	std::ifstream file(fileName.c_str());
	if (file.is_open()){
		std::string line;
		while (std::getline(file, line) && !line.empty()){
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
		std::cout << "TSP with " << nPoints() << " points " << std::endl;
	}
	else{
		std::cout << "unable to open : " << fileName << std::endl;
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

//double TSPInstance::computeCost(DblVector const & input){
//	return 0;
//}
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

bool TSPInstance::isInteger(DblVector const & sol)const{
	for (auto v : sol){
		if (std::fabs(v) > 1e-10 && std::fabs(1 - v) > 1e-10)
			return false;
	}
	return true;
}
int TSPInstance::getMipSolution(XPRSprob prob, DblVector & sol){
	int n;
	XPRSgetintattrib(prob, XPRS_COLS, &n);
	sol.resize(n);
	XPRSgetmipsol(prob, sol.data(), NULL);
	return 0;
}

int TSPInstance::getSolution(XPRSprob prob, DblVector & sol){
	int n;
	XPRSgetintattrib(prob, XPRS_ORIGINALCOLS, &n);
	sol.resize(n);

	int MIPINFEAS;
	XPRSgetintattrib(prob, XPRS_MIPINFEAS, &MIPINFEAS);
	if (MIPINFEAS <= 0){
		XPRSgetlpsol(prob, sol.data(), NULL, NULL, NULL);
	}
	return MIPINFEAS;
}

void TSPInstance::randomInit(IntVector & output){
	int const n(nPoints());
	output.resize(n);
	IntVector available(n, 0);
	for (int i(0); i < n; ++i)
		available[i] = i;
	//std::cout << n << std::endl;
	//std::cout << available << std::endl;
	//std::exit(0);
	for (int i(0); i < n; ++i){
		int const index(std::rand() % (n - i));
		int k = available[index];
		std::swap(available[index], available[n - i - 1]);
		output[i] = k;
	}
}