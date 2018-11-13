#ifndef BINOMIAL_HPP
#define BINOMIAL_HPP

#include "sequence.hpp"

template<unsigned D, class S = unsigned>
struct Binomial: Binomial<D-1, S> {
	typedef Binomial<D-1, S> base_type;
	typedef typename base_type::value_type prev_type;
	typedef decltype(Detail::prepend<S, 0>(prev_type{})
			+ Detail::append<S, 0>(prev_type{})) value_type;
};
template<class S>
struct Binomial<0, S> {
	typedef Binomial<0, S> base_type;
	typedef Detail::Seq<S, 1> value_type;
};

template<class S, unsigned D, class T>
S& operator<<(S &os, Binomial<D, T> const&) {
	typedef typename Binomial<D, T>::value_type value_type;
	return os << value_type{}, os;
}

#endif
