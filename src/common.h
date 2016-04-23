#ifndef __COMMON_H_
#define __COMMON_H_

#include <algorithm>
#include <string>
#include <map>
#include <list>
#include <set>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <iomanip>
#include <memory>
#include <cstdlib>
#include  <xprs.h>

typedef std::pair<int, int> IntPair;
typedef std::pair<double, double> DblPair;
typedef std::vector<DblPair> Coordinates;

typedef std::map<IntPair, int> Variables;
typedef std::vector<int> IntVector;
typedef std::vector<IntVector> IntMatrix;

typedef std::vector<double> DblVector;
typedef std::vector<DblVector> DblMatrix;

typedef std::vector<char> CharVector;


typedef std::list<int> IntList;
typedef std::shared_ptr<IntList> IntListPtr;
typedef std::vector<IntListPtr> IntListPtrVector;
typedef std::list<IntListPtr> IntListPtrList;

typedef std::set<int> IntSet;
typedef std::shared_ptr<IntSet> IntSetPtr;

typedef std::pair<double, IntVector> SolutionHeur;

class IntSetPredicate{
public:
	bool operator()(IntSetPtr const & p1, IntSetPtr const & p2){
		return (*p1) < (*p2);
	}
};

inline std::ostream & operator<<(std::ostream & stream, IntList const & rhs){
	std::copy(rhs.begin(), rhs.end(), std::ostream_iterator<int>(stream, " "));
	return stream;
}
inline std::ostream & operator<<(std::ostream & stream, IntListPtr const & rhs){
	stream << *rhs;
	return stream;
}
inline std::ostream & operator<<(std::ostream & stream, IntSet const & rhs){
	std::copy(rhs.begin(), rhs.end(), std::ostream_iterator<int>(stream, " "));
	return stream;
}
inline std::ostream & operator<<(std::ostream & stream, IntVector & rhs){
	std::copy(rhs.begin(), rhs.end(), std::ostream_iterator<int>(stream, " "));
	return stream;
}
inline std::ostream & operator<<(std::ostream & stream, IntSetPtr const & rhs){
	stream << *rhs;
	return stream;
}

inline IntPair build_var(int i, int j){
	return std::make_pair(std::min(i, j), std::max(i, j));
}

#endif