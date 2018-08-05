#include "math.hpp"
#include <iostream>
#include <iomanip>
#include <vector>

int main(int argc, const char *argv[]) {
	using namespace std;

	typedef float T;

	static constexpr const char *epsilon = "\u03B5";
	cout << "Proximity:\n";
	T nl = .1, dl = .125, dr = dl,
			adl = abs(dl), adr = abs(dr),
			adlMaj = floor(adl), adrMaj = floor(adr),
			adlMin = adl - adlMaj, adrMin = adr - adrMaj;
	unsigned lmax = 8, rmax = ceil(lmax * dl / dr),
			numDigitsL = ceil(log10(adlMaj+1)+1),
			numDigitsR = ceil(log10(adrMaj+1)+1);
	if(adlMin > 0) numDigitsL += 1 + floor(abs(log2(adlMin)));
	if(adrMin > 0) numDigitsR += 1 + floor(abs(log2(adrMin)));
	if(dl < 0) numDigitsL++;
	if(dr < 0) numDigitsR++;
	auto setL = setw(numDigitsL), setR = setw(numDigitsR);

	vector<T> lvals, rvals;
	for(auto l = 0; l <= lmax; l++)
		lvals.emplace_back(l*dl);
	for(auto r = 0; r <= rmax; r++)
		rvals.emplace_back(r*dr);
	cout << right << setL << "" << "  ";
	for(T const& rhs : rvals)
		cout << setR << rhs << " ";
	cout << endl;
	for(T const& lhs : lvals) {
		cout << setL << lhs << ": ";
		for(T const& rhs : rvals) {
			cout << setR << near(lhs, rhs, nl) << " ";
		}
		cout << endl;
	}
	return 0;
}
