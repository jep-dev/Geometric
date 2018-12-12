#include <iostream>
//#include <iomanip>
#include <sstream>

#include "binomial.hpp"
#include "dual.hpp"

using namespace Detail;

template<class S, class T, T T0, T... TN>
S& operator<<(S& os, Seq<T, T0, TN...> const&)
	{ return os << T0, os; }
template<class S, class T, T T0, T T1, T... TN>
S& operator<<(S& os, Seq<T, T0, T1, TN...> const&)
	{ return os << T0 << ", " << Seq<T, T1, TN...>{}, os; }

template<class T, T... TN>
std::string to_string(Seq<T, TN...> const& seq) {
	std::ostringstream oss;
	oss << seq;
	return "{" + oss.str() + "}";
}


template<class S>
S& testBinomial(S &s) { return s; }

template<class S, Size N, Size... I>
S& testBinomial(S &os, SeqSz<N, I...> = {}) {
	os << N << ". ";
	endl(os << Binomial<N>());
	return testBinomial<S, I...>(os);
}

int main(int argc, const char *argv[]) {
	using namespace std;
	// The executable size is independent of N, which should mean that the sequences
	// all disappear in abstract elimination.
	static constexpr unsigned N = 10, I = 4;
	typedef RepeatSeq<N, unsigned, 1> ones_t;                 // {1, 1, ..., 1, 1}
	typedef SumSeq<ValueType_t<false, ones_t>> sum_t;
	typedef PartialSumSeq<ones_t> inc_t;                      // {1, 2, ..., 9, 10}
	typedef ProductSeq<ValueType_t<false, inc_t>> product_t;
	typedef PartialSumSeq<ValueType_t<false, inc_t>> inc2_t;  // {1, 3, ..., 45, 55}

	cout << "10 1's: " << ones_t{} << endl;
	cout << "  - Sum: " << sum_t{} << endl;
	cout << "  - Partial sum: " << inc_t{} << endl;
	cout << "    - Product: " << product_t{} << endl;
	cout << "    - Partial sum: " << inc2_t{} << endl;
	cout << "    - Nth for N=" << I << ": " << get<I>(inc2_t{}) << endl;
	cout << '\n';

	cout << "Fibonacci<10> = " << Fibonacci<10>::value << endl;
	cout << '\n';

	cout << "Binomial coefficients: " << endl;
	testBinomial<ostream&, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9>(cout) << endl;

	//
}
