#include "TSPAlgo.h"
#include "TSPSolver.h"
#include "callback.h"
#include <iomanip>

TSPAlgo::TSPAlgo(){

}


TSPAlgo::~TSPAlgo(){

}

void TSPAlgo::run_callback(TSPInstance & tsp){

	TSPSolver solver(tsp);
	solver.log() = true;
	std::cout << "TSP with " << tsp.nPoints() << " points " << std::endl;

	XPRSprob oprob = solver.buildProblem();
	XPRSsetintcontrol(oprob, XPRS_THREADS, 1);
	//XPRSsetintcontrol(oprob, XPRS_CUTSTRATEGY, 0);
	//XPRSsetintcontrol(oprob, XPRS_HEURSTRATEGY, 0);
	//XPRSsetdblcontrol(oprob, XPRS_MIPABSSTOP, 0);
	//XPRSsetdblcontrol(oprob, XPRS_MIPRELSTOP, 0);
	//XPRSsetdblcontrol(oprob, XPRS_MIPTOL, 0);
	XPRSsetcboptnode(oprob, cboptnode, &solver);
	XPRSsetcbpreintsol(oprob, cbpreintsol, &solver);
	
	XPRSmipoptimize(oprob, "");
	double obj;
	XPRSgetdblattrib(oprob, XPRS_MIPOBJVAL, &obj);
	std::cout << std::setw(10) << obj;
	std::cout << std::setw(10) << solver._nCuts;
	solver.getMipSolution(oprob);
	int nsubtour = solver.getSubTours();
	std::cout << std::setw(10) << nsubtour;
	std::cout << std::endl;
}

void TSPAlgo::run_iterative(TSPInstance & tsp){
	TSPSolver solver(tsp);
	solver.log() = false;
	std::cout << "TSP with " << tsp.nPoints() << " points " << std::endl;

	XPRSprob oprob = solver.buildProblem();
	XPRSsetintcontrol(oprob, XPRS_THREADS, 1);
	//XPRSsetintcontrol(oprob, XPRS_CUTSTRATEGY, 0);
	//XPRSsetintcontrol(oprob, XPRS_HEURSTRATEGY, 0);
	//XPRSsetdblcontrol(oprob, XPRS_MIPABSSTOP, 0);
	//XPRSsetdblcontrol(oprob, XPRS_MIPRELSTOP, 0);
	//XPRSsetdblcontrol(oprob, XPRS_MIPTOL, 0);


	double obj;
	int nSubTour;
	int mipunfeas;
	do{
		XPRSmipoptimize(oprob, "");
		mipunfeas = solver.getMipSolution(oprob);
		if (mipunfeas == 0 && solver.newSolution()){
			nSubTour = solver.getSubTours();
			if (nSubTour > 1){
				solver.add(oprob, false);
			}
		}
		else{
			std::cout << "ERROR" << std::endl;
			std::exit(0);
		}
		XPRSgetdblattrib(oprob, XPRS_MIPOBJVAL, &obj);
		std::cout << std::setw(10) << obj;
		std::cout << std::setw(10) << nSubTour;
		std::cout << std::endl;
	} while (nSubTour > 1);
	XPRSdestroyprob(oprob);
}