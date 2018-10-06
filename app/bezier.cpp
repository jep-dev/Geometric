#include "dual.tpp"
#include "point.hpp"
#include "bezier.hpp"
#include "math.tpp"
#include "streams.tpp"
#include <iostream>

int main(int argc, const char *argv[]) {
	using namespace std;
	using namespace Streams;
	typedef float T;
	typedef DualQuaternion<T> Dq;
	typedef Point<T> Pt;


	Pt p22[][2] = {
		{1_y, 1_x},
		{-1_x, -1_y},
	}, p33[][3] = {
		{1_y, 0_x, 1_x},
		{0_x, 0_x, 0_x},
		{-1_x, 0_x, -1_y},
	}, p44[][4] = {
		{1_y, 0_x, 0_x, 1_x},
		{0_x, 0_x, 0_x, 0_x},
		{0_x, 0_x, 0_x, 0_x},
		{-1_x, 0_x, 0_x, -1_y}
	};

	auto abbrev = [] (Dq const& d)
		-> string { return to_string(d-(1_e + 0_E), 2, false, true); };

	unsigned W = 6, H = 6;
	float divisors[] = {1,1,1};
	vector<string> rows[] = {vector<string>(H), vector<string>(H), vector<string>(H)};
	for(unsigned i = 0; i < W; i++) {
		float s = float(i)/(H-1);
		for(unsigned j = 0; j < H; j++) {
			float t = float(j)/(W-1);
			rows[0][j] += abbrev(linear(p22[0], p22[1], s/divisors[0], t/divisors[0]));
			rows[1][j] += abbrev(quadratic(p33[0], p33[1], p33[2], s/divisors[1], t/divisors[1]));
			rows[2][j] += abbrev(cubic(p44[0], p44[1], p44[2], p44[3], s/divisors[2], t/divisors[2]));
		}
		for(vector<string> & r : rows)
			level_insert(r, "  ");
	}
	for(auto const& r : rows) {
		for(auto const& row : r)
			cout << row << endl;
		cout << endl;
	}

}
