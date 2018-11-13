#include <iostream>
//#include <iomanip>
//#include <sstream>

#include "binomial.hpp"

template<class S>
S& testBinomial(S &s) { return s; }

template<class S, unsigned N, unsigned... IN>
S& testBinomial(S &os) {
	os << N << ". ";
	endl(os << Binomial<N>());
	return testBinomial<S, IN...>(os);
}

int main(int argc, const char *argv[]) {
	using namespace std;
	using namespace Detail;
	// The executable size is independent of N, which should mean that the sequences
	// all disappear in abstract elimination.
	static constexpr unsigned N = 10;
	typedef RepeatSeq<N, unsigned, 1> ones_t;
	typedef PartialSumSeq<ones_t> inc_t;
	typedef PartialSumSeq<inc_t> inc2_t;
	cout << "10 repeats of 1: " << ones_t{} << endl;
	cout << "1st partial sum: " << inc_t{} << endl;
	cout << "2nd partial sum: " << inc2_t{} << endl;
	cout << endl;
	cout << "Binomial coefficients: " << endl;
	testBinomial<std::ostream&, 0, 1, 2, 3, 4, 5, 6>(cout);
}
