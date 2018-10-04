#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <functional>
#include <sstream>

#include "point.hpp"
#include "quaternion.tpp"
#include "dual.tpp"
#include "math.tpp"
#include "streams.tpp"
#include "geometric.hpp"
#include "pretty.tpp"

void test_ops(void) {
	using namespace std;
	using namespace Streams;
	typedef Point<float> Pt;
	vector<Pt> points = {1_x, 1_y, 1_z, -1_x, -1_y, -1_z, 2_x, 1_y + 1_z};
	auto N = points.size();
	vector<string> rows(N+1);

	auto pad = "  ";
	auto buff = [&] (void) {
		level(rows);
		rows[0] += pad;
		level(rows);
	};
	rows[0] += "Point";
	for(unsigned i = 0; i < N; i++)
		rows[i+1] += string(points[i]);
	buff();
	rows[0] += "Length";
	for(unsigned i = 0; i < N; i++)
		rows[i+1] += to_string(points[i].length(), 1);
	buff();
	rows[0] += " Normal";
	for(unsigned i = 0; i < N; i++)
		rows[i+1] += to_string(points[i].normalize(), 1);
	buff();
	rows[0] += " Perpendicular";
	for(unsigned i = 0; i < N; i++) {
		auto perp = perpendicular(points[i], true);
		rows[i+1] += to_string(perp.first, 1) + ", " + to_string(perp.second, 1);
	}
	buff();
	for(auto const& row : rows)
		cout << row << endl;
}
int main(int argc, const char *argv[]) {
	using namespace std;
	using namespace Streams;

	using T = float;
	using PT = Point<T>;
	using Q = Quaternion<T>;
	using DQ = DualQuaternion<T>;

	test_ops();
	cout << endl;

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

	transform(pts.begin(), pts.end(), qRows.begin(),
			[=](PT & pt) { return to_string(pt) + bord; });
	level(qRows);
	qRows.insert(qRows.begin(), string(minimax(qRows).second, ' '));
	transform(pts.begin(), pts.end(), dqRows.begin(),
			[=](PT & pt) { return to_string(pt) + bord; });
	level(dqRows);
	dqRows.insert(dqRows.begin(), string(minimax(dqRows).second, ' '));

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
}
