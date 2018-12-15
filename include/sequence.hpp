#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

#include "utility.hpp"
#include "geometric.hpp"

namespace Detail {

/** A variadic unsigned sequence, useful for indexing Tag/Val. */
template<class T = void, T... I> struct Seq {
	typedef T type;
};
template<class T, T I0, T... IN> struct Seq<T, I0, IN...> {
	typedef T type;
	template<std::size_t N>
	static constexpr T get(std::integral_constant<std::size_t, N> = {})
		{ return Seq<T, IN...>::get(std::integral_constant<std::size_t, N-1>{}); }
	static constexpr T get(std::integral_constant<std::size_t, 0>)
		{ return I0; }
	template<std::size_t N>
	static constexpr T get(Seq<std::size_t, N>)
		{ return std::integral_constant<std::size_t, get<N>()>::value; }

	template<std::size_t N>
	using nth_type = Seq<T, get<N>()>;
};

template<std::size_t N, class T, T... I>
constexpr T get(Seq<T, I...>)
	{ return Seq<T, I...>::get(Seq<std::size_t, N>{}); }



template<class T> struct SeqArray;
template<class T, T... I> struct SeqArray<Seq<T, I...>> {
	static constexpr T value[sizeof...(I)] = {I...};
};
template<class T, T... I>
constexpr T SeqArray<Seq<T, I...>>::value [sizeof...(I)];

// Enable more if you need them.
template<char... I> using SeqC = Seq<char, I...>;
//template<short... I> using SeqS = Seq<short, I...>;
template<unsigned... I> using SeqU = Seq<unsigned, I...>;
template<int... I> using SeqI = Seq<int, I...>;
template<long... I> using SeqL = Seq<long, I...>;
template<std::size_t... I> using SeqSz = Seq<std::size_t, I...>;
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
struct PartialSumSeq: PartialSumSeq<Value_t<true, T>> {
	using PartialSumSeq<Value_t<true, T>>::value_type;
};
template<class T, T I0, T I1, T... IN>
struct PartialSumSeq<Seq<T, I0, I1, IN...>>:
		decltype(prepend<T, I0>(PartialSumSeq<Seq<T, I0+I1, IN...>>{})) {
	// I hate repeating myself, but my usual method (adding a trailing default template arg
	// and using it as a superclass and a member) doesn't work with specializations.
	typedef Value_t<false, decltype(prepend<T, I0>(
		PartialSumSeq<Seq<T, I0+I1, IN...>>{}))> value_type;
};

/** @brief Defines value as the product of all of {I0, IN...}.
 * Not to be confused with inner product, etc. */
template<class S> struct ProductSeq;
template<class S, S I0>
struct ProductSeq<Seq<S, I0>>: std::integral_constant<S, I0> {};
template<class S, S I0, S I1, S... IN>
struct ProductSeq<Seq<S, I0, I1, IN...>>: ProductSeq<Seq<S, I0*I1, IN...>> {};

template<unsigned N, class S = unsigned, S A = 1, S B = 1, S... C>
struct Fibonacci: Fibonacci<N-1, S, A+B, A, B, C...> {};
template<class S, S A, S B, S... C>
struct Fibonacci<0, S, A, B, C...>: std::integral_constant<S, A> {};



template<class S, S A, S B>
struct LexSortConstant {
	using value_type = S;
	// static constexpr int value = A < B;
	static constexpr bool lt = A<B, eq = A==B, gt = A>B, le = A<=B, ge = A>=B;
	static constexpr S first = lt ? A : B, second = lt ? B : A;
};

/*template<class S, class R = std::false_type>
constexpr auto get_value(S && s, R && r = {})
	-> Value_t<R::value, S> { return {}; }*/

/*
 * remove_reference_t and remove_const_t fail to handle arrays
 * Replace them with a series of specializations:
 *
 * <S> RemoveRef_t<S> = S
 * <S> RemoveRef_t<S&> = S
 * <S> RemoveRef_t<S const&> = S
 * <S,N> RemoveRef_t<S [N]> = S [N]
 * <S,N> RemoveRef_t<const S (&) [N]> = S [N]
 * <S,N> RemoveRef_t<S (&&) [N]> = S [N]
 * Don't recursively apply RemoveRef_t (as in, to the S in the array expressions).
 */

template<class S, class T, std::size_t N = 0> struct Find;

template<class S, std::size_t N> struct Find<S, Tag<>, N> {};
template<class S, class... TN, std::size_t N>
struct Find<S, Tag<S, TN...>, N>:
	std::integral_constant<std::size_t, N> {};
template<class S, std::size_t N, class T0, class... TN>
struct Find<S, Tag<T0, TN...>, N>:
	std::conditional_t<std::is_same<S, T0>::value,
		std::integral_constant<std::size_t, N>,
		Find<S, Tag<TN...>, N+1>> {};

/*template<class S, class T, std::size_t N = 0>
struct Find;
template<class S, class... T, std::size_t N>
struct Find<S, Tag<S, T...>, N>:
	std::integral_constant<std::size_t, N> {};
template<class S, class T0, class... TN, std::size_t N>
struct Find<S, Tag<T0, TN...>, N>:
	Find<S, Tag<TN...>, N+1> {};*/

/*template<class S, class T> struct Find;
// template<class S, S I, class T> struct FindSeq;
template<class S, class T> struct Find: Find<S, Value_t<false, T>> {};


// template<class S, S I, S... TI>
// struct FindSeq<S, I, Seq<S, I, TI...>>: std::integral_constant<S, I> {};
template<class S, class... T>
struct Find<S, Tag<S, T...>>: Tag<S> {};

template<class S, S T0, S... TI, S I>
struct FindSeq<S, I, Seq<S, T0, TI...>>: FindSeq<S, I, Seq<S, TI...>> {};
template<class S, class T0, class... TN>
struct Find<S, Tag<T0, TN...>>: Find<S, Tag<TN...>> {};*/


/*
 * Note - inserting at point N, popping N off the head, reversing all N elements, etc.
 * incur linear costs. Something like sorting that could be done in O(N log N) may only be
 * possible in O(N^2) without O(1) splicing, random access, etc.
 *
 * Assume that's correct, sorting is O(N^2).
 * If you have two sequences a0<...<aM and b0<...<bN, you can merge sort in O(M+N).
 * Sorting A and B independently then merge sorting is O(M^2 + N^2 + M + N) = O(M^2 + N^2).
 * If A and B are unsorted, merging A and B is just sorting the concatenation of both,
 * so that's O((M + N)^2) = O(M^2 + N^2). 
 * It does hint that the average case is significantly better with conventional
 */

/* Can you represent a deque?
Rev({}) = {}
Rev({a, b ...}) = Rev({b ...}) + {a}
		= {[z] ... b, a}
// Rev({a, b, c ...}) = Rev({c ...}) + {b, a}
// Rev({a, b, c, d ...}) = Rev({d ...}) + {c, b, a}
// etc. - you don't need the commented lines, but
// these decrease the average case performance (stack size).
*/

/*template<class S, S A>
auto reverse(Seq<S, A> = {}) -> Seq<S, A> { return {}; }
template<class S, S A, S B, S... C>
auto reverse(Seq<S, A, B, C...> = {})
	-> decltype(reverse(Seq<S, B, C...>{}) + Seq<S,A>{}) { return {}; }

// template<template<class, class> class CMP>
// auto reverse(Tag<> = {}, Tag<CMP> = {}) -> Tag<> { return {}; }


// template<class S, S A, S... B>
// struct RevSeq: decltype(reverse<S, A, B...>()) {};
*/

/*
Pop<0>({a ...}) = {a ...}
Pop<N>({a, b ...}) = Pop<N-1>({b ...})
// Pop<3>({abcdef})
// Pop<2>({bcdef})
// Pop<1>({cdef})
// {def}

Head<0>({a ...}) = {}
Head<N>({a, b ...}) = {a} + Head<N-1>(Pop<1>({b ...}))
// Head<3>({abcdef})
// {a}+Head<2>(Pop<1>({abcdef}))
// {a}+Head<2>({bcdef})
// {a}+{b}+Head<1>(Pop<1>({bcdef}))
// {ab}+Head<1>({cdef})
// {ab}+{c}
// {abc}

RevHead<0>({a ...}) = {}
RevHead<N>({a, b ...}) = RevHead<N-1>({b ...}) + {a}
// RevHead<N>({a ...}) = Rev(Head<N>({a ...}))
//
// RH<3>({abcdef})
// RH<2>({bcdef})+{a}
// RH<1>({cdef})+{b}+{a}
// RH<0>({def})+{c}+{ba}
// {}+{cba}

make_tail_seq<N>({a ...}) = make_head_seq<N>(make_rev_seq({a...}))
make_rev_tail_seq<N>({a ...}) = make_rev_seq(make_tail_seq<N>({a...}))


Now, if a deque of N elements should be split at point M,
{I...} -> {RevHead<N>({I...}), Pop<N>({I...})
   .-[N-1, ..., 0] = first_type = RevHead<N>({I...})
   '--[N, ..., M-1] = second_type = Pop<N>({I...})
template<class S, S... A, S... B>
DequeSeq<Seq<S, A...>, Seq<S, B...>> { ... }

template<int N, class S, S... A>
make_head_seq(Seq<S, A...> a = {})
	-> Head<N, S, A...> { ... }
template<int N, class S, S... A>
make_rev_head_seq(Seq<S, A...> a = {})
	-> RevHead<N, S, A...> { ... }
template<int N, class S, S... A>
make_deque_seq(Seq<S, A...> a = {})
	-> Deque<Value_t<RevHead<N,S,A...>>, Value_t<Pop<N,S,A...>>>

template<int N, class S, S... A, S... B>
shift(Deque<Seq<S,A...>,Seq<S,B...>> = {})

template<int N, class S, S... A, S... B>
shift(Deque<Seq<S,A...>,Seq<S,B...>> = {})
-> conditional_t<N >= 0,
	Deque<Pop<N,S,A...>,
		decltype(RevHead<N,S,A...>{}+Seq<S,B...>{})>,
	Deque<decltype(RevHead<N,S,B...>{}+Seq<S,A...>{}),
		Pop<N,S,B...>>
> { ... }

If a queue of N elements should be split at point M,
   .-[0, ..., N-1] = first_type = Head<N>({I...})
   '--[N, ..., M-1] = second_type = Pop<N>({I...})

*/



}

#endif
