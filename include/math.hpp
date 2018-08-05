#ifndef MATH_HPP
#define MATH_HPP

#include <algorithm>
#include <math.h>
#include <type_traits>
#include <limits>
#include <utility>

/** Abstract comparison to zero (mainly intended for floating point types) */
template<class L, class R = L>
bool near_zero(L, R = std::numeric_limits<std::remove_reference_t<L>>::epsilon());

/** Abstract comparison (mainly intended for floating point types) */
template<class L, class R = L, class T = R>
bool near(L, R, T = std::numeric_limits<
		std::remove_reference_t<std::common_type_t<L,R>>>::epsilon());

/** Abstract minimax (pair with first=min and second=max) */
template<class T> std::pair<long, long> minimax(T && t);

/** Abstract number of digits in decimal */
template<class T> long numDigits(T const& t);


/*
template<class L, class R>
bool near_zero(L lhs, R rhs) { return abs(lhs) <= rhs; }

template<class L, class R, class T>
bool near(L lhs, R rhs, T least) { return near_zero(lhs - rhs, least); }
*/


/* Credit to floating-point-gui.de for the combination of relative and absolute comparisons;
 * I read enough of Bruce Dawson's articles to decide that a "decent" solution will have to do
 * without specific constraints. */
template<class L, class R, class T>
bool near(L lhs, R rhs, T least) {
	using LRT = std::common_type_t<L, R, T>;
	using NL = std::numeric_limits<LRT>;
	auto epsilon = NL::epsilon(), minnorm = NL::min();
	if(lhs == rhs) return true;
	L labs = abs(lhs), rabs = abs(rhs), dabs = abs(lhs - rhs);
	if((labs == 0) || (rabs == 0) || (dabs < minnorm))
		return dabs < (least * minnorm);
	return dabs / std::min(labs + rabs, NL::max()) < least;
}
template<class L, class T>
bool near_zero(L lhs, T least) {
	return near(lhs, 0, least);
}


template<class T>
std::pair<long, long> minimax(T && t) {
	long m0 = std::numeric_limits<long>::max(), m1 = 0;
	for(auto &it : t) {
		long len = it.length();
		m0 = std::min(len, m0);
		m1 = std::max(len, m1);
	}
	if(m0 > m1) m0 = m1;
	return {m0, m1};
}

template<class T>
long numDigits(T const& t) {
	long out = ceil(log10(abs(long(t))+1));
	return out ? out : 1;
}
template<class T>
long numDigits(T const& t, unsigned radix) {
	long out = ceil(log(abs(long(t))+1)/log(radix));
	return out ? out : 1;
}

#endif
