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
	static constexpr unsigned N = 10, I = 4;
	typedef RepeatSeq<N, unsigned, 1> ones_t; // {1, 1, ..., 1, 1}
	typedef SumSeq<typename ones_t::value_type> sum_t;
	typedef PartialSumSeq<ones_t> inc_t;      // {1, 2, ..., 9, 10}
	typedef ProductSeq<typename inc_t::value_type> product_t;
	typedef PartialSumSeq<typename inc_t::value_type> inc2_t;      // {1, 3, ..., 45, 55}

	cout << "10 1's: " << ones_t{} << endl;
	cout << "  - Sum: " << sum_t{} << endl;
	cout << "  - Partial sum: " << inc_t{} << endl;
	cout << "    - Product: " << product_t{} << endl;
	cout << "    - Partial sum: " << inc2_t{} << endl;
	cout << "    - Nth for N=" << I << ": " << inc2_t::nth<I>() << endl;
	cout << '\n';

	cout << "Binomial coefficients: " << endl;
	testBinomial<std::ostream&, 0, 1, 2, 3, 4, 5, 6>(cout);
}
