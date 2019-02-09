#ifndef TTAG_HPP
#define TTAG_HPP

#include "utility.hpp"
#include "tag.hpp"
#include "sequence.hpp"

namespace Detail {

template<template<class...> class C>
struct TsTag {};
template<template<class... S, S...> class C>
struct VTag {};

/*template<class C, class T = Tag<>>
struct TsPartial;

template<template<class...> class C, class... T>
struct TsPartial<TsTag<C>, Tag<T...>> {
	static constexpr bool complete = false;
};

template<template<class...> class C, class... T>
struct TsPartial<C, Tag<T...>, void> {
	static constexpr bool complete = false;
};

template<template<class...> class C, class... T>
struct TsPartial<C, Tag<T...>, Void_t<decltype(std::declval<C<T...>>())>> {
	static constexpr bool complete = true;
	typedef C<T...> type;
};*/

}

#endif
