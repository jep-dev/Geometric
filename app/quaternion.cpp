#include <iostream>
#include <iomanip>
#include <vector>
#include "math.tpp"

#include "quaternion.tpp"

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

		vector<Quaternion<T>> LHS = {{1}, {0,1}, {0,0,1}, {0,0,0,1}},
			RHS = {rotation(angle, 1, 0, 0),
					rotation(angle, 0, 1, 0),
					rotation(angle, 0, 0, 1)};

		vector<string> rows(LHS.size());
		unsigned i = 0;
		for(auto const& lhs : LHS) {
			if(i) {
				long j = i-1, N = RHS.size();
				if(j < N)
					rows[i] += to_string(RHS[j], 2);
			}
			else rows[i] += tpad;
			i++;
		}
		Streams::level_insert(rows, bord);
		i = 0;
		for(auto const& rhs : RHS) {
			//rows[i++] += to_string(rhs, 2);
			for(auto const& lhs : LHS) {
				rows[i++] += to_string(lhs * rhs, 2);
			}
			i = 0;
			Streams::level_insert(rows, sep);
		}
		for(auto & row : rows) {
			cout << row << endl;
			row = "";
		}
		endl(cout);
		i = 0;
		for(auto const& lhs : LHS) {
			if(i) {
				long j = i-1, N = RHS.size();
				if(j < N)
					rows[i] += to_string(RHS[j], 2);
			}
			else rows[i] += tpad;
			i++;
		}
		Streams::level_insert(rows, bord);
		i = 0;
		for(auto const& rhs : RHS) {
			//rows[i++] += to_string(rhs, 2);
			for(auto const& lhs : LHS) {
				rows[i++] += to_string(lhs / rhs, 2);
			}
			i = 0;
			Streams::level_insert(rows, sep);
		}
		for(auto & row : rows) {
			cout << row << endl;
			row = "";
		}
		endl(cout);

		for(auto const& rhs : RHS) {
			auto const& q = rhs;
			//auto q = scale2 * rhs;
			cout << "For u = " << to_string(q, 3) << ": u/u = " << to_string(q/q, 3) << "; "
					"u/|u| = " << to_string(q.normalize(), 3) << "; "
					"1/u = " << to_string(1/q, 3) << '.' << endl;
		}
		endl(cout);
		for(auto const& rhs : RHS) {
			cout << "For u = " << std::string(rhs) << ": ";
			bool met = false;
			for(auto const& lhs : LHS) {
				if(met) cout << "; ";
				met = true;
				cout << "u(" << 10*lhs << ")u* = " << (rhs*10*lhs**rhs);
			}
			cout << endl;
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
