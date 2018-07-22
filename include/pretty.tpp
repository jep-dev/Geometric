#ifndef PRETTY_TPP
#define PRETTY_TPP

#include "pretty.hpp"

template<class... T>
Pretty<T...>::operator std::string(void) const {
	using namespace std;
	string unknown = "(unknown)", orig = pretty(), lhs, rhs;
#ifdef HAS_FUNCSIG
	lhs = "<", rhs = ">";
#elif defined HAS_PRETTY
	lhs = "{", rhs = "}";
#else
	return "";
#endif
	// Match the earliest opening and latest closing brackets
	auto p0 = orig.find_first_of(lhs), p1 = orig.find_last_of(rhs);
	if(p0 == string::npos || p1 == string::npos) return std::move(unknown);
	p0++; // Offset by the width of the opening character
	auto dp = p1 - p0; // Resulting width
	if(p0 > p1) return std::move(unknown); // Inverted range
	auto result = orig.substr(p0, dp); // Range interior
	// Trim the space possibly used to separate adjacent brackets
	if(result[result.length()-1] == ' ') result.erase(result.length()-1);
	return std::move(result);
}

#endif
