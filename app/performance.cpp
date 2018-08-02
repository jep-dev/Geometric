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

	// using T = float;
	using T = double;
	unsigned M = 10000, N = M * 100;
	int n_digits = max(numDigits(M), numDigits(N));

	auto flags = cout.flags();
	cout << "Quaternion<T>:\n" << delta(Quaternion<T>{1}, 100, 1000, 10000) << endl;
	cout << "DualQuaternion<T>:\n" << delta(DualQuaternion<T>{1}, 100, 1000, 10000) << endl;
}
