#ifndef MATH_HPP
#define MATH_HPP

#include <algorithm>
#include <math.h>
#include <type_traits>
#include <limits>
#include <utility>

#include "math.hpp"
#include "utility.hpp"

template<class> struct Point;
template<class> struct Quaternion;
template<class> struct DualQuaternion;

/** Stream insertion operator; left generic to support ostringstream, etc. exactly. */
template<class L, class S> L& operator<<(L&, Point<S> const&);
template<class L, class S> L& operator<<(L&, Quaternion<S> const&);
template<class L, class S> L& operator<<(L&, DualQuaternion<S> const&);

template<class S, class DELIM = const char*>
std::string to_string(Point<S> const& d, unsigned prec = 0, DELIM delim = ",");
template<class S, class DELIM = const char*>
std::string to_string(Quaternion<S> const& d, unsigned prec = 0, DELIM delim = "+");
template<class S, class DELIM = const char*>
std::string to_string(DualQuaternion<S> const& d, unsigned prec = 0, DELIM delim = "+");
template<class S, class DELIM = const char*>
std::enable_if_t<std::is_arithmetic<S>::value, std::string>
to_string(S const& s, unsigned prec = 0);

template<class S, class T, class ST = std::common_type_t<S,T>>
DualQuaternion<ST> operator*(Point<S> const& p, DualQuaternion<T> const& d);
template<class S, class T, class ST = std::common_type_t<S,T>>
DualQuaternion<ST> operator*(DualQuaternion<S> const& l, Point<T> const& p);

/** Abstract comparison to zero (mainly intended for floating point types) */
template<class L, class R = L>
bool near_zero(L, R = std::numeric_limits<std::remove_reference_t<L>>::epsilon());

/** Abstract comparison (mainly intended for floating point types) */
template<class L, class R = L, class T = R>
bool near(L, R, T = std::numeric_limits<
		std::remove_reference_t<std::common_type_t<L,R>>>::epsilon());

/** Abstract number of digits in decimal */
template<class T> long numDigits(T const& t);


/* Credit to floating-point-gui.de for the combination of relative and absolute comparisons;
 * I read enough of Bruce Dawson's articles to decide that a "decent" solution will have to do
 * without specific constraints. */
template<class L, class R, class T>
bool near(L lhs, R rhs, T least) {
	using LRT = std::common_type_t<L, R, T>;
	using NL = std::numeric_limits<LRT>;
	auto epsilon = NL::epsilon(), minnorm = NL::min();
	if(lhs == rhs) return true;
	L labs = abs(lhs), rabs = abs(rhs), dabs = abs(lhs - rhs);
	if((labs == 0) || (rabs == 0) || (dabs < minnorm))
		return dabs < (least * minnorm);
	return dabs / std::min(labs + rabs, NL::max()) < least;
}
template<class L, class T>
bool near_zero(L lhs, T least) {
	return near(lhs, 0, least);
}

template<class T>
long numDigits(T const& t) {
	long out = ceil(log10(abs(long(t))+1));
	return out ? out : 1;
}
template<class T>
long numDigits(T const& t, unsigned radix) {
	long out = ceil(log(abs(long(t))+1)/log(radix));
	return out ? out : 1;
}

#endif
