#ifndef MATH_HPP
#define MATH_HPP

#include <math.h>
#include <type_traits>
#include <limits>

/** Abstract comparison to zero (mainly intended for floating point types) */
template<class T>
bool near_zero(T const& t, T least = std::numeric_limits<T>::epsilon());

/** Abstract comparison (mainly intended for floating point types) */
template<class T>
bool near(T const& t, T const& cmp, T least = std::numeric_limits<T>::epsilon());

/** Abstract minimax (pair with first=min and second=max) */
template<class T> std::pair<long, long> minimax(T && t);

/** Abstract number of digits in decimal */
template<class T> long numDigits(T const& t);



template<class T>
bool near_zero(T const& t, T least) {
	return t <= least;
}
template<class T>
bool near(T const& t, T const& cmp, T least) {
	return near_zero(t - cmp, least);
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
	return ceil(log10(abs(long(t))+1));
}
template<class T>
long numDigits(T const& t, unsigned radix) {
	return ceil(log(abs(long(t))+1)/log(radix));
}

#endif
