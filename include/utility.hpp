#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <algorithm>
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

template<class T, class SFINAE = void>
struct ValueType {
	typedef T type;
};

template<class... T>
using ValueType_t = typename ValueType<T...>::type;

template<class T>
struct ValueType<T, Detail::Void_t<typename T::value_type>> {
	typedef ValueType_t<typename T::value_type> type;
};

template<class S, class... T>
auto get_value_type(Detail::Tag<S> = {}) -> ValueType_t<S, T...> { return {}; }

template<class... T>
using Common_ValueType_t = std::common_type_t<ValueType_t<T>...>;

template<class S, class T, class... U>
struct Add_ValueType;

template<class S, class T, class... U>
struct Add_ValueType {
	using value_type = decltype(std::declval<ValueType_t<S>>()
		+ std::declval<ValueType_t<T>>());
};

template<class... T> using Add_ValueType_t = ValueType_t<Add_ValueType<T...>>;

template<class R, class S, class T, class... U>
struct Add_ValueType<R, S, T, U...> {
	using value_type = Add_ValueType_t<Add_ValueType_t<Add_ValueType_t<R, S>, T>, U...>;
};
}

#endif
