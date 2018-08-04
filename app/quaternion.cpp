#include "quaternion.tpp"

#include <iostream>
#include <iomanip>

int main(int argc, const char *argv[]) {
	using namespace std;

	using T = float;
	unsigned N = 2;
	T angle = M_PI / 2, scale = 2, scale2 = sqrt(2);
	string bord(N, ' '), sep(1, ' '), pad(N, ' '),
			tbord(N, ' '), tsep(sep.length(), '_'), tpad(N, ' ');
	tpad[1] = 'x';
	bord[1] = '|';

	Quaternion<T> LHS[] = {{1}, {0,1}, {0,0,1}, {0,0,0,1}},
		RHS[] = {scale * rotation(angle, 1, 0, 0),
				scale * rotation(angle, 0, 1, 0),
				scale * rotation(angle, 0, 0, 1)};

	auto fills = setfill(' '), fillu = setfill('_');
	auto setn = setw(N);

	cout << tpad << tbord << fillu;
	for(auto const& lhs : LHS)
		cout << setw(N) << lhs << tsep;
	long i = 0, iMax = sizeof(LHS) / sizeof(LHS[0]);
	cout << fills << endl;
	for(auto const& lhs : LHS) {
		cout << setw(N) << lhs << bord;
		if(i == (iMax-1)) cout << fillu;
		for(auto const& rhs : LHS)
			cout << setw(N) << (lhs * rhs) << ((i == (iMax - 1)) ? tsep : sep);
		cout << '|' << endl;
		i++;
		cout << fills;
	}
	endl(cout);
	for(auto const& lhs : LHS) {
		for(auto const& rhs : RHS) {
			cout << '(' << ((scale2/scale)*rhs) << ")*" << scale << "/\u221A2 * " << lhs
					<< " * (" << (*rhs*(scale2/scale)) << ")*" << scale << "/\u221A2 = "
					<< setw(numDigits(scale)+2) << right << (rhs * lhs * * rhs) << endl;
		}
	}
	for(auto const& rhs : RHS) {
		auto scaled = scale2 / scale * rhs;
		ostringstream oss1, oss2, oss3;
		oss1 << scaled, oss2 << scale << "/\u221A2";
		auto str1 = oss1.str(), str2 = oss2.str(), str3 = str1 + "*" + str2;
		cout << "For u = " << str1 << "...\n"
			"   u/u  = " << rhs/rhs << ";\n"
			"  u/|u| = " << rhs.normalize() << ";\n"
			"   1/u  = " << 1/rhs << "\n";
	}
	{
		auto const& lhs = LHS[0], rhs = LHS[1];
		cout << "For u = " << lhs << ", v = " << rhs << "...\n";
		for(float t = 0; t < 1 + .1 - std::numeric_limits<float>::epsilon(); t += .1) {
			cout << "  lerp(u, v, " << t << ") = " << lerp(lhs, rhs, t) << "; "
			"nlerp(u, v, " << t << ") = " << nlerp(lhs, rhs, t) << endl;
		}
	}
}
