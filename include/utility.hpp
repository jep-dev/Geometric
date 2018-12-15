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
template<class S, class... T> struct DifferenceType { typedef S value_type; };
template<class S, class... T> struct ProductType { typedef S value_type; };
template<class S, class... T> struct QuotientType { typedef S value_type; };
template<bool RECURSE, class S, class... U>
struct SumValueType { typedef Value_t<RECURSE, S> value_type; };
template<bool RECURSE, class S, class... U>
struct DifferenceValueType { typedef Value_t<RECURSE, S> value_type; };
template<bool RECURSE, class S, class... U>
struct ProductValueType { typedef Value_t<RECURSE, S> value_type; };
template<bool RECURSE, class S, class... U>
struct QuotientValueType { typedef Value_t<RECURSE, S> value_type; };

//using SumType_t = typename SumType<T...>::value_type;
//using DifferenceType_t = typename DifferenceType<T...>::value_type;
//using ProductType_t = typename ProductType<T...>::value_type;
//using QuotientType_t = typename QuotientType<T...>::value_type;
//using SumValue_t = typename SumValueType<R, T...>::value_type;
//using DifferenceValue_t = typename DifferenceValueType<R, T...>::value_type;
//using ProductValue_t = typename ProductValueType<R, T...>::value_type;
//using QuotientValue_t = typename QuotientValueType<R, T...>::value_type;

template<bool R, class... T>
using SumValue_t = Value_t<false, SumValueType<R, T...>>;
template<class... T>
using SumType_t = Value_t<false, SumType<T...>>;
template<bool R, class... T>
using DifferenceValue_t = Value_t<false, DifferenceValueType<R, T...>>;
template<class... T>
using DifferenceType_t = Value_t<false, DifferenceType<T...>>;
template<bool R, class... T>
using ProductValue_t = Value_t<false, ProductValueType<R, T...>>;
template<class... T>
using ProductType_t = Value_t<false, ProductType<T...>>;
template<bool R, class... T>
using QuotientValue_t = Value_t<false, QuotientValueType<R, T...>>;
template<class... T>
using QuotientType_t = Value_t<false, QuotientType<T...>>;

// template<bool R, class... T> using DifferenceValue_t = Value_t<R, DifferenceType<T...>>;
// template<class... T> using DifferenceType_t = DifferenceValue_t<false, T...>;
// template<bool R, class... T> using ProductValue_t = Value_t<R, ProductType<T...>>;
// template<class... T> using ProductType_t = ProductValue_t<false, T...>;
// template<class... T>
// using QuotientType_t = Value_t<false, QuotientType<T...>>;
// template<bool R, class... T>
// using SumValue_t = Value_t<false, SumValueType<R, T...>>;
// template<bool R, class... T>
// using DifferenceValue_t = Value_t<false, DifferenceValueType<R, T...>>;
// template<bool R, class... T>
// using ProductValue_t = Value_t<false, ProductValueType<R, T...>>;
// template<bool R, class... T>
// using QuotientValue_t = Value_t<false, QuotientValueType<R, T...>>;

template<class S, class T, class... U> struct SumType<S, T, U...> {
	typedef decltype(declval<S>() + declval<SumType_t<T, U...>>()) value_type;
};
template<bool R, class S, class T, class... U>
struct SumValueType<R, S, T, U...> {
	typedef decltype(declval<Value_t<R, S>>()
		+ declval<Value_t<R, T, U...>>()) value_type;
};

template<class S, class T, class... U> struct DifferenceType<S, T, U...> {
	typedef decltype(declval<S>() - declval<DifferenceType_t<T, U...>>()) value_type;
};

template<bool R, class S, class T, class... U>
struct DifferenceValueType<R, S, T, U...>
	{ typedef decltype(declval<Value_t<R, S>>()
		- declval<Value_t<R, T, U...>>()) value_type; };

template<class S, class T, class... U> struct ProductType<S, T, U...> {
	typedef decltype(declval<S>() * declval<ProductType_t<T, U...>>()) value_type;
};
template<bool R, class S, class T, class... U>
struct ProductValueType<R, S, T, U...> {
	typedef decltype(declval<Value_t<R, S>>()
		* declval<Value_t<R, T, U...>>()) value_type;
};

template<class S, class T, class... U> struct QuotientType<S, T, U...> {
	typedef decltype(declval<S>() / declval<QuotientType_t<T, U...>>()) value_type;
};
template<bool R, class S, class T, class... U>
struct QuotientValueType<R, S, T, U...> {
	typedef decltype(declval<Value_t<R, S>>()
		/ declval<Value_t<R, T, U...>>()) value_type;
};

}

#endif
