#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <algorithm>
#include <limits>
#include <math.h>
#include <tuple>
#include <utility>
#include <type_traits>

namespace Detail {

/** Delimiter tag; only impacts compile-time resolution */
struct Delim {};

/** Allows SFINAE for missing members. */
template<class... T> struct Void;
/** Helper for Void's type */
template<class... T> using Void_t = typename Void<T...>::value_type;

/** A variadic tag, essentially a type sequence. */
template<class...> struct Tag;

/** Equivalent to std::tuple; see eval for conversion from Tag */
template<class... U>
using Val = std::tuple<U...>;

/** A variadic unsigned sequence, useful for indexing Tag/Val. */
template<unsigned... I>
struct Seq{}; //= Tag<std::integral_constant<unsigned, I>...>;

template<class... T> struct Void { using value_type = void; };

}

/** Abstract minimax (pair with first=min and second=max) */
template<class T> std::pair<long, long> minimax(T && t);

/** Abstract number of digits in decimal */
template<class T> long numDigits(T const& t);
// TODO: Consider numDigits with a (template?) parameter designating the radix


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
