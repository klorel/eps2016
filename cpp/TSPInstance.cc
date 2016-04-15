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
}

TSPInstance::~TSPInstance(){

}

void TSPInstance::read(std::string const & fileName){
	std::ifstream file(fileName.c_str());
	if (file.is_open()){
		std::string line;
		while(std::getline(file, line)){
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

void TSPInstance::buildDistance(){
	_distances.assign(nPoints(), DblVector(nPoints(), -1));
	for (size_t i(0); i < nPoints(); ++i){
		for (size_t j(0); j < nPoints(); ++j){
			_distances[i][j] = std::sqrt(std::pow(_coordinates[i].first - _coordinates[j].first, 2.0) + std::pow(_coordinates[i].second - _coordinates[j].second, 2.0));
		}
	}
}