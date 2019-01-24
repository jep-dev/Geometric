#ifndef FUNCTIONAL_HPP
#define FUNCTIONAL_HPP

#include "sequence.hpp"
#include "tag.hpp"

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

//namespace Detail {

template<class F, class... S, class... T, unsigned... I>
auto transform(Detail::SeqU<I...>, std::tuple<S...> const& s, F const& f, T &&... t)
	-> std::tuple<decltype(f(std::get<I>(s), std::forward<T>(t)...))...>
		{ return std::make_tuple(f(std::get<I>(s), std::forward<T>(t)...)...); }
/*template<class F, class... S, class... T, class DESC = Detail::DecSeq<sizeof...(S)>>
auto transform(std::tuple<S...> const& s, F const& f, T &&... t)
	-> decltype(transform(DESC{}, s, f, std::forward<T>(t)...))
		{ return transform(DESC{}, s, f, std::forward<T>(t)...); }*/
template<class F, class... S, class... T, class INC = Detail::IncSeq<sizeof...(S)>>
auto transform(std::tuple<S...> const& s, F const& f, T &&... t)
	-> decltype(transform(INC{}, s, f, std::forward<T>(t)...))
		{ return transform(INC{}, s, f, std::forward<T>(t)...); }

//}

/*
template<class... S, class F, class... T>
auto transform(std::tuple<S...> &s, F const& f, T &&... t)
	-> decltype(Detail::transform(Detail::DecSeq<sizeof...(S)>{}, s, f, std::forward<T>(t)...))
	{ return Detail::transform(Detail::DecSeq<sizeof...(S)>{}, s, f, std::forward<T>(t)...); }
template<class... S, class F, unsigned... I, class... T>
auto transform(Detail::SeqU<I...> i, std::tuple<S...> & s, F const& f, T &&... t)
	-> decltype(Detail::transform(reverse(i), s, f, std::forward<T>(t)...))
		{ return Detail::transform(reverse(i), s, f, std::forward<T>(t)...); }
*/

/// @brief Transforms tuple elements to tag elements
template<class... S>
auto tuple_to_tag(std::tuple<S...> const&) -> Detail::Tag<S...> { return {}; }

/// @brief Transforms tag segments to tuple elements (values)
template<class... S, class F, class... T>
auto tag_to_tuple(Detail::Tag<S...> const&, F const& f, T &&... t)
	-> std::tuple<decltype(f(Detail::Tag<S>{}, std::forward<T>(t)...))...>
		{ return std::make_tuple(f(Detail::Tag<S>{}, std::forward<T>(t)...)...); }

/*
/// @brief Transforms template type tags to values in a tuple
template<template<class...> class... S, class F, class... T>
auto ttag_to_tuple(Detail::Tag<Detail::TTag<S>...> const&, F const& f, T &&... t)
	-> std::tuple<decltype(f(Detail::TTag<S>{}, std::forward<T>(t)...))...>
		{ return std::make_tuple(f(Detail::TTag<S>{}, std::forward<T>(t)...)...); }
*/

/// @brief Converts the sequence to tuple sequence segments
template<class S, S... I>
auto seq_to_tuple_seq(Detail::Seq<S, I...> const&)
	-> std::tuple<Detail::Seq<S,I>...> { return std::make_tuple(Detail::Seq<S,I>{}...); }

/// @brief Transforms sequence elements to tuple elements
template<class S, S... I, class F, class... T>
auto seq_to_tuple(Detail::Seq<S, I...> const&, F const& f, T &&... t)
	-> std::tuple<decltype(f(I, std::forward<T>(t)...))...>
		{ return std::make_tuple(f(I, std::forward<T>(t)...)...); }

/// @brief Transforms sequence elements to array elements
template<class S, S I0, S... I, class F, class... T>
auto seq_to_array(Detail::Seq<S, I0, I...> const&, F const& f, T &&... t)
	-> std::array<decltype(f(I0, std::forward<T>(t)...)), 1 + sizeof...(I)>
		{ return {f(I0, std::forward<T>(t)...), f(I, std::forward<T>(t)...)...}; }

#endif
