#include <chrono>
#include <iostream>
#include <iomanip>
#include <string>

#include <limits>
#include <utility>
#include <vector>

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
	int n_digits = max(ceil(log10(M+1)), ceil(log10(N+1)));

	auto flags = cout.flags();
	cout << "Quaternion<T>:\n" << delta(Quaternion<T>{1}, 100, 1000, 10000) << endl;
	cout << "DualQuaternion<T>:\n" << delta(DualQuaternion<T>{1}, 100, 1000, 10000) << endl;
}
