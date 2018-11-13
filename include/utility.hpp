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

/** Removes qualifiers correctly, even for arrays */
template<class S> struct RemoveCVRef;
/** Helper for RemoveCVRef's type */
template<class S> using RemoveCVRef_t = typename RemoveCVRef<S>::type;

/** A variadic tag, essentially a type sequence. */
template<class...> struct Tag;

/** Equivalent to std::tuple; see eval for conversion from Tag */
template<class... U>
using Val = std::tuple<U...>;

template<class... T> struct Void { using value_type = void; };

template<class S>
struct RemoveCVRef { typedef std::remove_cv_t<std::remove_reference_t<S>> type; };
template<class S, unsigned N>
struct RemoveCVRef<S[N]> { typedef RemoveCVRef_t<S> type[N]; };

template<bool RECURSE, class T, class SFINAE = void>
struct ValueType { typedef T value_type; };
template<bool RECURSE, class... T>
using ValueType_t = typename ValueType<RECURSE, T...>::value_type;
template<bool RECURSE, class T>
struct ValueType<RECURSE, T, Detail::Void_t<typename T::value_type>> {
	typedef std::conditional_t<RECURSE,
			ValueType_t<RECURSE, typename T::value_type>,
			typename T::value_type> value_type;
};

// usage: get_value_type<RECURSE>(Tag<S> + Tag<T...>),
//        get_value_type<RECURSE, S, T...>(), etc.
template<bool RECURSE, class S, class... T>
auto get_value_type(Detail::Tag<S, T...> = {}, std::integral_constant<bool, RECURSE> = {})
	-> ValueType_t<RECURSE, S, T...> { return {}; }

// usage: get_value_type<RECURSE>(s, t...)
template<bool RECURSE, class S, class... T>
auto get_value_type(S && s, T &&... t)
	-> ValueType_t<RECURSE, S, T...> { return {}; }

template<bool RECURSE, class... T>
using CommonValueType_t = std::common_type_t<ValueType_t<RECURSE, T>...>;

template<class S, class... T> struct SumType { typedef S value_type; };
template<class S, class... T> struct DifferenceType { typedef S value_type; };
template<class S, class... T> struct ProductType { typedef S value_type; };
template<class S, class... T> struct QuotientType { typedef S value_type; };
template<bool RECURSE, class S, class... U>
struct SumValueType { typedef ValueType_t<RECURSE, S> value_type; };
template<bool RECURSE, class S, class... U>
struct DifferenceValueType { typedef ValueType_t<RECURSE, S> value_type; };
template<bool RECURSE, class S, class... U>
struct ProductValueType { typedef ValueType_t<RECURSE, S> value_type; };
template<bool RECURSE, class S, class... U>
struct QuotientValueType { typedef ValueType_t<RECURSE, S> value_type; };

template<class... T>
using SumType_t = typename SumType<T...>::value_type;
template<class... T>
using DifferenceType_t = typename DifferenceType<T...>::value_type;
template<class... T>
using ProductType_t = typename ProductType<T...>::value_type;
template<class... T>
using QuotientType_t = typename QuotientType<T...>::value_type;
template<bool R, class... T>
using SumValueType_t = typename SumValueType<R, T...>::value_type;
template<bool R, class... T>
using DifferenceValueType_t = typename DifferenceValueType<R, T...>::value_type;
template<bool R, class... T>
using ProductValueType_t = typename ProductValueType<R, T...>::value_type;
template<bool R, class... T>
using QuotientValueType_t = typename QuotientValueType<R, T...>::value_type;

template<class S, class T, class... U> struct SumType<S, T, U...> {
	typedef decltype(std::declval<S>()
		+ std::declval<SumType_t<T, U...>>()) value_type;
};
template<bool R, class S, class T, class... U>
struct SumValueType<R, S, T, U...> {
	typedef decltype(std::declval<ValueType_t<R, S>>()
		+ std::declval<ValueType_t<R, T, U...>>()) value_type;
};

template<bool R, class S, class T, class... U>
struct ProductValueType<R, S, T, U...> {
	typedef decltype(std::declval<ValueType_t<R, S>>()
			* std::declval<ProductValueType_t<R, T, U...>>()) value_type;
};

}

#endif
