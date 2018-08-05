#ifdef DUAL2
#include "dual2.hpp"
#else
#include "dual.hpp"
#endif

#ifndef DUAL_IMPL
#define DUAL_IMPL "unknown"
#endif

#include "dual.tpp"
#include <iostream>
#include <iomanip>

int main(int argc, const char *argv[]) {
	using namespace std;
	cout << "Using " << DUAL_IMPL << " to implement dual quaternions!\n" << endl;

	typedef float T;
	DualQuaternion<T> LHS[] = {{1}, {0,1}, {0,0,1}, {0,0,0,1},
			{0,0,0,0,1}, {0,0,0,0,0,1}, {0,0,0,0,0,0,1}, {0,0,0,0,0,0,0,1}};
	for(auto const& lhs : LHS) {
		for(auto const& rhs : LHS) {
			cout << setw(3) << (lhs * rhs) << " ";
		}
		endl(cout);
	}
	cout << "\nSclerp:\n";
	{
		auto u = LHS[1], v = LHS[2] + 10 * LHS[6];
		//cout << "  For u = " << u << ", v = " << v << "...\n";

		// TODO proper ranging, this is sloppy
		for(T i = 0, di = .1, dj = di/10;
				i < 1 + di - std::numeric_limits<T>::epsilon(); i += di) {
			auto s = sclerp(u, v, i);
			// TODO this too
			for(auto j = 0; j < 8; j++)
				if(s[j]*s[j] < dj*dj) s[j] = 0;
			cout << "  sclerp(" << u << ", " << v << ", " << i << ") = "
					<< s << ";\n";
		}
	}
}
