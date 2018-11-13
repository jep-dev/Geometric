#include <iostream>
//#include <iomanip>
//#include <sstream>

#include "binomial.hpp"

template<class S>
void testBinomial(S &s) { return s; }

template<class S, unsigned N, unsigned... IN>
S& testBinomial(S &os) {
	endl(os << Binomial<N>());
	return testBinomial<S, IN...>(os);
}

int main(int argc, const char *argv[]) {
	/*
	 * Seq<I...> -> Tag<Binomial<I>::value_type...>
	 */
}
