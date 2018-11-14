#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

#include "utility.hpp"

namespace Detail {

/** A variadic unsigned sequence, useful for indexing Tag/Val. */
template<class T = void, T... I> struct Seq { typedef T type; };
template<class T, T T0, T... TN> struct Seq<T, T0, TN...> {
	typedef T type;
	template<unsigned I>
	static constexpr T nth(std::integral_constant<unsigned, I> = {}) {
		return Seq<T, TN...>::nth(std::integral_constant<unsigned, I-1>{});
	}
	static constexpr T nth(std::integral_constant<unsigned, 0>) {
		return T0;
	}
};

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

/** Prepends a value to a sequence. */
template<class T, T I0, T... IN>
constexpr auto prepend(Detail::Seq<T, IN...> = {})
	-> Detail::Seq<T, I0, IN...> { return {}; }
/** Appends a value to a sequence (same as prepend in reverse order) */
template<class T, T I0, T... IN>
constexpr auto append(Detail::Seq<T, IN...> = {})
	-> Detail::Seq<T, IN..., I0> { return {}; }
/** Prepends a sequence to another. */
template<class T, T... I0, T I1, T... IN>
constexpr auto prepend(Detail::Seq<T, I0...> = {}, Detail::Seq<T, I1, IN...> = {})
	-> Detail::Seq<T, I0..., I1, IN...> { return {}; }
/** Appends a sequence to another. */
template<class T, T... I0, T I1, T... IN>
constexpr auto append(Detail::Seq<T, I0...> = {}, Detail::Seq<T, I1, IN...> = {})
	-> Detail::Seq<T, I1, IN..., I0...> { return {}; }

template<class T, T... IN>
constexpr std::size_t getSize(Detail::Seq<T, IN...> = {}) { return sizeof...(IN); }
/** Pairwise sum of sequences */
template<class TI, TI... I, class TJ, TJ... J,
		class T = SumType_t<TI, TJ>>
constexpr auto operator+(Seq<TI, I...>, Seq<TJ, J...>) -> Seq<T, I+J...> { return {}; }

/** Pairwise difference of sequences */
template<class TI, TI... I, class TJ, TJ... J,
		class T = DifferenceType_t<TI, TJ>>
constexpr auto operator-(Seq<TI, I...>, Seq<TJ, J...>) -> Seq<T, I-J...> { return {}; }

/** Pairwise product of sequences */
template<class TI, TI... I, class TJ, TJ... J,
		class T = ProductType_t<TI, TJ>>
constexpr auto operator*(Seq<TI, I...>, Seq<TJ, J...>) -> Seq<T, I*J...> { return {}; }

/** Pairwise division of sequences */
template<class TI, TI... I, class TJ, TJ... J,
		class T = QuotientType_t<TI, TJ>>
constexpr auto operator/(Seq<TI, I...>, Seq<TJ, J...>) -> Seq<T, I/J...> { return {}; }

template<class OS, class S, S... I>
OS& operator<<(OS &os, Detail::Seq<S, I...> const&) {
	bool first = true;
	os << '{';
	for(auto i : {I...}) {
		if(!first) os << ", ";
		first = false;
		os << i;
	}
	os << '}';
	return os;
}

/** @brief Defines value as the sum of all {I0, IN...}.
 * Not to be confused with pairwise sum, etc. */
template<class S> struct SumSeq;
template<class S, S I0>
struct SumSeq<Seq<S, I0>>: std::integral_constant<S, I0> {};
template<class S, S I0, S I1, S... IN>
struct SumSeq<Seq<S, I0, I1, IN...>>: SumSeq<Seq<S, I0+I1, IN...>> {};

/** @brief Represents N copies of I as a Seq. */
template<unsigned N, class T, T I>
struct RepeatSeq: decltype(prepend<T, I>(typename RepeatSeq<N-1, T, I>::value_type{})) {
	typedef typename RepeatSeq<N-1, T, I>::value_type prev_type;
	typedef decltype(prepend<T, I>(prev_type{})) value_type;
	constexpr operator value_type(void) const { return {}; }
};
template<class T, T I>
struct RepeatSeq<0, T, I> {};
template<class T, T I>
struct RepeatSeq<1, T, I>: Seq<T, I> {
	typedef Seq<T, I> value_type;
	constexpr operator value_type(void) const { return {}; }
};

/** @brief Represents the partial sum of a Seq. */
template<class T> struct PartialSumSeq;
template<class T, T I0>
struct PartialSumSeq<Seq<T, I0>>: Seq<T, I0> {
	typedef Seq<T, I0> value_type;
};
template<class T>
struct PartialSumSeq: PartialSumSeq<ValueType_t<true, T>> {
	using PartialSumSeq<ValueType_t<true, T>>::value_type;
};
template<class T, T I0, T I1, T... IN>
struct PartialSumSeq<Seq<T, I0, I1, IN...>>:
		decltype(prepend<T, I0>(PartialSumSeq<Seq<T, I0+I1, IN...>>{})) {
	// I hate repeating myself, but my usual method (adding a trailing default template arg
	// and using it as a superclass and a member) doesn't work with specializations.
	typedef ValueType_t<false, decltype(prepend<T, I0>(
		PartialSumSeq<Seq<T, I0+I1, IN...>>{}))> value_type;
};

/** @brief Defines value as the product of all of {I0, IN...}.
 * Not to be confused with inner product, etc. */
template<class T, T I0 = 1, T... IN>
struct ProductSeq: std::integral_constant<T, I0> {};
template<class T, T I0, T I1, T... IN>
struct ProductSeq<T, I0, I1, IN...>: ProductSeq<T, I0*I1, IN...> {};

}

#endif
