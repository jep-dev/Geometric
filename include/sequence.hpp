#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

#include "utility.hpp"

namespace Detail {

/** A variadic unsigned sequence, useful for indexing Tag/Val. */
template<class T = void, T... I> struct Seq {};

// Enable more if you need them.
template<char... I> using SeqC = Seq<char, I...>;
//template<short... I> using SeqS = Seq<short, I...>;
template<unsigned... I> using SeqU = Seq<unsigned, I...>;
template<int... I> using SeqI = Seq<int, I...>;
template<long... I> using SeqL = Seq<long, I...>;
//template<unsigned char... I> using SeqUC = Seq<unsigned char, I...>;
//template<unsigned short... I> using SeqUS = Seq<unsigned short, I...>;
//template<unsigned long... I> using SeqUL = Seq<unsigned long, I...>;

/* Removed make_seq(), especially make_seq(void) for now:
 * make_seq determines the T in Seq<T,...> from its arguments, so without arguments,
 * the type should be undefined.
 * Given T, e.g.
 *     makeSeqU(...) -> SeqU<...>; // OK even if length is zero
 * behavior around the empty set is poorly defined, e.g.
 *     SeqI<> + SeqC<...> = SeqI<...> // SeqI<> carries type int; int + char = int; or
 *     SeqI<> + SeqC<...> = SeqC<...> // SeqI<> is memberless only int-related in name; or
 * or undefined in general, which might suggest Seq*<> should be undefined after all.
 */

template<class TI, unsigned... I, class TJ, unsigned... J,
		class T = SumType_t<TI, TJ>>
constexpr auto operator+(Seq<TI, I...>, Seq<TJ, J...>) -> Seq<T, I+J...> { return {}; }

template<class TI, unsigned... I, class TJ, unsigned... J,
		class T = SumType_t<TI, TJ>>
constexpr auto operator-(Seq<TI, I...>, Seq<TJ, J...>) -> Seq<T, I-J...> { return {}; }

template<class TI, unsigned... I, class TJ, unsigned... J,
		class T = ProductType_t<TI, TJ>>
constexpr auto operator*(Seq<TI, I...>, Seq<TJ, J...>) -> Seq<T, I*J...> { return {}; }

template<class TI, unsigned... I, class TJ, unsigned... J,
		class T = QuotientType_t<TI, TJ>>
constexpr auto operator/(Seq<TI, I...>, Seq<TJ, J...>) -> Seq<T, I/J...> { return {}; }

/** Defines value as the product of all of {IN...}.
 * Not to be confused with inner product, etc. */
//template<class T, T... IN> struct ProductSeq;
template<class T, T I, T... IN> struct ProductSeq:
	std::integral_constant<T, I> {};
template<class T, T I, T I0, T... IN> struct ProductSeq<T, I, I0, IN...>:
	std::integral_constant<T, ProductSeq<T, I*I0, IN...>::value> {};

}

#endif
