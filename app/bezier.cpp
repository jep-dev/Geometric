#include "dual.tpp"
#include "point.hpp"
#include "bezier.hpp"
#include "math.tpp"
#include "streams.tpp"
#include <iostream>

float linearTransfer(float f){ return f; };
// log(1)=0, log(e)=1
float logTransfer(float f) { return log(1 + M_E * f); };

int main(int argc, const char *argv[]) {
	using namespace std;
	using namespace Streams;
	typedef float T;
	typedef DualQuaternion<T> Dq;
	typedef Point<T> Pt;

	/*static constexpr unsigned rank = 3;
	unsigned widths[rank] = {4, 4, 4};
	auto fn = [] (unsigned (&t) [rank])
			-> T { return 100*t[0] + 10*t[1] + t[2]; };
	T (*fnPtr)(unsigned (&indices) [rank]) = fn;
	auto poly = polyLoop<T>(widths, fnPtr);
	//auto poly = polyTest<T>(normalize<T>); */

	float p22[][2] = {
		{1, 0},
		{0, 1}
	}, p33[][3] = {
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1}
	}, p44[][4] = {
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1},
	}, p55[][5] = {
		{1, 0, 0, 0, 0},
		{0, 1, 0, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 0, 1, 0},
		{0, 0, 0, 0, 1},
	};

	/*auto abbrev = [] (float const& f)
		-> string { return to_string(f, 0, true, false); };*/
	auto abbrev = [] (float fVal, float fMin = 0, float fMax = 1,
			float (*transfer)(float) = linearTransfer) -> std::string {
		//static constexpr const char *szHex = "0123456789ABCDEF";
		static constexpr const char *szHex[] = {"0123456789ABCDEF", "0123456789abcdef"},
			*szDec = "0123456789";
		auto ft = fminf(1, fmaxf(0, transfer((fVal - fMin) / (fMax - fMin))));
		unsigned uf = 9*ft;
		return {szDec[uf]};
		// unsigned uf = 255 * ft, uMaj = uf >> 4, uMin = uf & 15;
		//return {szHex[0][uMaj], szHex[1][uMin]};
		//return {szHex[uMaj], szHex[uMin]};
	};

	unsigned W = 10, H = 10;
	float divisors[] = {1,1,1};
	pair<string, vector<string>> rows[] = {
		{"Linear", vector<string>(H)},
		{"Quadratic", vector<string>(H)},
		{"Cubic", vector<string>(H)}
		//{"Quartic", vector<string>(H)}
	};
	for(unsigned i = 0; i < W; i++) {
		float s = float(i)/(H-1);
		for(unsigned j = 0; j < H; j++) {
			float t = float(j)/(W-1);
			rows[0].second[j] += abbrev(linear(p22[0], p22[1],
					s/divisors[0], t/divisors[0]));
			//rows[0].second[j] += abbrev(linear(p22[0], p22[1],
			//		s/divisors[0], t/divisors[0]));
			rows[1].second[j] += abbrev(quadratic(p33[0], p33[1], p33[2],
					s/divisors[1], t/divisors[1]));
			rows[2].second[j] += abbrev(cubic(p44[0], p44[1], p44[2], p44[3],
					s/divisors[2], t/divisors[2]));
		}
		for(auto & r : rows)
			level_insert(r.second, " ");
	}
	for(auto const& r : rows) {
		cout << r.first << endl;
		for(auto const& row : r.second)
			cout << row << endl;
		cout << endl;
	}

}
