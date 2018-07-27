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

/** Tags a type with no value; only impacts compile-time resolution */
template<class...> struct Tag;
/** Delimiter tag; only impacts compile-time resolution */
struct Delim {};

/** Allows SFINAE for missing members. */
template<class... T> struct Void;
/** Helper for Void's type */
template<class... T> using Void_t = typename Void<T...>::value_type;

/** A variadic tag, essentially a type sequence. */
template<class...> struct Tag;

/** Abstract minimax (pair with first=min and second=max) */
template<class T> std::pair<long, long> minimax(T && t);

/** Abstract number of digits in decimal */
template<class T> long numDigits(T const& t);
// TODO: Consider numDigits with a (template?) parameter designating the radix


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

/** Empty exterior; may be necessary due to ambiguity between ext(U,Tag<>) and ext(Tag<>,X) */
auto ext(Tag<>, Tag<>) -> Tag<> { return {}; }
/** Exterior with empty LHS */
template<class X>
auto ext(Tag<>, X &&) -> Tag<> { return {}; }
/** Exterior with empty RHS */
template<class U>
auto ext(U &&, Tag<>) -> Tag<> { return {}; }
/** Exterior of simple types (sizeof...(U) = sizeof...(X) = 1) */
template<class U, class X>
auto ext(Tag<U>, Tag<X>) -> Tag<Tag<U,X>> { return {}; }
/** Exterior of variadic LHS and simple RHS (sizeof...(U) > 1 = sizeof...(X)) */
template<class U, class... V, class X>
auto ext(Tag<U, V...>, Tag<X>) -> Tag<Tag<U, X>, Tag<V, X>...> { return {}; }
/** Exterior of simple LHS and variadic RHS (sizeof...(U) = 1 < sizeof...(X)) */
template<class U, class X, class... Y>
auto ext(Tag<U>, Tag<X, Y...>) -> Tag<Tag<U, X>, Tag<U, Y>...> { return {}; }
/** Exterior of variadic LHS and RHS (sizeof...(U) > 1, sizeof...(X) > 1) */
template<class U, class... V, class X, class... Y>
auto ext(Tag<U, V...>, Tag<X, Y...>) -> decltype(Tag<Tag<U, X>, Tag<U, Y>...>{}
	+ ext(Tag<V...>{}, Tag<X,Y...>{})) { return {}; }
/** Syntax sugar for ext (exterior); avoid where operator^ may be ambiguous (arithmetic, etc.) */
template<class... U, class... X>
auto operator^(Tag<U...> u, Tag<X...> x) -> decltype(ext(u, x)) { return ext(u, x); }

template<class... U>
using Val = std::tuple<U...>;

template<class U>
auto eval(U && u = {}) { return std::forward<U>(u); }
template<class... U>
auto eval(Tag<U...>) -> Val<decltype(eval(std::declval<U>()))...> { return {eval(U{})...}; }

template<class T>
auto access(T && t) -> decltype(t) { return std::forward<T>(t); }

template<unsigned I, class... T>
auto access(Val<T...> const& t) -> decltype(std::get<I>(t)) { return std::get<I>(t); }
template<unsigned I, unsigned J, unsigned... K, class... T>
auto access(Val<T...> const& t) -> decltype(access<J, K...>(std::get<I>(t))) {
	return access<J, K...>(std::get<I>(t));
}
template<unsigned I, class... T>
auto access(Val<T...> & t) -> decltype(std::get<I>(t)) { return std::get<I>(t); }
template<unsigned I, unsigned J, unsigned... K, class... T>
auto access(Val<T...> & t) -> decltype(access<J, K...>(std::get<I>(t))) {
	return access<J, K...>(std::get<I>(t));
}
template<unsigned I, class... T>
auto access(Val<T...> && t) -> decltype(std::get<I>(t)) { return std::get<I>(t); }
template<unsigned I, unsigned J, unsigned... K, class... T>
auto access(Val<T...> && t) -> decltype(access<J, K...>(std::get<I>(t))) {
	return access<J, K...>(std::get<I>(t));
}


template<class U, class... V, class X = Tag<>>
auto reverse(Tag<U, V...>, X x = {})
-> decltype(reverse(Tag<V...>{}, Tag<U>{} + x)) { return {}; }
template<class... X>
auto reverse(Tag<>, Tag<X...> x = {}) -> decltype(x) { return {}; }

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

template<class... T> struct Void { using value_type = void; };

template<class T>
long numDigits(T const& t) {
	return ceil(log10(abs(long(t))+1));
}

#endif
