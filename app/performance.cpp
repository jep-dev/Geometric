#include <iostream>
#include <iomanip>
#include <string>

#include <limits>
#include <utility>
#include <vector>

#ifdef DUAL2
#include "dual2.hpp"
#else
#include "dual.hpp"
#endif

#include "dual.tpp"
#include "quaternion.tpp"

#include "timing.hpp"
#include "utilities.hpp"


int main(int argc, const char *argv[]) {
	using namespace std;
	using namespace chrono;
	using namespace Timing;

	int M, N;
	stringstream ss;
	if(argc > 1) {
		ss << argv[1];
		ss >> M;
		if(ss.fail())
			M = 2048;
		ss.clear();
		if(argc > 2) {
			ss << argv[2];
			ss >> N;
			if(ss.fail())
				N = 2;
			ss.clear();
		} else N = 2;
	} else M = 2048, N = 2;



	// using T = float;
	using T = double;
	int n_digits = max(numDigits(M), numDigits(N));

	auto stopwatch = make_stopwatch(high_resolution_clock());
	auto flags = cout.flags();
	cout << "Quaternion<T>:\n" << delta(Quaternion<T>{1}, M, M*N, M*N*N, M*N*N*N) << endl;
	cout << "  Elapsed (total): " << stopwatch.update().count() << " seconds" << endl;
	stopwatch.advance();
	cout << "DualQuaternion<T>:\n" << delta(DualQuaternion<T>{1}, M, M*N, M*N*N, M*N*N*N) << endl;
	cout << "  Elapsed (total): " << stopwatch.update().count() << " seconds" << endl;
	cout.setf(flags);
}
