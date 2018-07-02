#include <iostream>
#include <sstream>
#include <iomanip>
#include <array>
#include <vector>
#include <limits>
#include <math.h>
#include <functional>

#include "pretty.hpp"

#include "quaternion.hpp"
#include "quaternion.tpp"

template<class T>
std::pair<long, long> minimax(T && t, int = 0) {
/*template<template<class> class TT>
std::pair<long, long> minimax(TT<std::ostringstream> const& tt) {*/
	long m0 = std::numeric_limits<long>::max(), m1 = 0;
	for(auto &it : t) {
		long len = it.tellp();
		m0 = std::min(len, m0);
		m1 = std::max(len, m1);
	}
	if(m0 > m1) m0 = m1;
	return {m0, m1};
}
template<class T>
std::pair<long, long> minimax(T && t, long) {
	long m0 = std::numeric_limits<long>::max(), m1 = 0;
	for(auto const& it : t) {
		auto len = it.length();
		m0 = std::min(long(len), m0);
		m1 = std::max(long(len), m1);
	}
	if(m0 > m1) m0 = m1;
	return {m0, m1};
}

int main(int argc, const char *argv[]) {
	using namespace std;


	Quaternion<float> LHS[] = {{1}, {3,4}, {8,15}, {7,24}};
	using OSS = ostringstream;

	static constexpr auto N = 4;
	vector<string> rows = {
		" q  =",
		" q* =",
		"-q  =",
		"|q| ="
	};
	for(auto const& quat : LHS) {
		vector<ostringstream> oss(N);
			oss[0] << quat;
			oss[1] << *quat;
			oss[2] << -quat;
			oss[3] << quat.length();
		auto len = minimax(oss).second;
		for(auto i = 0; i < N; i++) {
			OSS result;
			result << setw(len) << right << oss[i].str();
			rows[i] += " " /*+ column[i] + " "*/ + result.str() + " |";
		}
	}

	for(auto const& row : rows)
		cout << row << endl;
}
