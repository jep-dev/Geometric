#ifndef GEOMETRIC_HPP
#define GEOMETRIC_HPP

#include <type_traits>

template<class...> struct Tag;
template<class...> struct Tag {
	typedef Tag<> head_type;
	typedef Tag<> tail_type;
	static constexpr unsigned size = 0;
};
template<class HEAD, class... REST> struct Tag<HEAD, REST...> {
	typedef Tag<HEAD> head_type;
	typedef Tag<REST...> tail_type;
	static constexpr unsigned size = 1 + sizeof...(REST);
};

/** Direct sum (treats LHS and RHS as orthogonal) */
template<class... LHS, class... RHS>
Tag<LHS..., RHS...> operator+(Tag<LHS...>, Tag<RHS...>) { return {}; }

/** Inner product */
template<class... LN, class... RN>
Tag<Tag<LN,RN>...> operator*(Tag<LN...>, Tag<RN...>) { return {}; }

/** Outer product; |LHS|*|RHS|=0 -> |LHS*RHS|=0 */
template<class... LN, class... RN>
Tag<> operator^(Tag<LN...>, Tag<RN...>) { return {}; }

/** Outer product; |LHS|=1 and |RHS|>1 */
template<class L, class... RN>
Tag<Tag<L, RN>...> operator^(Tag<L> lhs, Tag<RN...> rhs) { return {}; }

/** Outer product; |LHS|>1 and |RHS|=1 */
template<class... LN, class R>
Tag<Tag<LN, R>...> operator^(Tag<LN...> lhs, Tag<R> rhs) { return {}; }

/** Outer product; |LHS|>1 and |RHS|>1 */
template<class L0, class L1, class... LN, class R0, class R1, class... RN>
auto operator^(Tag<L0, L1, LN...> lhs, Tag<R0, R1, RN...> rhs)
-> Tag<decltype(Tag<L0>{} ^ rhs), decltype(Tag<L1>{} ^ rhs),
	decltype(Tag<LN>{} ^ rhs)...> { return {}; }


#endif
