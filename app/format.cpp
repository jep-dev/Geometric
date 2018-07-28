#include <iostream>
#include <iomanip>
#include <array>
#include <vector>
#include <limits>
#include <functional>

#include "pretty.tpp"
#include "format.hpp"

#include "quaternion.hpp"
#include "quaternion.tpp"

#include "utilities.hpp"

int main(int argc, const char *argv[]) {
	using namespace std;


	Quaternion<float> LHS[] = {{1}, {3,4}, {8,15}, {7,24}};
	using OSS = ostringstream;

	static constexpr auto N = 4;
	vector<string> rows = {
		" q  =",
		" q* =",
		"-q  =",
		"|q| ="
	};
	for(auto const& quat : LHS) {
		vector<ostringstream> oss(N);
		vector<string> v(N);
			v[0] = (oss[0] << quat, oss[0]).str();
			v[1] = (oss[1] << *quat, oss[1]).str();
			v[2] = (oss[2] << -quat, oss[2]).str();
			v[3] = (oss[3] << quat, oss[3]).str();
		auto len = minimax(v).second;
		for(auto i = 0; i < N; i++) {
			OSS result;
			result << setw(len) << right << v[i];
			rows[i] += " " /*+ column[i] + " "*/ + result.str() + " |";
		}
	}

	for(auto const& row : rows)
		cout << row << endl;

	cout << Streams::join("Line 1\nLine 2, longer", "Line 1, longer\nLine 2", ' ', " = ") << endl;
}
