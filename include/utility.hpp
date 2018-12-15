#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <algorithm>
#include <tuple>
#include <utility>
#include <type_traits>

namespace Detail {
	using std::declval;

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
using Value_t = typename ValueType<RECURSE, T...>::value_type;
template<bool RECURSE, class T>
struct ValueType<RECURSE, T, Detail::Void_t<typename T::value_type>> {
	typedef std::conditional_t<RECURSE,
			Value_t<RECURSE, typename T::value_type>,
			typename T::value_type> value_type;
};

// usage: get_value_type<RECURSE>(Tag<S> + Tag<T...>),
//        get_value_type<RECURSE, S, T...>(), etc.
template<bool RECURSE, class S, class... T>
auto get_value_type(Detail::Tag<S, T...> = {}, std::integral_constant<bool, RECURSE> = {})
	-> Value_t<RECURSE, S, T...> { return {}; }

// usage: get_value_type<RECURSE>(s, t...)
template<bool RECURSE, class S, class... T>
auto get_value_type(S && s, T &&... t)
	-> Value_t<RECURSE, S, T...> { return {}; }

template<bool RECURSE, class... T>
using CommonValue_t = std::common_type_t<Value_t<RECURSE, T>...>;

template<class S, class T>
using ConvertibleFrom_t = std::enable_if_t<std::is_convertible<T,S>::value, T>;
template<class S, class T>
using ConvertibleTo_t = std::enable_if_t<std::is_convertible<T,S>::value, S>;

template<class S, class... T> struct SumType { typedef S value_type; };
template<class R, class S, class... T> struct SumType<R,S,T...> {
	using value_type = decltype(declval<R>()
		+ declval<Value_t<false, SumType<S,T...>>>());
};
template<class S, class... T> struct DifferenceType { typedef S value_type; };
template<class R, class S, class... T> struct DifferenceType<R,S,T...> {
	using value_type = decltype(declval<R>()
		* declval<Value_t<false, DifferenceType<S,T...>>>());
};
template<class S, class... T> struct ProductType { typedef S value_type; };
template<class R, class S, class... T> struct ProductType<R,S,T...> {
	using value_type = decltype(declval<R>()
		* declval<Value_t<false, ProductType<S,T...>>>());
};
template<class S, class... T> struct QuotientType { typedef S value_type; };
template<class R, class S, class... T> struct QuotientType<R,S,T...> {
	using value_type = decltype(declval<R>()
		/ declval<Value_t<false, QuotientType<S,T...>>>());
};

template<bool R, class... S> struct SumValue {
	using value_type = Value_t<false, SumType<Value_t<R, S>...>>;
};
template<bool R, class... S> struct DifferenceValue {
	using value_type = Value_t<false, DifferenceType<Value_t<R, S>...>>;
};
template<bool R, class... S> struct ProductValue {
	using value_type = Value_t<false, ProductType<Value_t<R, S>...>>;
};
template<bool R, class... S> struct QuotientValue {
	using value_type = Value_t<false, QuotientType<Value_t<R, S>...>>;
};

template<class... S>
using SumType_t = Value_t<false, SumType<S...>>;
template<class... S>
using DifferenceType_t = Value_t<false, DifferenceType<S...>>;
template<class... S>
using ProductType_t = Value_t<false, ProductType<S...>>;
template<class... S>
using QuotientType_t = Value_t<false, QuotientType<S...>>;
template<bool R, class... S>
using SumValue_t = Value_t<false, SumValue<R, S...>>;
template<bool R, class... S>
using DifferenceValue_t = Value_t<false, DifferenceValue<R, S...>>;
template<bool R, class... S>
using ProductValue_t = Value_t<false, ProductValue<R, S...>>;
template<bool R, class... S>
using QuotientValue_t = Value_t<false, QuotientValue<R, S...>>;

}

#endif
