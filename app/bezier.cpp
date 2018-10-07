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


	float p22[][2] = {
		{1, 1},
		{0, 1}
	}, p33[][3] = {
		{1, 1, 1},
		{0, 0, 1},
		{0, 0, 1}
	}, p44[][4] = {
		{1, 1, 1, 1},
		{0, 0, 0, 1},
		{0, 0, 0, 1},
		{0, 0, 0, 1},
	};

	auto abbrev = [] (float const& f)
		-> string { return to_string(f, 0, true, false); };

	unsigned W = 10, H = 10;
	float divisors[] = {1,1,1};
	vector<string> rows[] = {vector<string>(H), vector<string>(H), vector<string>(H)};
	for(unsigned i = 0; i < W; i++) {
		float s = float(i)/(H-1);
		for(unsigned j = 0; j < H; j++) {
			float t = float(j)/(W-1);
			rows[0][j] += abbrev(9*linear(p22[0], p22[1], s/divisors[0], t/divisors[0]));
			//rows[0][j] += abbrev(linear(p22[0], p22[1], s/divisors[0], t/divisors[0]));
			rows[1][j] += abbrev(9*quadratic(p33[0], p33[1], p33[2], s/divisors[1], t/divisors[1]));
			rows[2][j] += abbrev(9*cubic(p44[0], p44[1], p44[2], p44[3], s/divisors[2], t/divisors[2]));
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
