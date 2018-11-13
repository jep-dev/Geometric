#include <iostream>
//#include <iomanip>
//#include <sstream>

#include "binomial.hpp"

template<class S>
S& testBinomial(S &s) { return s; }

template<class S, unsigned N, unsigned... IN>
S& testBinomial(S &os) {
	os << "Binomial coefficients (" << N << "): ";
	endl(os << Binomial<N>());
	return testBinomial<S, IN...>(os);
}

int main(int argc, const char *argv[]) {
	testBinomial<std::ostream&, 0, 1, 2, 3, 4, 5, 6>(std::cout);
}
