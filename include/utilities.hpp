#ifndef UTILITIES_HPP
#define UTILITIES_HPP

///@cond
#include <algorithm>
#include <limits>
#include <string>
#include <tuple>
#include <utility>
#include <math.h>
///@endcond

namespace Detail {

/** Allows SFINAE for missing members. */
template<class... T> struct Void;
/** Tags a type with no value; only impacts compile-time resolution */
template<class...> struct Tag;

/** Helper for Void's type */
template<class... T> using Void_t = typename Void<T...>::value_type;

/** A variadic tag, essentially a type sequence. */
template<class...> struct Tag;

template<class T> std::pair<long, long> minimax(T && t);

template<class... T> struct Void { using value_type = void; };

template<class...> struct Tag {
	typedef Tag<> head_type;
	typedef Tag<> tail_type;
	static constexpr unsigned size = 0;
};
template<class HEAD, class... REST> struct Tag<HEAD, REST...> {
	typedef Tag<HEAD> head_type;
	typedef Tag<REST...> tail_type;
	static constexpr unsigned size = 1 + sizeof...(REST);
};

/** Direct sum (treats LHS and RHS as orthogonal) */
template<class... LHS, class... RHS>
Tag<LHS..., RHS...> operator+(Tag<LHS...>, Tag<RHS...>) { return {}; }

/** Inner product */
template<class... LN, class... RN>
Tag<Tag<LN,RN>...> operator*(Tag<LN...>, Tag<RN...>) { return {}; }

template<class U>
auto ext(U &&, Tag<>) -> Tag<> { return {}; }
template<class X>
auto ext(Tag<>, X &&) -> Tag<> { return {}; }
template<class U, class X>
auto ext(Tag<U>, Tag<X>) -> Tag<Tag<U,X>> { return {}; }
template<class U, class... V, class X>
auto ext(Tag<U, V...>, Tag<X>) -> Tag<Tag<U, X>, Tag<V, X>...> { return {}; }
template<class U, class X, class... Y>
auto ext(Tag<U>, Tag<X, Y...>) -> Tag<Tag<U, X>, Tag<U, Y>...> { return {}; }
template<class U, class... V, class X, class... Y>
auto ext(Tag<U, V...>, Tag<X, Y...>) -> decltype(Tag<Tag<U, X>, Tag<U, Y>...>{}
	+ ext(Tag<V...>{}, Tag<X,Y...>{})) { return {}; }

template<class... U, class... X>
auto operator^(Tag<U...> u, Tag<X...> x) -> decltype(ext(u, x)) { return ext(u, x); }

template<class... U>
using Val = std::tuple<U...>;

template<class U>
auto eval(U && u = {}) { return std::forward<U>(u); }
template<class... U>
auto eval(Tag<U...>) -> Val<decltype(eval(std::declval<U>()))...> { return {eval(U{})...}; }

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

#endif
