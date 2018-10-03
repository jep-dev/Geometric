#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <functional>

#include "point.hpp"
#include "quaternion.tpp"
#include "dual.tpp"
#include "math.tpp"
#include "streams.tpp"
#include "geometric.hpp"
#include "pretty.tpp"

void test_perp(unsigned prec = 0) {
	using namespace std;
	using namespace Streams;
	typedef Point<float> Pt;
	Pt ws[] = {{2,0,0}, {0,2,0}, {0,0,2}};
	for(Pt const& pt : ws) {
		for(Pt w : {pt, -pt}) {
			auto perp = perpendicular(w);
			auto const& u = perp.first, v = perp.second;
			auto uv = cross(u, v), vw = cross(v, w), wu = cross(w, u);
			cout << "    w = " << to_string(w, prec) << "; "
					"    u = " << to_string(u, prec) << "; "
					"    v = " << to_string(v, prec) << ";\n"
					"u x v = " << to_string(uv, prec) << ";\n"
					"v x w = " << to_string(vw, prec) << ";\n"
					"w x u = " << to_string(wu, prec) << ";\n";
		}
		cout << endl;
	}
}

int main(int argc, const char *argv[]) {
	using namespace std;
	using namespace Streams;
	test_perp();
	return 0;

	using T = float;
	using PT = Point<T>;
	using Q = Quaternion<T>;
	using DQ = DualQuaternion<T>;

	vector<Q> qs = {
		rotation(float(M_PI/2), 1, 0, 0),
		rotation(float(M_PI/2), 0, 1, 0),
		rotation(float(M_PI/2), 0, 0, 1),
	};
	vector<DQ> dqs = {
		1_j + 1_I, 1_j + 1_J, 1_j + 1_K
	};
	vector<PT> pts = {
		{0,0,0}, {1,0,0}, {0,1,0}, {0,0,1}
	};

	string bord = " | ", delim = "  ";
	vector<string> qRows(pts.size()+1, ""), dqRows(pts.size()+1,"");
	unsigned qPrec = 3, dqPrec = qPrec;

	// Using "to_string<Q>" causes very strange issues
	transform(pts.begin(), pts.end(), qRows.begin(),
			[=](PT & pt) { return to_string(pt) + bord; });
	level(qRows);
	qRows.insert(qRows.begin(), string(minimax(qRows).second, ' '));
	transform(pts.begin(), pts.end(), dqRows.begin(),
			[=](PT & pt) { return to_string(pt) + bord; });
	level(dqRows);
	dqRows.insert(dqRows.begin(), string(minimax(dqRows).second, ' '));
	//copy(qRows.begin(), qRows.begin(), dqRows.begin());

	for(long i = 0, M = qs.size(); i < M; i++) {
		for(long j = 0, N = pts.size(); j < N; j++) {
			auto &row = qRows[j+1];
			row += delim + to_string(pts[j]^qs[i]);
		}
		auto dl = minimax(qRows).second - qRows[0].length();
		qRows[0] += center(to_string(qs[i], qPrec), dl) + delim;
		level(qRows);
	}
	for(auto const& row : qRows)
		cout << row << endl;

	for(long i = 0, M = dqs.size(); i < M; i++) {
		for(long j = 0, N = pts.size(); j < N; j++) {
			auto &row = dqRows[j+1];
			row += delim + to_string(pts[j]^dqs[i]);
		}
		auto dl = minimax(dqRows).second - dqRows[0].length();
		dqRows[0] += center(to_string(dqs[i], dqPrec), dl) + delim;
		level(dqRows);
	}
	for(auto const& row : dqRows)
		cout << row << endl;


	/*bool met = false;
	for(auto const& col : qCols) {
		if(met) cout << delim;
		met = true;
		cout << col;
	}
	cout << endl;
	met = false;
	for(auto const& col : qCols) {
		if(met) cout << delim;
		met = true;
		cout << col;
	}
	cout << endl;*/

	/*auto fmt_sandwich = [] (ostream &os, auto const& sub, auto const& sup,
			auto const& rhs, unsigned prec = 0) -> ostream& {
		return os << to_string(sub, prec) << "^(" << to_string(sup, prec) << ")"
				" = " << to_string(rhs, prec);
	};
	for(PT pt : {{}, {1}, 1_e + 1_J, 1_e + 1_K}) {
		cout << "Point: " << pt << endl;
		for(Q t : {
			rotation(float(M_PI/2), 1, 0, 0),
			rotation(float(M_PI/2), 0, 1, 0),
			rotation(float(M_PI/2), 0, 0, 1),
		}) cout << "  " << t << "^(" << to_string(t,3) << ") "
				"= " << to_string(pt ^ t, 1) << endl;
		for(DQ t : {1_e + 0.5_I, 1_e + 0.5_J, 1_e + 0.5_K})
			cout << "  " << t << "^(" << to_string(t,3) << ") "
				"= " << to_string(pt ^ t, 1) << endl;
	}*/
}
