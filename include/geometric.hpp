#ifndef GEOMETRIC_HPP
#define GEOMETRIC_HPP

#include "utility.hpp"

namespace Detail {

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
auto operator^(Tag<>, Tag<>) -> Tag<> { return {}; }

/** Exterior with empty LHS */
template<class X>
auto operator^(Tag<>, X &&) -> Tag<> { return {}; }

/** Exterior with empty RHS */
template<class U>
auto operator^(U &&, Tag<>) -> Tag<> { return {}; }

/** Exterior of simple types (sizeof...(U) = sizeof...(X) = 1) */
template<class U, class X>
auto operator^(Tag<U>, Tag<X>) -> Tag<Tag<U,X>> { return {}; }

/** Exterior of variadic LHS and simple RHS (sizeof...(U) > 1 = sizeof...(X)) */
template<class U, class... V, class X>
auto operator^(Tag<U, V...>, Tag<X>) -> Tag<Tag<U, X>, Tag<V, X>...> { return {}; }

/** Exterior of simple LHS and variadic RHS (sizeof...(U) = 1 < sizeof...(X)) */
template<class U, class X, class... Y>
auto operator^(Tag<U>, Tag<X, Y...>) -> Tag<Tag<U, X>, Tag<U, Y>...> { return {}; }

/** Exterior of variadic LHS and RHS (sizeof...(U) > 1, sizeof...(X) > 1) */
template<class U, class... V, class X, class... Y>
auto operator^(Tag<U, V...>, Tag<X, Y...>) -> decltype(Tag<Tag<U, X>, Tag<U, Y>...>{}
	+ (Tag<V...>{} ^ Tag<X,Y...>{})) { return {}; }

/** Eval's default case; identity with perfect forwarding */
template<class U>
auto eval(U && u = {}) { return std::forward<U>(u); }
/** Maps a value-free Tag to a tag-free Val (tuple) */
template<class... U>
auto eval(Tag<U...>) -> Val<decltype(eval(std::declval<U>()))...> { return {eval(U{})...}; }

/** Access's default case; identity with perfect forwarding */
template<class T>
auto access(T && t) -> decltype(t) { return std::forward<T>(t); }

/** (Const reference) access's base case; delegates to std::get */
template<unsigned I, class... T>
auto access(Val<T...> const& t) -> decltype(std::get<I>(t)) { return std::get<I>(t); }

/** (Const reference) access's inductive case; recurses to compose the tail with std::get */
template<unsigned I, unsigned J, unsigned... K, class... T>
auto access(Val<T...> const& t) -> decltype(access<J, K...>(std::get<I>(t))) {
	return access<J, K...>(access<I>(t));
}

/** (Reference) access's base case; delegates to std::get */
template<unsigned I, class... T>
auto access(Val<T...> & t) -> decltype(std::get<I>(t)) { return std::get<I>(t); }

/** (Reference) access's inductive case; recurses to compose the tail with std::get */
template<unsigned I, unsigned J, unsigned... K, class... T>
auto access(Val<T...> & t) -> decltype(access<J, K...>(std::get<I>(t))) {
	return access<J, K...>(access<I>(t));
}

/** (Rvalue) access's base case; delegates to std::get */
template<unsigned I, class... T>
auto access(Val<T...> && t) -> decltype(std::get<I>(std::move(t))) {
	return std::get<I>(std::move(t));
}

/** (Rvalue) access's inductive case; recurses to compose the tail with std::get */
template<unsigned I, unsigned J, unsigned... K, class... T>
auto access(Val<T...> && t) -> decltype(access<J, K...>(access<I>(std::move(t)))) {
	return access<J, K...>(std::get<I>(std::move(t)));
}

/** Reverse's inductive case; recurses after moving the head of the LHS to the RHS */
template<class U, class... V, class X = Tag<>>
auto reverse(Tag<U, V...>, X x = {})
-> decltype(reverse(Tag<V...>{}, Tag<U>{} + x)) { return {}; }

/** Reverse's base case; simply returns the RHS */
template<class... X>
auto reverse(Tag<>, Tag<X...> x = {}) -> decltype(x) { return {}; }

template<class X, class... Y>
auto rotate(Tag<X, Y...> = {}) -> Tag<Y..., X> { return {}; }

}

#endif
