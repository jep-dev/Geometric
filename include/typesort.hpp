#ifndef TYPESORT_HPP
#define TYPESORT_HPP

#include <limits>

#include "functional.hpp"
#include "tag.hpp"
#include "utility.hpp"
#include <type_traits>

namespace Detail {

enum EDim : unsigned char {
	e_width = 0, e_height, e_depth, en_dims
};

template<class C, class S, S U> struct Comparable;
template<class C, class S, S U> struct Comparable: Tag<C, Seq<S, U>> {
	typedef S key_type;
	static constexpr S key_value = U;

	template<class D, S V> using lesser =
		std::conditional_t<(U < V), C, D>;
	template<class D, S V> using lesser_comparable =
		Comparable<lesser<D, V>, S, (U < V ? U : V)>;

	/*template<class D, S V> using greater = typename Comparable<D, S, V>::lesser<C, U>;
	template<class D, S V> using greater_comparable =
		typename Comparable<D, S, V>::lesser_comparable<C, U>;*/
	template<class D, S V> using greater =
		std::conditional_t<(U > V), C, D>;
	template<class D, S V> using greater_comparable =
		Comparable<greater<D, V>, S, (U > V ? U : V)>;


	// Comparison of comparables is comparison of keys
	template<class D, S V> constexpr bool
	operator<(Comparable<D, S, V> const&) const { return U < V; }
	template<class D, S V> constexpr bool
	operator==(Comparable<D, S, V> const&) const { return U == V; }
	template<class D, S V> constexpr bool
	operator>(Comparable<D, S, V> const&) const { return U > V; }
	template<class D, S V> constexpr bool
	operator!=(Comparable<D, S, V> const&) const { return U != V; }

	constexpr Comparable(void) {}
};

template<class L, class C, class R>
struct Inserting;
template<class... L, class C, class... R>
struct Inserting<Tag<L...>, C, Tag<R...>>: Tag<L..., C> {
	typedef Tag<L..., C> type;
};
template<class... L, class C, class R0, class... R>
struct Inserting<Tag<L...>, C, Tag<R0, R...>> {
	static constexpr decltype(C::key_value) ckey = C::key_value;
	static constexpr decltype(R0::key_value) rkey = R0::key_value;
	typedef typename Inserting<Tag<L..., std::conditional_t<(ckey < rkey), C, R0>>,
		std::conditional_t<(ckey < rkey), R0, C>, Tag<R...>>::type type;
};
template<class C, class GE> using Insert = Inserting<Tag<>, C, GE>;
template<class C, class GE> using Insert_t = typename Insert<C, GE>::type;

template<class L, class K, class R = Tag<>, template<class...> class CMP = std::is_same>
struct Removing: R { typedef R type; };
template<class L0, class... L, class K, class... R, template<class...> class CMP>
struct Removing<Tag<L0, L...>, K, Tag<R...>, CMP>: Removing<Tag<L...>, K,
		std::conditional_t<CMP<L0, K>::value, Tag<R...>, Tag<R..., L0>>>::type {
	typedef typename Removing<Tag<L...>, K,
		std::conditional_t<CMP<L0, K>::value, Tag<R...>, Tag<R..., L0>>>::type type;
};

template<class L, class K, template<class...> class CMP = std::is_same>
using Remove = Removing<L, K, Tag<>, CMP>;
template<class L, class K, template<class...> class CMP = std::is_same>
using Remove_t = typename Remove<L, K, CMP>::type;

template<std::size_t N, class... L, class... R, class V = std::enable_if_t<!N, void>>
constexpr Tag<R...> head(SeqSz<N>, Tag<L...>, Tag<R...>, V = {}) { return {}; }

template<std::size_t N, class L0, class... L, class... R, class V = std::enable_if_t<N, void>>
constexpr auto head(SeqSz<N>, Tag<L0, L...>, Tag<R...>, V = {})
-> decltype(head<N>(Tag<L...>{}, Tag<R..., L0>{})) { return {}; }

template<std::size_t N, class... L, class = std::enable_if_t<(N < sizeof...(L)-1), void>>
constexpr auto tail(SeqSz<N>, Tag<L...> V = {})
-> decltype(head<N>(V >> SeqL<N>{})) { return {}; }

template<class... L, class... R>
constexpr bool match_head(Tag<L...>, Tag<R...>) { return false; }
template<class... L, class... R>
constexpr bool match_head(Tag<L...>, Tag<L..., R...>) { return true; }

template<class... L, class... R>
constexpr bool match_tail(Tag<L...> l, Tag<R...> r) { return match_head(reverse(l), reverse(r)); }

template<class... L, class... R>
constexpr Tag<R...> remove_head(Tag<L...>, Tag<R...>) { return {}; }
template<class... L, class... R>
constexpr Tag<R...> remove_head(Tag<L...>, Tag<L..., R...>) { return {}; }

template<class... L, class... R>
constexpr auto remove_tail(Tag<L...> l, Tag<R...> r)
-> decltype(reverse(remove_head(reverse(l), reverse(r)))) { return {}; }

/*template<std::size_t N>
constexpr auto pop_head(Tag<>) -> Tag<> { return {}; }
template<std::size_t N, class S, class... T>
constexpr auto pop_head(Tag<S, T...>)
	-> decltype(pop_head<N-1>(Tag<T...>{})) { return {}; }

template<class... L, class... R>
constexpr auto split_head(SeqSz<0>, Tag<L...>, Tag<R...>)
	-> Tag<Tag<R...>, Tag<L...>> { return {}; };
template<std::size_t N, class L0, class... L, class... R>
constexpr auto split_head(SeqSz<N>, Tag<L0, L...>, Tag<R...>)
	-> decltype(split_head(SeqSz<N-1>{}, Tag<L...>{}, Tag<R..., L0>{})) { return {}; }
template<std::size_t N, class... L>
constexpr auto split_head(SeqSz<N>, Tag<L...>)
	-> decltype(split_head(SeqSz<N>{}, Tag<L...>{}, Tag<>{})) { return {}; }

template<std::size_t N, class... L>
constexpr auto split_tail(SeqSz<N> s, Tag<L...> l)
	-> decltype(reverse(split_head(s, l >> SeqL<long(N)>{}))) { return {}; }*/


/*template<class C, class R>
struct Insert;
template<class C, class... R>
struct Insert<C, Tag<R...>>: InsertionSort<Tag<>, C, Tag<R...>>::type {
	typedef typename InsertionSort<Tag<>, C, Tag<R...>>::type type;
};
template<class C, class R>
using Insert_t = typename Insert<C, R>::type;*/

/*
template<long W>
struct Width;
template<long W>
struct Width: Comparable<EDim, e_width, Width<W>> {};
// static constexpr auto w = make_width<1024>() = Width<1024>{}
template<long H>
struct Height;
template<long H>
struct Height: Comparable<EDim, e_height, Height<H>> {};
// static constexpr auto h = make_height<768>() = Height<768>{}
// static_assert(is_same<typename Width::lesser_type<e_
*/

}

#endif
