#include "math.hpp"
#include <iostream>
#include <iomanip>

int main(int argc, const char *argv[]) {
	using namespace std;

	typedef float T;
	static constexpr const char *epsilon = "\u03B5";
	cout << "Proximity:\n";
	T nl = .1;
	T lvals[] = {0, .5, 1, 1.5, 2, 2.5, 3, 3.5, 4}, rvals[] = {1, 2};
	auto N = 3;
	cout << right << setw(N) << "" << "  ";
	for(T const& rhs : lvals)
		cout << setw(N) << rhs << " ";
	cout << endl;
	for(T const& lhs : lvals) {
		cout << setw(N) << lhs << ": ";
		for(T const& rhs : lvals) {
			cout << setw(N) << near(lhs, rhs, nl) << " ";
		}
		/*cout << "  |  ";
		for(T const& rhs : lvals) {
		}*/
		cout << endl;
	}
	return 0;
}
