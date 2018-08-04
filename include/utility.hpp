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

#endif
