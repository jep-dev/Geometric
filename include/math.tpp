#ifndef MATH_TPP
#define MATH_TPP

#include <sstream>
#include <iomanip>

template<class S>
std::enable_if_t<std::is_arithmetic<S>::value, std::string>
to_string(S const& s) {
	std::ostringstream oss;
	oss << s;
	return oss.str();
}

template<class S>
std::enable_if_t<std::is_arithmetic<S>::value, std::string>
to_string(S const& s, unsigned prec) {
	std::ostringstream oss;
	oss << std::setprecision(prec) << std::fixed << s;
	return oss.str();
}

#endif
