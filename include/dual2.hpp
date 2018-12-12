#ifndef DUAL2_HPP
#define DUAL2_HPP

#include "utility.hpp"
#include "sequence.hpp"

namespace Dual2 {
using namespace Detail;
using std::is_scalar;
using std::is_class;
using std::enable_if_t;
using std::is_convertible;
using std::is_default_constructible;
using std::is_constructible;
template<class T, T V>
using Constexpr = std::integral_constant<T, V>;

template<class S> static constexpr bool isScalar(Tag<S> = {}) { return is_scalar<S>::value; }
template<class S> static constexpr bool isClass(Tag<S> = {}) { return is_class<S>::value; }

template<class S, class = void>
struct Wrapper: S {
	typedef S value_type;

	operator S & (void) { return static_cast<S&>(*this); }
	operator S const& (void) const { return static_cast<S const&>(*this); }

	Wrapper(void): S() {}
	Wrapper(S const& s): S(s) {}
	Wrapper(S && s): S(std::move(s)) {}
	template<class... T> Wrapper(T &&... t):
			S{std::forward<T>(t)...} {}
	virtual ~Wrapper(void) {}
};

template<class S>
struct Wrapper<S, enable_if_t<isScalar<S>(), void>> {
	typedef S value_type;
	S value;

	operator S & (void) { return value; }
	operator S const& (void) const { return value; }

	Wrapper(void): value{} {}
	template<class... T> Wrapper(T &&... t):
			value(std::forward<T>(t)...) {}
	virtual ~Wrapper(void) {}
};

template<class S> S && unwrap(S && w) { return w; }
template<class S> S & unwrap(S & w) { return w; }
template<class S> S const& unwrap(S const& w) { return w; }
template<class S> S && unwrap(Wrapper<S> && w) { return w; }
template<class S> S & unwrap(Wrapper<S> & w) { return w; }
template<class S> S const& unwrap(Wrapper<S> const& w) { return w; }

template<class S> auto wrap(S s, int)
		-> std::enable_if_t<isScalar<S>(), Wrapper<S>> { return Wrapper<S>(s); }
template<class S> auto wrap(S && s, long) -> S && { return std::move(s); }
template<class S> decltype(auto) wrap(S && s) { return wrap(std::forward<S>(s), 0); }
// To prevent nesting...
// template<class S> Wrapper<S> && wrap(Wrapper<S> && s) { return std::move(s); }
// ... but the intention is generally to use Wrapper as an intermediate type/value.

template<class S, class T0 = char, class... TN> auto zero(T0 && t0 = 0, TN &&... tn)
	{ return Wrapper<S>(std::forward<T0>(t0), std::forward<TN>(tn)...); }
template<class S, class T0 = char, class... TN> auto one(T0 && t0 = 1, TN &&... tn)
	{ return Wrapper<S>(std::forward<T0>(t0), std::forward<TN>(tn)...); }

template<char C = '\0', int POW = 1>
struct Unit;
template<char C, int POW>
struct Unit {
	static constexpr char label = C;
	static constexpr int total = POW, power = total & 1;
	template<int N> constexpr auto operator^(SeqI<N>) const
			-> Unit<C, (power * N) & 1> { return {}; }
	template<int N> constexpr auto operator^(std::integral_constant<int, N>) const
			-> decltype(*this ^ SeqI<N>{}) { return {}; }
	constexpr Unit(void) {}
};

template<int SQ, int CO, class U> struct Units;
template<int SQ, int CO, char... C, int... POW>
struct Units<SQ, CO, Tag<Unit<C,POW>...>> {
	static constexpr int square = SQ,
			coefficient = SumSeq<SeqI<((POW&2) ? SQ : 1)...>>::value;
	using value_type = Units<square, coefficient, Tag<Unit<C,POW & 1>...>>;
	template<char C1, int POW1>
	constexpr auto operator*(Unit<C1, POW1>)
		-> Units<SQ, CO * ((POW1 & 2) ? SQ : 1),
			Tag<Unit<C,POW>..., Unit<C1, POW1>>> { return {}; }
};

template<int SQ, int CO, char C1, char... C, int POW1, int... POW>
constexpr auto operator*(Unit<C1, POW1> const&,
		Units<SQ, CO, Unit<C,POW>...> const&)
	-> Units<SQ, CO * ((POW1 & 2) ? SQ : 1),
		Unit<C1, POW1>, Unit<C,POW>...> { return {}; }

template<int SQ = -1, int CO = 1, char... C, class... T>
Units<SQ, CO, Tag<Unit<C, 1>...>>
units(SeqC<C...> = {}, T...) { return {}; }

template<int SQ = -1, int CO = 1, char... C, int... POW>
Units<SQ, CO, Unit<C, POW>...> // Replace CO with calculated coefficient
units(Tag<SeqC<C...>, SeqI<POW...>> = {}) { return {}; }
template<int SQ = -1, int CO  = 1, char... C, int... POW>
auto units(SeqC<C...>, SeqI<POW...> = {})
	-> decltype(units(Tag<SeqC<C...>, SeqI<POW...>>{})) { return {}; }

template<int SQ = -1, int CO = 1, char... C>
Units<SQ, CO, Unit<C, 1>...>
units(std::integral_constant<char, C> ...) { return {}; }



/*template<int N, int SQ, int CO, char... C>
static constexpr auto pow(Units<SQ, CO, SeqC<C...>> const& u, Constexpr<int, N> n = {}) {
	return Units<SQ, SumSeq<SeqI<((N & 2) ? SQ : 1)...> >::value,
		decltype(Unit<C>{} ^ N)...> {};
}*/
template<int N, int SQ, int CO, char... C, int... POW>
static constexpr auto pow(Units<SQ, CO, SeqC<C...>> const& u, Constexpr<int, N> n = {}) {
	return Units<SQ, SumSeq<SeqI<(((N*POW) & 2) ? SQ : 1)...> >::value,
		decltype(Unit<C>{} ^ N)...> {};
}
/*template<int N> static constexpr auto pow(Units const& u, Seq<int, N> n = {}) {
	return Units<SQ, SumSeq<SeqI<(((POW*N) & 2) ? SQ : 1)...> >::value,
		decltype(Unit<C, POW>{} ^ N)...> {};
}
template<class S>
constexpr auto operator^(S const& s) const { return pow(*this, s); }*/



}

#endif
