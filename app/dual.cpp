#ifdef DUAL2
#include "dual2.hpp"
#else
#include "dual.hpp"
#endif

#include "dual.tpp"
#include <iostream>
#include <iomanip>

int main(int argc, const char *argv[]) {
	using namespace std;

	typedef float T;
	DualQuaternion<T> LHS[] = {{1}, {0,1}, {0,0,1}, {0,0,0,1},
			{0,0,0,0,1}, {0,0,0,0,0,1}, {0,0,0,0,0,0,1}, {0,0,0,0,0,0,0,1}};
	for(auto const& lhs : LHS) {
		for(auto const& rhs : LHS) {
			cout << setw(3) << (lhs * rhs) << " ";
		}
		endl(cout);
	}
	{
		auto lhs = LHS[1], rhs = LHS[2] + 10 * LHS[6];
		cout << "For u = " << lhs << ", v = " << rhs << "...\n";
		for(T i = 0, di = .1, dj = di/5;
				i < 1 + di - std::numeric_limits<T>::epsilon(); i += di) {
			auto s = sclerp(lhs, rhs, i);
			/*for(auto j = 0; j < 8; j++) {
				if(near_zero(s[j], dj)) s[j] = 0;
				else if(near(s[j], 1, dj)) s[j] = 1;
				else if(near(s[j], -1, dj)) s[j] = -1;
			}*/
			cout << "  s = sclerp(u, v, " << i << ") = " << s << ";\n";
		}
	}
}
