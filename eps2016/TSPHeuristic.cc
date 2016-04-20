#include "TSPHeuristic.h"
#include "TSPAlgo.h"


TSPHeuristic::TSPHeuristic(TSPSolver & solver) :_tsp(solver._tsp), _solver(&solver){

}

TSPHeuristic::~TSPHeuristic(){

}

#define __DEBUG_HEURISITC__ 0


bool TSPHeuristic::localSearch(IntVector const & input, SolutionHeur & output, bool log){
	double init_cost(_tsp->computeCost(input));
	output.second = input;
	output.first = init_cost;
	size_t const n(input.size());
	size_t current(0);
	if (log){
		std::cout << "start with ";
		std::cout << std::setw(20) << std::setprecision(10) << output.first << std::endl;
	}
	for (size_t ite(0); ite < n; ++ite, ++current){
		current = (current%n);
#if __DEBUG_HEURISITC__
		std::cout << "------------------" << std::endl;
		std::cout << "current is " << current << std::endl;
#endif
		size_t const i1(current);
		size_t const i2((current + 1) % n);
		size_t const i3((current + 2) % n);
		size_t const i4((current + 3) % n);
		// v1-v2-v3-v4 --> v1-v3-v2-v4
		size_t const v1(output.second[i1]);
		size_t const v2(output.second[i2]);
		size_t const v3(output.second[i3]);
		size_t const v4(output.second[i4]);
#if __DEBUG_HEURISITC__
		std::cout << n << std::endl;

		std::cout << i1 << std::endl;
		std::cout << i2 << std::endl;
		std::cout << i3 << std::endl;
		std::cout << i4 << std::endl;

		std::cout << v1 << std::endl;
		std::cout << v2 << std::endl;
		std::cout << v3 << std::endl;
		std::cout << v4 << std::endl;
#endif
		// better if v1-v2+v3-v4 > v1-v3+v2-v4
		double const v1_v2(_tsp->_distances[v1][v2]);
		double const v3_v4(_tsp->_distances[v3][v4]);
		double const v1_v3(_tsp->_distances[v1][v3]);
		double const v2_v4(_tsp->_distances[v2][v4]);
		//
		double const delta(v1_v3 + v2_v4 - v1_v2 - v3_v4);
		//
		bool improvement(delta < -1e-6);
		if (improvement){
			ite = 0;
			std::swap(output.second[i2], output.second[i3]);
			output.first += delta;
			if (log){
				std::cout << "swap";
				std::cout << std::setw(6) << v2;
				std::cout << std::setw(6) << v3;
				std::cout << std::setw(20) << std::setprecision(10) << output.first;
				std::cout << std::endl;
			}
		}
		else{
		}
	}
	return init_cost != output.first;
}

// chose a point at random and then its k neighbor in the solution, 
size_t TSPHeuristic::shake(size_t k, IntVector const & input, IntVector & subproblem, DblVector & lb, DblVector & ub){
	int const n(_tsp->nPoints());
	int const p(_tsp->nVariables());

	DblVector sol;
	_tsp->getSol(input, sol);

	// all fixed
	lb = sol;
	ub = sol;

	int random_index = std::rand() % n;
	size_t const current(input[random_index]);
	//std::cout << "shaking index " << random_index << ", " << current << std::endl;
	subproblem.clear();
	subproblem.push_back((int)current);
	for (size_t i(0); i < k; ++i){
		size_t const before(input[(random_index - i - 1) % n]);
		size_t const after(input[(random_index + i + 1) % n]);
		subproblem.push_back((int)before);
		subproblem.push_back((int)after);
	}
	//std::cout << "creating a subproblem of size " << subproblem.size() << std::endl;
	for (size_t i(0); i < subproblem.size(); ++i){
		int const vi(subproblem[i]);
		for (size_t j(i + 1); j < subproblem.size(); ++j){
			int const vj(subproblem[j]);
			int const id(_tsp->getVariable(vi, vj));
			lb[id] = 0;
			ub[id] = 1;
		}
	}
	return subproblem.size();
}
void TSPHeuristic::vns(size_t kMax, IntVector const & input, SolutionHeur & output){
	int p(_tsp->nVariables());

	DblVector lb(p, 0);
	DblVector ub(p, 1);
	bool stop(false);
	size_t ite(0);

	SolutionHeur current;
	current.first = _tsp->computeCost(input);
	current.second = input;
	SolutionHeur best = current;

	TSPAlgo tspAlgo;
	std::cout << std::setw(6) << ite;
	std::cout << std::setw(6) << 0;
	std::cout << std::setw(6) << 0;
	std::cout << std::setw(25) << std::setprecision(15) << current.first;
	std::cout << std::endl;
	while (!stop){
		for (size_t k(1); k < kMax + 1; ++k){
			++ite;
			//std::cout << current.second << std::endl;
			IntVector subproblem;
			// create a neighborhood
			size_t subsize = shake(k, current.second, subproblem, lb, ub);
			// solve 
			tspAlgo.run_iterative(*_tsp, current.second, current.first, lb, ub);
			// accept or not the solution
			if (best.first > current.first + 1e-6){
				localSearch(current.second, best, false);
				//best = current;
				std::cout << std::setw(6) << ite;
				std::cout << std::setw(6) << k;
				std::cout << std::setw(6) << subsize;
				std::cout << std::setw(25) << std::setprecision(15) << best.first;
				std::cout << std::endl;
				k = 0;
			}
			else{
				current = best;
			}
		}
	}
}



#undef __DEBUG_HEURISITC__