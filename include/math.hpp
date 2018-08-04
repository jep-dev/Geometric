#ifndef MATH_HPP
#define MATH_HPP

#include <math.h>
#include <type_traits>
#include <limits>

template<class T>
bool near_zero(T const& t, T least = std::numeric_limits<T>::epsilon()) {
	return t <= least;
}

template<class T>
bool near(T const& t, T const& cmp, T least = std::numeric_limits<T>::epsilon()) {
	return near_zero(t - cmp, least);
}

#endif
