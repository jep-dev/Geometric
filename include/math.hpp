#ifndef MATH_HPP
#define MATH_HPP

#include <math.h>
#include <limits>

#include "utility.hpp"

template<class> struct Point;
template<class> struct Quaternion;
template<class> struct DualQuaternion;

/** Stream insertion operator; left generic to support ostringstream, etc. exactly. */
template<class L, class S> L& operator<<(L&, Point<S> const&);
template<class L, class S> L& operator<<(L&, Quaternion<S> const&);
template<class L, class S> L& operator<<(L&, DualQuaternion<S> const&);

template<class S, class DELIM = const char*>
std::enable_if_t<std::is_arithmetic<S>::value, std::string>
to_string(S s, unsigned prec = 0, bool show_zero = true, bool fill_zeroes = false) {
	using namespace std;
	if(isnan(s))
		return "NaN";
	bool bSign = s < 0;
	// s= abs(s);
	if(bSign) s = -s;

	string out;
	unsigned uFactor = 0, uMajor = s, uDec = uMajor,
			nMajor = 0, nMinor = 0, nZeroes = 0;
	while(uDec) {
		unsigned uCur = uDec % 10, uNext = uDec / 10;
		out = char('0' + uCur) + out;
		uDec = uNext;
		if(uCur) nMajor++;
	}
	if((show_zero && !nMajor)) {
		out = "0";
	}
	s -= uMajor;
	while(prec) {
		s *= 10;
		unsigned uCur = s;
		if(uCur || fill_zeroes) {
			s -= uCur;
			if(!nMinor) out += '.';
			nMinor++;
			if(nZeroes) {
				out += string(nZeroes, '0');
				nZeroes = 0;
			}
			out += char('0' + uCur);
		} else {
			nZeroes++;
		}
		prec--;
	}
	if(!show_zero && out.find("0.") != string::npos)
		out = out.substr(1);
	if(!out.length()) out = "0";
	if(bSign && (nMajor || nMinor))
		return '-' + out;
	return out;
}
template<class S, class DELIM = const char*, class... T>
std::string to_string(Point<S> const& d, unsigned prec = 2, DELIM delim = ", ", T &&... t);
template<class S, class... T>
std::string to_string(Quaternion<S> const& d, T &&... t);
template<class S, class... T>
std::string to_string(DualQuaternion<S> const& d, T &&... t);

template<class S, class... T>
std::enable_if_t<std::is_arithmetic<S>::value, std::string>
to_unit_string(S s, char i, T &&... t) {
	auto out = to_string(s, std::forward<T>(t)...);
	if(out == "0") return "";
	if(out == "1") return std::string("") + i;
	if(out == "-1") return std::string("-") + i;
	return out + i;
}

template<class S, class T, class ST = std::common_type_t<S,T>>
DualQuaternion<ST> operator*(Point<S> const& p, DualQuaternion<T> const& d);
template<class S, class T, class ST = std::common_type_t<S,T>>
DualQuaternion<ST> operator*(DualQuaternion<S> const& l, Point<T> const& p);

template<class S, class T>
S lerp(S const& u, S const& v, T const& t) {
	return u + (v-u)*t;
}
template<class S, class T>
S lerp(S const& u0, S const& u1, S const& v0, S const& v1, T t0, T t1) {
	return (u0*(1-t0)+u1*t0)*(1-t1) + (v0*(1-t0)+v1*t0)*t1;
}

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
