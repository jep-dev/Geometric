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



template<class L, class R>
bool near_zero(L lhs, R rhs) { return abs(lhs) <= rhs; }

template<class L, class R, class T>
bool near(L lhs, R rhs, T least) { return near_zero(lhs - rhs, least); }

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
	return ceil(log10(abs(long(t))+1));
}
template<class T>
long numDigits(T const& t, unsigned radix) {
	return ceil(log(abs(long(t))+1)/log(radix));
}

#endif
