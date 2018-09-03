#include "quaternion.tpp"

#include <iostream>
#include <iomanip>
#include <vector>

int main(int argc, const char *argv[]) {
	using namespace std;

	using T = float;
	unsigned N = 2;
	if(argc == 1) {
		T angle = M_PI / 2, scale1 = 2, scale2 = sqrt(2);
		string bord(N, ' '), sep(1, ' '), pad(N, ' '),
				tbord(N, ' '), tsep(sep.length(), '_'), tpad(N, ' ');
		tpad[1] = 'x';
		bord[1] = '|';

		Quaternion<T> LHS[] = {{1}, {0,1}, {0,0,1}, {0,0,0,1}},
			RHS[] = {rotation(angle, 1, 0, 0),
					rotation(angle, 0, 1, 0),
					rotation(angle, 0, 0, 1)};

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
				cout << '(' << std::string(rhs) << ")*" << lhs
						<< "*(" << std::string(*rhs) << ") = "
						<< (rhs * lhs * * rhs) << endl;
			}
		}
		for(auto const& rhs : RHS) {
			auto scaled = scale2 / scale1 * rhs;
			ostringstream oss1, oss2, oss3;
			oss1 << std::string(scaled), oss2 << scale1 << "/\u221A2";
			auto str1 = oss1.str(), str2 = oss2.str(), str3 = str1 + "*" + str2;
			ostringstream oss4, oss5, oss6;
			print_fixed(oss4, rhs/rhs, 3) << ';';
			print_fixed(oss5, rhs.normalize(), 3) << ';';
			print_fixed(oss6, 1/rhs, 3);
			cout << "For u = " << str1 << "...\n"
				"   u/u  = " << oss4.str() << "\n"
				"  u/|u| = " << oss5.str() << "\n"
				"   1/u  = " << oss6.str() << "\n";
		}
		return 0;
	}


	enum Erp {
		e_lerp = 0, e_nlerp, e_slerp, e_nslerp, e_erps
	};
	bool enabled[e_erps] = {0};
	vector<string> keywords[e_erps] = {
			{"lerp", "linear", "all"},
			{"nlerp", "normalized-linear", "normalized", "all"},
			{"slerp", "spherical-linear", "spherical", "all"},
			{"nslerp", "normalized-spherical-linear", "normalized-spherical", "all"}
	};
	/*if(argc == 1) enabled[e_slerp] = true;
	else {*/
		bool hit = false;
		for(int i = 1; i < argc; i++) {
			for(int j = 0; j < e_erps; j++) {
				for(auto const& kw : keywords[j]) {
					if(argv[i] == kw) {
						enabled[j] = hit = true;
						break;
					}
				}
			}
		}
		if(!hit) enabled[e_slerp] = true;
	//}
	auto prec = 2, wprec = prec + 3;
	cout << fixed << setprecision(prec);
	auto printq = [=] (ostream& os, Quaternion<T> const& q) -> ostream& {
		auto f = os.flags();
		for(auto i : {'1', 'i', 'j', 'k'}) {
			if(i != 1) os << ' ';
			os << setw(wprec) << q[i];
		}
		os.flags(f);
		return os;
	};
	T ut = M_PI/4, vt = -M_PI/4;
	int scale = 2;
	auto u = scale*rotation(ut, 1, 0, 0), v = scale*rotation(vt, 1, 0, 0);
	auto lim = std::numeric_limits<T>::epsilon(), dt = T(.25);
	printq(cout << "For u = " << scale
			<< "exp(" << setw(6) << ut/2/M_PI << "\u03c0i) = ", u) << ",\n";
	printq(cout << "    v = " << scale
			<< "exp(" << setw(6) << vt/2/M_PI << "\u03c0i) = ", v) << "...\n";
	auto print_ss = [] (ostream &dest, ostringstream &l, ostringstream &r) -> ostream& {
		dest << setw(20) << l.str() << " = " << r.str();
		l.str(""), r.str("");
		return dest;
	};
	auto stringq = [=] (Quaternion<T> const& q) -> string {
		ostringstream oss;
		oss << setprecision(prec) << right
			<< setw(wprec) << q.w << ' ' << setw(wprec) << q.x << ' '
			<< setw(wprec) << q.y << ' ' << setw(wprec) << q.z;
		return oss.str();
	};

	ostringstream loss, ross;
	for(T t = 0; t < 1 + dt - lim; t += dt) {
		string strings[e_erps] = {stringq(lerp(u,v,t)), stringq(nlerp(u,v,t)),
				stringq(slerp(u,v,t)), stringq(slerp(u,v,t,true))};
		auto i = 0;
		for(auto j = 0; j < e_erps; j++) {
			if(enabled[j]) {
				if(!i) cout << "t = " << setw(4) << t << ": ";
				else if(i & 1) cout << "; ";
				else cout << setw(10) << "";
				cout << setw(6) << right << keywords[j][0] << " = "
						<< setw(20) << strings[j];
				if(i & 1) cout << "; " << endl;
				i++;
			}
		}
		if((i & 1)) cout << "; " << endl;
	}
}
