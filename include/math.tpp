#ifndef MATH_TPP
#define MATH_TPP

#include <sstream>
#include <iomanip>

template<class S, class DELIM>
std::enable_if_t<std::is_arithmetic<S>::value, std::string>
to_string(S const& s, unsigned prec, DELIM delim) {
	std::ostringstream oss;
	if(prec) oss << std::setprecision(prec) << std::fixed;
	oss << s;
	return oss.str();
}
template<class S, unsigned N, class DELIM>
std::string to_string(const S (&sn)[N], unsigned prec, DELIM delim) {
	using namespace std;
	bool met = false;
	std::string out;
	for(auto const& s : sn) {
		if(met) out += delim;
		out += to_string(s, prec, delim);
		met = true;
	}
	return out;
}
template<class S, class T, class ST>
DualQuaternion<ST> operator*(Point<S> const& p, DualQuaternion<T> const& d) {
	return DualQuaternion<ST>(p) * d;
}

#endif
