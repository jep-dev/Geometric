#include "quaternion.tpp"

#include <iostream>
#include <iomanip>

int main(int argc, const char *argv[]) {
	using namespace std;


	using T = float;
	unsigned N = 2;
	T angle = M_PI / 2, scale = 1, scale2 = sqrt(2);
	string bord(N, ' '), sep(1, ' '), pad(N, ' '),
			tbord(N, ' '), tsep(sep.length(), '_'), tpad(N, ' ');
	bord[1] = /*tbord[1] =*/ '|';
	Quaternion<T> LHS[] = {{1}, {0,1}, {0,0,1}, {0,0,0,1}},
		RHS[] = {rotation(angle, 1, 0, 0) * scale,
				rotation(angle, 0, 1, 0) * scale, rotation(angle, 0, 0, 1) * scale};

	/*static constexpr const char *epsilon = "\u03B5";
	cout << "Near:\n";
	auto nl = std::numeric_limits<T>::epsilon();
	T lvals[] = {0, nl, 2*nl, 20*nl}, rvals[] = {0, nl, nl*10, nl*100};
	for(T lhs : lvals) {
		for(T rhs : rvals) {
			cout << lhs/nl << "*" << epsilon << ' ' << (near_zero(lhs, rhs) ? "is" : "isn't")
					<< " within " << rhs/nl << "*" << epsilon << " of 0\n  "
					"and " << (near(lhs, rhs, nl) ? "is" : "isn't")
					<< " within " << epsilon << " of " << rhs/nl << "*" << epsilon << endl;
		}
	}
	return 0;*/

	cout << ' ' << tpad << tbord << setfill('_');
	for(auto const& lhs : LHS)
		cout << setw(N) << lhs << tsep;
	long i = 0, iMax = sizeof(LHS) / sizeof(LHS[0]);
	cout << ' ' << setfill(' ') << endl;
	for(auto const& lhs : LHS) {
		cout << ' ' << setw(N) << lhs << bord;
		if(i == (iMax-1)) cout << setfill('_');
		for(auto const& rhs : LHS)
			cout << setw(N) << (lhs * rhs) << ((i == (iMax - 1)) ? tsep : sep);
		cout << '|' << endl;
		i++;
		cout << setfill(' ');
	}
	endl(cout);
	for(auto const& lhs : LHS) {
		for(auto const& rhs : RHS) {
			cout << '(' << (rhs*sqrtf(2)) << ")/\u221A2 * " << lhs
					<< " * (" << (*rhs*sqrtf(2)) << ")/\u221A2 = "
					<< setw(3) << right << (rhs * lhs * * rhs) << endl;
		}
	}
}
