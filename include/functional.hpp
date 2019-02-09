#ifndef FUNCTIONAL_HPP
#define FUNCTIONAL_HPP

#include "sequence.hpp"

namespace Detail {

template<std::size_t I = 0, class F, class... S, class... T>
auto for_each(std::tuple<S...> const& s, F const& f, T &&... t)
		-> std::enable_if_t<I == sizeof...(S), void> {}
template<std::size_t I = 0, class F, class... S, class... T>
auto for_each(std::tuple<S...> const& s, F const& f, T &&... t)
		-> std::enable_if_t<I < sizeof...(S), void> {
	f(std::get<I>(s), t...);
	for_each<I+1>(s, f, std::forward<T>(t)...);
}
template<std::size_t I = 0, class F, class... S, class... T>
auto for_each(std::tuple<S...> & s, F const& f, T &&... t)
		-> std::enable_if_t<I == sizeof...(S), void> {}
template<std::size_t I = 0, class F, class... S, class... T>
auto for_each(std::tuple<S...> & s, F const& f, T &&... t)
		-> std::enable_if_t<(I < sizeof...(S)), void> {
	f(std::get<I>(s), std::forward<T>(t)...);
	for_each<I+1>(s, f, std::forward<T>(t)...);
}

// TODO working on constexpr variants, considering making qualifiers optional
// (all combinations of qualifiers would warrant either selective specialization
// or C macro firepower.
/*template<std::size_t I = 0, class S, S... V, class F, class... T>
auto for_each(Seq<S, V...> const& v, F const& f, T &&... t)
	-> std::enable_if_t<(I == sizeof...(V), void> {}
template<std::size_t I = 0, class S, S... V, class F, class... T>
auto for_each(Seq<S, V...> const& v, F const& f, T &&... t)
	-> std::enable_if_t<(I < sizeof...(V)), void> {
	f(get<I>(v), t...);
	for_each<I+1>(v, f, std::forward<T>(t)...);
}*/

template<class F, class... S, class... T, unsigned... I>
auto transform(SeqU<I...>, std::tuple<S...> const& s, F const& f, T &&... t)
	-> std::tuple<decltype(f(std::get<I>(s), std::forward<T>(t)...))...>
		{ return std::make_tuple(f(std::get<I>(s), std::forward<T>(t)...)...); }
template<class F, class... S, class... T, class INC = IncSeq<sizeof...(S)>>
auto transform(std::tuple<S...> const& s, F const& f, T &&... t)
	-> decltype(transform(INC{}, s, f, std::forward<T>(t)...))
		{ return transform(INC{}, s, f, std::forward<T>(t)...); }


/** @brief Creates a type sequence of type pairs found in parallel U, V */
template<class... U, class... V>
constexpr auto zip_tag(Tag<U...> const&, Tag<V...> const&) -> Tag<Tag<U,V>...> { return {}; }
/** @brief Creates two type sequences from one by splitting each element */
template<class... U, class... V>
constexpr auto unzip_tag(Tag<Tag<U, V>...> const&) -> Tag<Tag<U...>, Tag<V...>> { return {}; }

template<class T, T... U, T... V>
constexpr auto zip_seq(Seq<T, U...> const&, Seq<T, V...> const&)
	-> Tag<Seq<T, U, V>...> { return {}; }
template<class T, T... U, T... V>
constexpr auto unzip_seq(Tag<Seq<T, U, V>...> const&)
	-> Tag<Seq<T, U...>, Seq<T, V...>> { return {}; }

/** Maps a sequence of values to a sequence of individual value types */
template<class S, S... I>
auto seq_to_tag_seq(Seq<S, I...> const&) -> Tag<Seq<S,I>...> { return {}; }

/** @brief Converts the sequence to tuple sequence segments */
template<class S, S... I>
auto seq_to_tuple_seq(Seq<S, I...> const&)
	-> std::tuple<Seq<S,I>...> { return std::make_tuple(Seq<S,I>{}...); }

/** @brief Transforms sequence elements to tuple elements */
template<class S, S... I, class F, class... T>
auto seq_to_tuple(Seq<S, I...> const&, F const& f, T &&... t)
	-> std::tuple<decltype(f(I, std::forward<T>(t)...))...>
		{ return std::make_tuple(f(I, std::forward<T>(t)...)...); }

/** @brief Transforms sequence elements to array elements */
template<class S, S I0, S... I, class F, class... T>
auto seq_to_array(Seq<S, I0, I...> const&, F const& f, T &&... t)
	-> std::array<decltype(f(I0, std::forward<T>(t)...)), 1 + sizeof...(I)>
		{ return {f(I0, std::forward<T>(t)...), f(I, std::forward<T>(t)...)...}; }


/*template<class... T>
void for_each(T &&... t) { return Detail::for_each(std::forward<T>(t)...); }
template<class... T>
auto transform(T &&... t) -> decltype(Detail::transform(std::forward<T>(t)...)) {
	return Detail::transform(std::forward<T>(t)...);
}*/

/*// Lvalue TTOA
template<class K, K... V, class S, class... T, class U = S>
auto tuple_to_array(Seq<K, V...> & indices, std::tuple<S, T...> & tup, Tag<U> = {})
-> std::array<U, sizeof...(V)> { return {(U) std::get<V>(tup)...}; }
*/// Const lvalue TTOA
template<class K, K... V, class S, class... T, class U = S>
auto tuple_to_array(Seq<K, V...> const& indices, std::tuple<S, T...> const& tup, Tag<U> = {})
-> std::array<U, sizeof...(V)> { return {(U) std::get<V>(tup)...}; }
/*// Lvalue TTOA, no indices (default)
template<class S, class... T, class U = S>
auto tuple_to_array(std::tuple<S, T...> const& t, Tag<U> u = {})
-> decltype(tuple_to_array(IncSeq<sizeof...(T)+1, true, std::size_t>{}, t, u))
	{ return tuple_to_array(IncSeq<sizeof...(T)+1, true, std::size_t>{}, t, u); }
*/// Const lvalue TTOA, no indices (default)
template<class S, class... T, class U = S>
auto tuple_to_array(std::tuple<S, T...> const& t, Tag<U> u = {})
-> decltype(tuple_to_array(IncSeq<sizeof...(T)+1, true, std::size_t>{}, t, u))
	{ return tuple_to_array(IncSeq<sizeof...(T)+1, true, std::size_t>{}, t, u); }


}
#endif
