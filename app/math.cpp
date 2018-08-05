#include "math.hpp"
#include <iostream>
#include <iomanip>
#include <vector>

template<class S>
std::ostream& center(std::ostream& out, S const& s, unsigned N) {
	using namespace std;
	ostringstream oss;
	oss << s;
	auto str = oss.str();
	auto len = str.length(), dlen = N - len, dlen0 = dlen/2, dlen1 = dlen - dlen0;
	return out << setw(dlen0) << "" << str << setw(dlen1) << "";
}

int main(int argc, const char *argv[]) {
	using namespace std;

	typedef float T;

	unsigned lmax = 32, rmax = 16;
	T nl = .5f, dnl = .5f, nls[] = {nl, nl*dnl, nl*powf(dnl,2), nl*powf(dnl,3), nl*powf(dnl,4)},
		dl = .25f, dr = .5f,
		adl = abs(dl), adr = abs(dr),
		adlMaj = floor(adl), adrMaj = floor(adr),
		adlMin = adl - adlMaj, adrMin = adr - adrMaj,
		adlMax = abs(dl*lmax), adrMax = abs(dr*rmax);
	constexpr unsigned n_nls = sizeof(nls) / sizeof(nls[0]);
	static constexpr const char *epsilon = "\u03B5";
	string shades[n_nls+1] = {"", "`", "'", "\"", "*", "#"};
	auto shade_max = minimax(shades).second;
	unsigned numDigitsNl = -log2(nls[n_nls-1]),
			numDigitsL = numDigits(adlMax),
			numDigitsR = numDigits(adrMax),
			numDecimalsL = 0, numDecimalsR = 0;
	if(adlMin > 0) {
		numDecimalsL = floor(abs(log2(adlMin)));
		numDigitsL += 1 + numDecimalsL;
	}
	if(adrMin > 0) {
		numDecimalsR = floor(abs(log2(adrMin)));
		numDigitsR += 1 + numDecimalsR;
	}
	if(dl < 0) numDigitsL++;
	if(dr < 0) numDigitsR++;
	auto numDigitsR0 = numDigitsR/2, numDigitsR1 = numDigitsR - numDigitsR0,
			numDigitsL0 = numDigitsL/2, numDigitsL1 = numDigitsL - numDigitsL0;
	auto setL = setw(numDigitsL), setL0 = setw(numDigitsL0), setL1 = setw(numDigitsL1),
			setR = setw(numDigitsR), setR0 = setw(numDigitsR0), setR1 = setw(numDigitsR1);

	vector<T> lvals, rvals;
	for(auto l = 0; l <= lmax; l++) lvals.emplace_back(l*dl);
	for(auto r = 0; r <= rmax; r++) rvals.emplace_back(r*dr);

	unsigned n_total = rvals.size() * (1+numDigitsR) + numDigitsL + 2;
	center(cout << setfill('_'), "Floating point comparisons", n_total) << setfill(' ') << endl;
	cout << "Legend: ";
	int n_nls_digits = ceil(abs(log2(nls[n_nls-1])));
	for(auto i = 0; i < n_nls+1; i++) {
		cout << "[" << setw(shade_max) << shades[i] << "]=" << left;
		if(i) cout << "1/2^" << setw(n_nls_digits-4) << ceil(abs(log2(nls[i-1])));
		else cout << setw(n_nls_digits) << "n/a";
		cout << "  ";
	}
	cout << "|\n" << setfill('-') << setw(n_total) << "" << "+\n" << setfill(' ');
	cout << fixed << right << setprecision(numDecimalsR) << setL0 << "" << setL1 << "" << "  ";
	for(T const& rhs : rvals) cout << setR << rhs << " ";
	cout << '|' << endl;
	for(T const& lhs : lvals) {
		cout << setL << setprecision(numDecimalsL) << lhs << ": ";
		for(T const& rhs : rvals) {
			unsigned nears = 0;
			for(auto const& n : nls)
				if(near(lhs, rhs, n)) nears++;
			center(cout, shades[nears], numDigitsR+1);
		}
		cout << '|' << endl;
	}
	cout << setfill('-') << setw(n_total) << "" << "'" << endl;
}
