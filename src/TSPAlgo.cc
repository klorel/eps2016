#include "TSPAlgo.h"
#include "TSPSolver.h"
#include "callback.h"
#include <iomanip>

TSPAlgo::TSPAlgo(){
	_user_heuristic = false;
}


TSPAlgo::~TSPAlgo(){

}

bool TSPAlgo::user_heuristic()const{
	return _user_heuristic;
}
bool &TSPAlgo::user_heuristic(){
	return _user_heuristic;
}

void TSPAlgo::run_iterative(TSPFormulation & tsp, IntVector & output, double & obj){
	DblVector lb;
	DblVector ub;
	run_iterative(tsp, output, obj, lb, ub);
}
void TSPAlgo::run_iterative(TSPFormulation & tsp, IntVector & output, double & obj, DblVector const & lb, DblVector const & ub){
	TSPSolver solver(tsp);

	XPRSprob oprob = solver.getTSPFormulation().buildProblemMIP(false);
	bool log = true;
	XPRSsetintcontrol(oprob, XPRS_THREADS, 1);
	//XPRSsetintcontrol(oprob, XPRS_ROOTPRESOLVE, 1);
	//XPRSsetintcontrol(oprob, XPRS_HEURSTRATEGY, 1);
	XPRSsetintcontrol(oprob, XPRS_HEURSTRATEGY, XPRS_HEURSTRATEGY_NONE);
	XPRSsetintcontrol(oprob, XPRS_ROOTPRESOLVE, 0);
	XPRSsetintcontrol(oprob, XPRS_CUTSTRATEGY, 0);
	if (_user_heuristic){
		XPRSsetcbintsol(oprob, cbintsol, &solver);
	}

	//XPRSsetintcontrol(oprob, XPRS_ROOTPRESOLVE, 0);
	//XPRSsetintcontrol(oprob, XPRS_HEURSTRATEGY, 0);
	if (!lb.empty() || !ub.empty()){
		log = false;
		//solver.log() = true;
		IntVector indexes(tsp.nVariables());
		CharVector b(tsp.nVariables(), 'B');
		CharVector u(tsp.nVariables(), 'U');
		for (int i(0); i < (int)indexes.size(); ++i){
			indexes[i] = i;
		}
		XPRSchgbounds(oprob, (int)indexes.size(), indexes.data(), b.data(), lb.data());
		XPRSchgbounds(oprob, (int)indexes.size(), indexes.data(), u.data(), ub.data());
	}
	XPRSsetintcontrol(oprob, XPRS_THREADS, 1);

	run_iterative(tsp, solver, oprob, log);

	DblVector sol;
	tsp.getMipSolution(oprob, sol);

	XPRSgetdblattrib(oprob, XPRS_MIPOBJVAL, &obj);
	tsp.getTour(sol, output);
	XPRSdestroyprob(oprob);

	//std::cout << "tour is " << output << std::endl;
	//std::cout << "value is " << obj << ", " << tsp.computeCost(output) << std::endl;
}
void TSPAlgo::run_iterative(TSPFormulation& tsp, TSPSolver & solver, XPRSprob & oprob, bool log){
	double obj;
	int nSubTour;
	int mipunfeas;
	DblVector sol;
	IntListPtrList subtours;
	size_t nCuts(0);
	int nodes;
	if (log){
		std::cout << std::setw(10) << "Obj value";
		std::cout << std::setw(10) << "Subtours";
		std::cout << std::setw(10) << "Nodes";
		std::cout << std::endl;
	}
	do{
		XPRSmipoptimize(oprob, "");
		mipunfeas = tsp.getMipSolution(oprob, sol);
		if (mipunfeas == 0 && solver.newSolution(sol, solver._knownSolution)){
			nSubTour = tsp.getSubTours(sol, subtours);
			if (nSubTour > 1){
				nCuts += solver.add(subtours, oprob, false);
			}
		}
		else{
			std::cout << "ERROR" << std::endl;
			std::exit(0);
		}
		XPRSgetdblattrib(oprob, XPRS_MIPOBJVAL, &obj);
		XPRSgetintattrib(oprob, XPRS_NODES, &nodes);
		if (log){
			std::cout << std::setw(10) << obj;
			std::cout << std::setw(10) << nSubTour;
			std::cout << std::setw(10) << nodes;
			std::cout << std::endl;
		}
	} while (nSubTour > 1);
	if (log)
		std::cout << "Nb of cuts generated " << std::setw(10) << nCuts << std::endl;
}